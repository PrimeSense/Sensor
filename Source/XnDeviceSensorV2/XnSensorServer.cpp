/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.0 Alpha                                               *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Common.                                   *
*                                                                            *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  PrimeSense Sensor is distributed in the hope that it will be useful,      *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>. *
*                                                                            *
*****************************************************************************/

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorServer.h"
#include "XnSensorClientServer.h"
#include <XnLog.h>
#include <XnIONetworkStream.h>
#include <XnStringsHash.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_SERVER_ACCEPT_CONNECTION_TIMEOUT		100

//---------------------------------------------------------------------------
// XnSensorServer class
//---------------------------------------------------------------------------
XnSensorServer::XnSensorServer(const XnChar* strConfigFile) :
	m_hListenSocket(NULL),
	m_hServerRunningEvent(NULL),
	m_hServerRunningMutex(NULL),
	m_hSessionsLock(NULL),
	m_nLastClientID(0),
	m_nErrorState(XN_STATUS_OK),
	m_sensorsManager(strConfigFile)
{
}

XnSensorServer::~XnSensorServer()
{
	Free();
}

XnStatus XnSensorServer::Run()
{
	//Initialize server
	XnStatus nRetVal = InitServer();

	if (nRetVal == XN_STATUS_OK)
	{
		//Initialization succeeded - run main loop
		nRetVal = ServerMainLoop();
	}

	Free();
	
	return nRetVal;
}

XnBool XnSensorServer::IsServerRunning()
{
	if (m_hServerRunningEvent == NULL)
	{
		return FALSE;
	}

	//Poll the Server Running event
	return xnOSIsEventSet(m_hServerRunningEvent);
}

XnStatus XnSensorServer::InitServer()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = xnOSCreateNamedMutex(&m_hServerRunningMutex, XN_SENSOR_SERVER_RUNNING_MUTEX_NAME);
	XN_IS_STATUS_OK(nRetVal);

	XnAutoMutexLocker serverRunningLock(m_hServerRunningMutex, XN_SENSOR_SERVER_RUNNING_MUTEX_TIMEOUT);
	nRetVal = serverRunningLock.GetStatus();
	if (nRetVal != XN_STATUS_OK)
	{
		//This could mean there's another server/client that's frozen and they're jamming the mutex...
		xnLogError(XN_MASK_SENSOR_SERVER, "Failed to lock server mutex: %s - exiting.", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return XN_STATUS_OS_MUTEX_TIMEOUT;
	}

	//From now on we're protected by m_hServerRunningMutex until we return from this function

	/*Create the Server Running event. 
	  This is created as a manual-reset event, because only the server resets it when it's shutting down. */
	nRetVal = xnOSOpenNamedEvent(&m_hServerRunningEvent, XN_SENSOR_SERVER_RUNNING_EVENT_NAME);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = xnOSCreateNamedEvent(&m_hServerRunningEvent, XN_SENSOR_SERVER_RUNNING_EVENT_NAME, TRUE);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (IsServerRunning())
	{
		//Another server is already running.
		xnLogInfo(XN_MASK_SENSOR_SERVER, "Detected another server running - exiting.");
		xnOSCloseEvent(&m_hServerRunningEvent);
		m_hServerRunningEvent = NULL;
		return XN_STATUS_DEVICE_SERVER_ALREADY_RUNNING;
	}

	nRetVal = m_sensorsManager.Init();
	XN_IS_STATUS_OK(nRetVal);

	// init network
	nRetVal = xnOSInitNetwork();
	XN_IS_STATUS_OK(nRetVal);

	// create lock
	nRetVal = xnOSCreateCriticalSection(&m_hSessionsLock);
	XN_IS_STATUS_OK(nRetVal);

	// create the listen socket
	nRetVal = xnOSCreateSocket(XN_OS_TCP_SOCKET, XN_SENSOR_SERVER_IP_ADDRESS, XN_SENSOR_SERVER_PORT, &m_hListenSocket);
	XN_IS_STATUS_OK(nRetVal);

	// bind it
	nRetVal = xnOSBindSocket(m_hListenSocket);
	XN_IS_STATUS_OK(nRetVal);

	// start listening
	nRetVal = xnOSListenSocket(m_hListenSocket);
	XN_IS_STATUS_OK(nRetVal);
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Server is now listening");

	/*Set the event to signal that the server is ready for requests. We do this AFTER we start listening so
	  the clients can wait on the event and then connect to the server socket. */
	nRetVal = xnOSSetEvent(m_hServerRunningEvent);
	XN_IS_STATUS_OK(nRetVal);

	xnOSGetTimeStamp(&m_nLastSessionActivity);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::ServerMainLoop()
{
	XnStatus nRetVal = XN_STATUS_OK;

	while (TRUE)
	{
		CheckForNewClients(XN_SENSOR_SERVER_ACCEPT_CONNECTION_TIMEOUT);

		// do some clean-up
		m_sensorsManager.CleanUp();
		CleanUpSessions();

		// now check if we should shutdown
		if (ShutdownIfPossible())
		{
			break;
		}
	}

	return XN_STATUS_OK;
}

void XnSensorServer::CheckForNewClients(XnUInt32 nTimeout)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// run in loop until we break due to timeout
	XN_SOCKET_HANDLE hClientSocket;
	while (TRUE)
	{
		nRetVal = xnOSAcceptSocket(m_hListenSocket, &hClientSocket, nTimeout);
		if (nRetVal == XN_STATUS_OS_NETWORK_TIMEOUT)
		{
			return;
		}
		else if (nRetVal != XN_STATUS_OK)
		{
			//Any other error beside timeout is not expected, but we treat it the same.
			xnLogWarning(XN_MASK_SENSOR_SERVER, "failed to accept connection: %s", xnGetStatusString(nRetVal));
		}
		else
		{
			xnLogInfo(XN_MASK_SENSOR_SERVER, "New client trying to connect...");

			//TODO: Check if we don't have too many clients
			nRetVal = AddSession(hClientSocket);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to add new client: %s", xnGetStatusString(nRetVal));
				xnOSCloseSocket(hClientSocket);
				//Still in loop
			}
		}
	}
}

void XnSensorServer::CleanUpSessions()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnAutoCSLocker locker(m_hSessionsLock);
	if (!m_sessions.IsEmpty())
	{
		XnSessionsList::Iterator it = m_sessions.begin();
		while (it != m_sessions.end())
		{
			XnSessionsList::Iterator curr = it;
			++it;

			XnServerSession* pSession = *curr;
			if (pSession->HasEnded())
			{
				nRetVal = RemoveSession(curr);
				if (nRetVal != XN_STATUS_OK)
				{
					xnLogWarning(XN_MASK_SENSOR_SERVER, "failed to remove session: %s", xnGetStatusString(nRetVal));
				}
			}
		}
	}
}

XnBool XnSensorServer::ShutdownIfPossible()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// lock sessions list
	XnAutoCSLocker locker(m_hSessionsLock);

	// check if no sessions and no sensors
	if (CanShutdown())
	{
		// lock the running lock
		XnAutoMutexLocker serverRunningLock(m_hServerRunningMutex, XN_SENSOR_SERVER_RUNNING_MUTEX_TIMEOUT);
		nRetVal = serverRunningLock.GetStatus();
		if (nRetVal == XN_STATUS_OK)
		{
			// make sure no client is waiting to connect
			CheckForNewClients(0);

			// re-check shutdown condition
			if (CanShutdown())
			{
				xnLogInfo(XN_MASK_SENSOR_SERVER, "No sensors are open and no client is connected. Shutting down...");

				// reset the event (to notify server is no longer up)
				nRetVal = xnOSResetEvent(m_hServerRunningEvent);
				if (nRetVal != XN_STATUS_OK)
				{
					xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to reset sensor server event: %s - proceeding with shutdown.", xnGetStatusString(nRetVal));
					XN_ASSERT(FALSE);
				}

				// and close the socket (to free the port for another server)
				xnOSCloseSocket(m_hListenSocket);
				m_hListenSocket = NULL;

				return TRUE;
			}
		}
	}

	return FALSE;
}

void XnSensorServer::Free()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_hServerRunningEvent != NULL)
	{
		xnOSCloseEvent(&m_hServerRunningEvent);
		m_hServerRunningEvent = NULL;
	}

	if (m_hListenSocket != NULL)
	{
		xnOSCloseSocket(m_hListenSocket);
		m_hListenSocket = NULL;
	}

	if (m_hSessionsLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hSessionsLock);
		m_hSessionsLock = NULL;
	}
}

XnBool XnSensorServer::CanShutdown()
{
	XnUInt64 nNow;
	xnOSGetTimeStamp(&nNow);

	XnAutoCSLocker locker(m_hSessionsLock);
	return (!m_sensorsManager.HasOpenSensors() && m_sessions.IsEmpty() && 
		(nNow - m_nLastSessionActivity) > m_sensorsManager.GetTimeout());
}

void XnSensorServer::ShutdownServer()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnAutoMutexLocker serverRunningLock(m_hServerRunningMutex, XN_SENSOR_SERVER_RUNNING_MUTEX_TIMEOUT);
	nRetVal = serverRunningLock.GetStatus();
	if (nRetVal != XN_STATUS_OK)
	{
		//This could mean there's another server/client that's frozen and they're jamming the mutex...
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to lock server mutex: %s - proceeding with shutdown.", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}

	if (m_hServerRunningEvent != NULL)
	{
		nRetVal = xnOSResetEvent(m_hServerRunningEvent);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to reset sensor server event: %s - proceeding with shutdown.", xnGetStatusString(nRetVal));
			XN_ASSERT(FALSE);
		}

		xnOSCloseEvent(&m_hServerRunningEvent);
		m_hServerRunningEvent = NULL;
	}

	XN_ASSERT(m_sessions.IsEmpty());

	if (m_hListenSocket != NULL)
	{
		xnOSCloseSocket(m_hListenSocket);
		m_hListenSocket = NULL;
	}

	if (m_hSessionsLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hSessionsLock);
		m_hSessionsLock = NULL;
	}
}

XnStatus XnSensorServer::AddSession(XN_SOCKET_HANDLE hClientSocket)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt32 nID = 0;

	{
		XnAutoCSLocker locker(m_hSessionsLock);
		++m_nLastClientID;
		nID = m_nLastClientID;
	}

	// create new session
	XnServerSession* pSession;
	XN_VALIDATE_NEW(pSession, XnServerSession, &m_sensorsManager, nID, hClientSocket, &m_logger);

	nRetVal = pSession->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pSession);
		return (nRetVal);
	}

	// add it to list in a lock
	{
		XnAutoCSLocker locker(m_hSessionsLock);
		nRetVal = m_sessions.AddLast(pSession);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pSession);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::RemoveSession(XnSessionsList::ConstIterator it)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnServerSession* pSession = *it;

	XnUInt32 nID = pSession->ID();
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Removing client %u...", nID);

	{
		XnAutoCSLocker locker(m_hSessionsLock);
		nRetVal = m_sessions.Remove(it);
		XN_IS_STATUS_OK(nRetVal);

		if (m_sessions.IsEmpty())
		{
			xnOSGetTimeStamp(&m_nLastSessionActivity);
		}
	}

	pSession->Free();
	XN_DELETE(pSession);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u removed", nID);

	return (XN_STATUS_OK);
}

XN_DEVICE_API XnStatus XnSensorServerGetGlobalConfigFile(const XnChar* strConfigDir, XnChar* strConfigFile, XnUInt32 nBufSize)
{
	return XnSensor::ResolveGlobalConfigFileName(strConfigFile, nBufSize, strConfigDir);
}

XN_DEVICE_API XnStatus XnSensorServerRun(const XnChar* strConfigFile)
{
	XnSensorServer server(strConfigFile);
	return server.Run();
}

