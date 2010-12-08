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
#include <XnDDK/XnStreamDataInternal.h>
#include <XnStringsHash.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_DEFAULT_SERVER_WAIT_FOR_CLIENT_TIME	10000
#define XN_SENSOR_SERVER_ACCEPT_CONNECTION_TIMEOUT		100
#define XN_SENSOR_TERMINATE_READER_THREAD_TIMEOUT		5000
#define XN_MASK_SENSOR_SERVER_COMM_DUMP					"SensorServerComm"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnDeviceString
{
	XnChar str[XN_DEVICE_MAX_STRING_LENGTH];
} XnDeviceString;

typedef struct XnServerStream 
{
	XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nRefCount;
	XnUInt32 nOpenRefCount;
	XnStreamData* pStreamData;
	XnBool bNewData;
} XnServerStream;

XN_DECLARE_STRINGS_HASH(XnDeviceString, XnNamesHash);
XN_DECLARE_STRINGS_HASH(XnServerStream, XnServerStreamsHash);

struct XnSensorServer::XnClient
{
	XnClient()
	{ 
		nID = 0;
		hSocket = NULL;
		hThread = NULL;
		pStream = NULL; 
		pPrivateIncomingPacker = NULL;
		pPrivateOutgoingPacker = NULL;
		pServer = NULL;
		pStreamDataSet = NULL;
		XnDeviceString deviceModule = { XN_MODULE_NAME_DEVICE };
		clientToServerNames.Set(XN_MODULE_NAME_DEVICE, deviceModule);
		serverToClientNames.Set(XN_MODULE_NAME_DEVICE, deviceModule);
		bShouldRun = TRUE;
	}

	~XnClient() 
	{ 
		XN_DELETE(pPrivateIncomingPacker);
		XN_DELETE(pPrivateOutgoingPacker);
		XN_DELETE(pStream); 
		if (hLock != NULL)
		{
			xnOSCloseCriticalSection(&hLock);
			hLock = NULL;
		}
		if (pStreamDataSet != NULL)
		{
			XnStreamDataSetDestroy(&pStreamDataSet);
		}
		if (hSocket != NULL)
		{
			xnOSCloseSocket(hSocket);
		}
	}

	const XnChar* ToServerModule(const XnChar* strClientModule)
	{
		XnNamesHash::Iterator it = clientToServerNames.end();
		if (XN_STATUS_OK != clientToServerNames.Find(strClientModule, it))
		{
			return NULL;
		}

		return it.Value().str;
	}

	const XnChar* FromServerModule(const XnChar* strServerModule)
	{
		XnNamesHash::Iterator it = serverToClientNames.end();
		if (XN_STATUS_OK != serverToClientNames.Find(strServerModule, it))
		{
			return NULL;
		}

		return it.Value().str;
	}

	XnBool IsStreamOpen(const XnChar* strServerStream)
	{
		XnNamesHash::Iterator it = serverToClientNames.end();
		return (XN_STATUS_OK == openStreams.Find(strServerStream, it));
	}

	XnUInt32 nID;
	XN_SOCKET_HANDLE hSocket;
	XN_THREAD_HANDLE hThread;
	XN_CRITICAL_SECTION_HANDLE hLock;
	XnIONetworkStream* pStream;
	XnDataPacker* pPrivateIncomingPacker;
	XnDataPacker* pPrivateOutgoingPacker;
	XnSensorServer* pServer;
	XnStreamDataSet* pStreamDataSet;
	XnNamesHash clientToServerNames;
	XnNamesHash serverToClientNames;
	volatile XnBool bShouldRun;
	XnNamesHash openStreams;
};

#define XN_SENSOR_SERVER_LOCK_BLOCK						XnAutoCSLocker __locker(m_hSensorLock);
#define XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK				XnAutoCSLocker __locker(pClient->hLock);

//---------------------------------------------------------------------------
// XnSensorServer class
//---------------------------------------------------------------------------
XnSensorServer::XnSensorServer() :
	m_bSensorOpen(FALSE),
	m_hListenSocket(NULL),
	m_hReaderThread(NULL),
	m_hNewDataEvent(NULL),
	m_hServerRunningEvent(NULL),
	m_hServerRunningMutex(NULL),
	m_hSensorLock(NULL),
	m_hBroadcastingLock(NULL),
	m_hClientsCriticalSection(NULL),
	m_nLastClientID(0),
	m_pServerStreams(NULL),
	m_noClientTimeout(XN_MODULE_PROPERTY_SERVER_NO_CLIENTS_TIMEOUT, XN_SENSOR_DEFAULT_SERVER_WAIT_FOR_CLIENT_TIME),
	m_startNewLog(XN_MODULE_PROPERTY_SERVER_START_NEW_LOG_FILE),
	m_nErrorState(XN_STATUS_OK)
{
	m_noClientTimeout.UpdateSetCallbackToDefault();
	m_startNewLog.UpdateSetCallback(StartNewLogCallback, this);

	m_serverDump = XN_DUMP_CLOSED;
	xnDumpInit(&m_serverDump, XN_MASK_SENSOR_SERVER_COMM_DUMP, "TS,Type,Size,Client\n", "%s.csv", XN_MASK_SENSOR_SERVER_COMM_DUMP);
}

XnSensorServer::~XnSensorServer()
{
	ShutdownServer();
}

XnStatus XnSensorServer::Run(const XnChar* strConfigFile)
{
	//Initialize server
	XnStatus nRetVal = InitServer(strConfigFile);

	if (nRetVal == XN_STATUS_OK)
	{
		//Initialization succeeded - run main loop
		nRetVal = ServerMainLoop();
	}

	//Shutdown the server
	ShutdownServer();
	
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

XnStatus XnSensorServer::InitServer(const XnChar* strConfigFile)
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

	nRetVal = m_sensor.SetGlobalConfigFile(strConfigFile);
	XN_IS_STATUS_OK(nRetVal);

	// read default timeout from file
	nRetVal = m_noClientTimeout.ReadValueFromFile(strConfigFile, XN_CONFIG_FILE_SERVER_SECTION);
	XN_IS_STATUS_OK(nRetVal);

	// init network
	nRetVal = xnOSInitNetwork();
	XN_IS_STATUS_OK(nRetVal);

	// create locks
	nRetVal = xnOSCreateCriticalSection(&m_hSensorLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hBroadcastingLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hClientsCriticalSection);
	XN_IS_STATUS_OK(nRetVal);

	// create the listen socket
	nRetVal = xnOSCreateSocket(XN_OS_TCP_SOCKET, XN_SENSOR_SERVER_IP_ADDRESS, XN_SENSOR_SERVER_PORT, &m_hListenSocket);
	XN_IS_STATUS_OK(nRetVal);

	// bind it
	nRetVal = xnOSBindSocket(m_hListenSocket);
	XN_IS_STATUS_OK(nRetVal);

	// register to events
	nRetVal = m_sensor.OnStreamCollectionChangedEvent().Register(StreamCollectionChangedCallback, this);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_sensor.OnNewStreamDataEvent().Register(NewStreamDataCallback, this);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_NEW(m_pServerStreams, XnServerStreamsHash);

	nRetVal = xnOSCreateEvent(&m_hNewDataEvent, FALSE);
	XN_IS_STATUS_OK(nRetVal);

	// start listening
	nRetVal = xnOSListenSocket(m_hListenSocket);
	XN_IS_STATUS_OK(nRetVal);
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Server is now listening");

	/*Set the event to signal that the server is ready for requests. We do this AFTER we start listening so
	  the clients can wait on the event and then connect to the server socket. */
	nRetVal = xnOSSetEvent(m_hServerRunningEvent);
	XN_IS_STATUS_OK(nRetVal);

	// start reader thread
	nRetVal = xnOSCreateThread(ReaderThread, this, &m_hReaderThread);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::OpenSensor(const XnChar* strConnectionString)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDeviceConfig config;
	config.DeviceMode = XN_DEVICE_MODE_READ;
	config.cpConnectionString = strConnectionString;
	config.pInitialValues = NULL;
	config.SharingMode = XN_DEVICE_SHARED;

	nRetVal = m_sensor.Init(&config);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_ADD_PROPERTIES(m_sensor.DeviceModule(), 
		&m_startNewLog, &m_noClientTimeout);

	// configure from global file
	nRetVal = m_sensor.ConfigureModuleFromGlobalFile(XN_MODULE_NAME_DEVICE, XN_CONFIG_FILE_SERVER_SECTION);
	XN_IS_STATUS_OK(nRetVal);

	// register to all properties
	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	nRetVal = m_sensor.DeviceModule()->GetAllProperties(&props);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = RegisterToProps(&props);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::ServerMainLoop()
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt64 nNow = 0;
	XnUInt64 nLastClientRemovedTimestamp = 0;
	XnUInt64 nTimeSinceNoClients = 0;
	XnBool bQuit = FALSE;
	XN_SOCKET_HANDLE hClientSocket = NULL;
	XnBool bNoClients = FALSE;

	xnOSGetTimeStamp(&nLastClientRemovedTimestamp);

	while (!bQuit)
	{
		nRetVal = xnOSAcceptSocket(m_hListenSocket, &hClientSocket, XN_SENSOR_SERVER_ACCEPT_CONNECTION_TIMEOUT);
		if (nRetVal == XN_STATUS_OK)
		{
			xnLogInfo(XN_MASK_SENSOR_SERVER, "New client trying to connect...");

			//TODO: Check if we don't have too many clients
			nRetVal = AddClient(hClientSocket);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to add new client: %s", xnGetStatusString(nRetVal));
				xnOSCloseSocket(hClientSocket);
				//Still in loop
			}
		}
		else // no client trying to connect, do some clean up work
		{
			if (nRetVal != XN_STATUS_OS_NETWORK_TIMEOUT)
			{
				//Any other error beside timeout is not expected, but we treat it the same.
				xnLogWarning(XN_MASK_SENSOR_SERVER, "failed to accept connection: %s", xnGetStatusString(nRetVal));
			}

			// remove all non-active clients
			xnOSEnterCriticalSection(&m_hClientsCriticalSection);

			if (!m_clients.IsEmpty())
			{
				// create a list of all clients to be removed
				XnClientsList toRemove;
				for (XnClientsList::Iterator it = m_clients.begin(); it != m_clients.end(); ++it)
				{
					XnClient* pClient = *it;
					if (!pClient->bShouldRun)
					{
						toRemove.AddLast(pClient);
					}
				}

				// now actually remove them
				for (XnClientsList::Iterator it = toRemove.begin(); it != toRemove.end(); ++it)
				{
					XnClient* pClient = *it;
					RemoveClient(pClient);
				}

				// check if there are any clients left
				if (m_clients.IsEmpty())
				{
					xnOSGetTimeStamp(&nLastClientRemovedTimestamp);

					// do some clean-up (so that next client will behave as if it started the server)
					ReturnToDefaults();
				}
			}

			xnOSLeaveCriticalSection(&m_hClientsCriticalSection);

			// if too much time has passed and no client has connected, shut down
			xnOSEnterCriticalSection(&m_hClientsCriticalSection);
			bNoClients = m_clients.IsEmpty();
			xnOSLeaveCriticalSection(&m_hClientsCriticalSection);
			
			xnOSGetTimeStamp(&nNow);

			nTimeSinceNoClients = (nNow - nLastClientRemovedTimestamp);
			if (bNoClients && (nTimeSinceNoClients >= m_noClientTimeout.GetValue()))
			{
				xnLogInfo(XN_MASK_SENSOR_SERVER, "No client connection for %u ms. Shutting down...", m_noClientTimeout.GetValue());
				bQuit = TRUE;
			}
		}
	}

	return XN_STATUS_OK;
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

	nRetVal = m_sensor.Destroy();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to destroy sensor: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}

	XN_DELETE(m_pServerStreams);
	m_pServerStreams = NULL;

	for (XnPropertySetData::Iterator it = m_allStreamsProps.begin(); it != m_allStreamsProps.end(); ++it)
	{
		XN_DELETE(it.Value());
	}
	m_allStreamsProps.Clear();

	XN_ASSERT(m_clients.IsEmpty());

	if (m_hNewDataEvent != NULL)
	{
		xnOSCloseEvent(&m_hNewDataEvent);
		m_hNewDataEvent = NULL;
	}

	if (m_hReaderThread != NULL)
	{
		xnOSWaitAndTerminateThread(&m_hReaderThread, XN_SENSOR_TERMINATE_READER_THREAD_TIMEOUT);
		m_hReaderThread = NULL;
	}

	if (m_hListenSocket != NULL)
	{
		xnOSCloseSocket(m_hListenSocket);
		m_hListenSocket = NULL;
	}

	if (m_hClientsCriticalSection != NULL)
	{
		xnOSCloseCriticalSection(&m_hClientsCriticalSection);
		m_hClientsCriticalSection = NULL;
	}

	if (m_hBroadcastingLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hBroadcastingLock);
		m_hBroadcastingLock = NULL;
	}

	if (m_hSensorLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hSensorLock);
		m_hSensorLock = NULL;
	}
}

XnStatus XnSensorServer::RegisterToProps(XnPropertySet* pProps)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	for (XnPropertySetData::Iterator itMod = pProps->pData->begin(); itMod != pProps->pData->end(); ++itMod)
	{
		XnActualPropertiesHash* pHash = itMod.Value();

		XnDeviceModule* pModule;
		nRetVal = m_sensor.FindModule(itMod.Key(), &pModule);
		XN_IS_STATUS_OK(nRetVal);

		for (XnActualPropertiesHash::Iterator itProp = pHash->begin(); itProp != pHash->end(); ++itProp)
		{
			XnProperty* pProp;
			nRetVal = pModule->GetProperty(itProp.Key(), &pProp);
			XN_IS_STATUS_OK(nRetVal);

			// no need to keep the handle. We only want to unregister when the stream is destroyed, and then
			// it happens anyway.
			nRetVal = pProp->OnChangeEvent().Register(PropertyChangedCallback, this);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::OnNewServerEvent(const XnUChar* pData, XnUInt32 nDataSize, XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = pClient->pStream->WriteData(pData, nDataSize);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

void XnSensorServer::DumpMessage(const XnChar* strType, XnUInt32 nSize /* = 0 */, XnUInt32 nClientID /* = 0 */, const XnChar* strComment /* = "" */ )
{
	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);

	xnDumpWriteString(m_serverDump, "%llu,%s,%d,%d,%s\n", nNow, strType, nSize, nClientID, strComment);
}

XnStatus XnSensorServer::OnStreamAdded(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get all props
	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	nRetVal = m_sensor.GetAllProperties(&props, FALSE, StreamName);
	XN_IS_STATUS_OK(nRetVal);

	// register to all props
	nRetVal = RegisterToProps(&props);
	XN_IS_STATUS_OK(nRetVal);

	XnActualPropertiesHash* pStreamProps = props.pData->begin().Value();

	// take type
	XnProperty* pProp = NULL;
	nRetVal = pStreamProps->Get(XN_STREAM_PROPERTY_TYPE, pProp);
	XN_IS_STATUS_OK(nRetVal);

	XnActualStringProperty* pTypeProp = (XnActualStringProperty*)pProp;

	nRetVal = XnPropertySetDataDetachModule(props.pData, StreamName, &pStreamProps);
	XN_IS_STATUS_OK(nRetVal);

	// add it to property list
	nRetVal = XnPropertySetDataAttachModule(&m_allStreamsProps, StreamName, pStreamProps);
	XN_IS_STATUS_OK(nRetVal);

	// create stream data
	XnServerStream serverStream;
	xnOSMemSet(&serverStream, 0, sizeof(serverStream));
	strcpy(serverStream.strType, StreamName);
	nRetVal = m_sensor.CreateStreamData(StreamName, &serverStream.pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pServerStreams->Set(StreamName, serverStream);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::OnStreamRemoved(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// no need to unregister from its props - they do not exist anymore.

	// remove stream data
	XnServerStream* pServerStream;
	nRetVal = m_pServerStreams->Get(StreamName, pServerStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_sensor.DestroyStreamData(&pServerStream->pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pServerStreams->Remove(StreamName);
	XN_IS_STATUS_OK(nRetVal);

	// remove from our list
	XnActualPropertiesHash* pStreamProps = NULL;
	nRetVal = XnPropertySetDataDetachModule(&m_allStreamsProps, StreamName, &pStreamProps);
	XN_IS_STATUS_OK(nRetVal);

	XN_DELETE(pStreamProps);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::OnStreamCollectionChanged(const XnChar* StreamName, XnStreamsChangeEventType EventType)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	switch (EventType)
	{
	case XN_DEVICE_STREAM_ADDED:
		{
			nRetVal = OnStreamAdded(StreamName);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_DEVICE_STREAM_DELETED:
		{
			nRetVal = OnStreamRemoved(StreamName);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "unknown event: %d", EventType);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::OnPropertyChanged(const XnProperty* pProp)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt64 nValue = 0;
	XnDouble dValue = 0;
	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	XnGeneralBuffer gbValue = {NULL, 0};
	XnBool bPassPropToClients = TRUE;

	switch (pProp->GetType())
	{
		case XN_PROPERTY_TYPE_INTEGER:
		{
			XnIntProperty* pIntProp = (XnIntProperty*)pProp;
			nRetVal = pIntProp->GetValue(&nValue);
			XN_IS_STATUS_OK(nRetVal);
			nRetVal = OnIntPropertyChangedInternally(pProp->GetName(), nValue, bPassPropToClients);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
		case XN_PROPERTY_TYPE_REAL:
		{
			XnRealProperty* pRealProp = (XnRealProperty*)pProp;
			nRetVal = pRealProp->GetValue(&dValue);
			XN_IS_STATUS_OK(nRetVal);
			nRetVal = OnRealPropertyChangedInternally(pProp->GetName(), dValue, bPassPropToClients);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
		case XN_PROPERTY_TYPE_STRING:
		{
			XnStringProperty* pStringProp = (XnStringProperty*)pProp;
			nRetVal = pStringProp->GetValue(strValue);
			XN_IS_STATUS_OK(nRetVal);
			nRetVal = OnStringPropertyChangedInternally(pProp->GetName(), strValue, bPassPropToClients);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
		case XN_PROPERTY_TYPE_GENERAL:
		{
			XnGeneralProperty* pGenProp = (XnGeneralProperty*)pProp;

			// find its size from properties map
			XnActualPropertiesHash* pModule;
			nRetVal = m_allStreamsProps.Get(pProp->GetModule(), pModule);
			XN_IS_STATUS_OK(nRetVal);

			XnProperty* pActualProp;
			nRetVal = pModule->Get(pProp->GetName(), pActualProp);
			XN_IS_STATUS_OK(nRetVal);

			XnActualGeneralProperty* pActualGenProp = (XnActualGeneralProperty*)pActualProp;

			// allocate
			nRetVal = XnGeneralBufferAlloc(&gbValue, pActualGenProp->GetValue().nDataSize);
			XN_IS_STATUS_OK(nRetVal);

			// take value
			nRetVal = pGenProp->GetValue(gbValue);
			if (nRetVal != XN_STATUS_OK)
			{
				XnGeneralBufferFree(&gbValue);
				return nRetVal;
			}

			nRetVal = OnGeneralPropertyChangedInternally(pProp->GetName(), gbValue, bPassPropToClients);
			if (nRetVal != XN_STATUS_OK)
			{
				XnGeneralBufferFree(&gbValue);
				return nRetVal;
			}

			break;
		}
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Unknown property type: %d", pProp->GetType());
	}

	if (bPassPropToClients)
	{
		xnOSEnterCriticalSection(&m_hClientsCriticalSection);
		for (XnClientsList::Iterator it = m_clients.begin(); it != m_clients.end(); ++it)
		{
			XnClient* pClient = *it;
			const XnChar* strModule = pClient->FromServerModule(pProp->GetModule());
			if (strModule == NULL)
			{
				continue;
			}

			// send message
			DumpMessage("PropChange", 0, pClient->nID, pProp->GetName());
			switch (pProp->GetType())
			{
			case XN_PROPERTY_TYPE_INTEGER:
				{
					XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;
					nRetVal = pClient->pPrivateOutgoingPacker->WriteProperty(strModule, pProp->GetName(), nValue);
					break;
				}
			case XN_PROPERTY_TYPE_REAL:
				{
					XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;
					nRetVal = pClient->pPrivateOutgoingPacker->WriteProperty(strModule, pProp->GetName(), dValue);
					break;
				}
			case XN_PROPERTY_TYPE_STRING:
				{
					XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;
					nRetVal = pClient->pPrivateOutgoingPacker->WriteProperty(strModule, pProp->GetName(), strValue);
					break;
				}
			case XN_PROPERTY_TYPE_GENERAL:
				{
					XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;
					nRetVal = pClient->pPrivateOutgoingPacker->WriteProperty(strModule, pProp->GetName(), gbValue);
					break;
				}
			}
			
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSLeaveCriticalSection(&m_hClientsCriticalSection);
				XnGeneralBufferFree(&gbValue);
				return nRetVal;
			}
		} // clients loop
		xnOSLeaveCriticalSection(&m_hClientsCriticalSection);
	}

	// free allocated buffer
	XnGeneralBufferFree(&gbValue);
	return XN_STATUS_OK;
}

XnStatus XnSensorServer::ReadStreams()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	while (IsServerRunning())
	{
		nRetVal = xnOSWaitEvent(m_hNewDataEvent, XN_NODE_WAIT_FOR_DATA_TIMEOUT);
		if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
		{
			continue;
		}

		if (nRetVal != XN_STATUS_OK)
		{
			xnLogWarning(XN_MASK_SENSOR_SERVER, "Got error waiting for new data event: %s", xnGetStatusString(nRetVal));
		}

		typedef struct
		{
			XnUInt64 nTimestamp;
			XnUInt32 nFrameID;
			XnChar strName[XN_MAX_NAME_LENGTH];
		} NewStreamData;

		NewStreamData aNewStreamData[5];
		XnUInt32 nCount = 0;

		// read streams with lock
		{
			XN_SENSOR_SERVER_LOCK_BLOCK;
			for (XnServerStreamsHash::Iterator it = m_pServerStreams->begin(); it != m_pServerStreams->end(); ++it)
			{
				XnServerStream& stream = it.Value();

				if (stream.bNewData)
				{
					// ignore audio (it is read by every client)
					if (strcmp(stream.strType, XN_STREAM_NAME_AUDIO) != 0)
					{
						// read this data
						nRetVal = m_sensor.ReadStream(stream.pStreamData);
						if (nRetVal != XN_STATUS_OK)
						{
							xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed reading from stream %s (though event was raised): %s", stream.strType, xnGetStatusString(nRetVal));
							stream.bNewData = FALSE;
							continue;
						}
					}

					stream.bNewData = FALSE;

					aNewStreamData[nCount].nTimestamp = stream.pStreamData->nTimestamp;
					aNewStreamData[nCount].nFrameID = stream.pStreamData->nFrameID;
					strcpy(aNewStreamData[nCount].strName, stream.strType);
					nCount++;
				}
			} // streams loop
		} // lock

		// now send notifications (outside of server lock)
		for (XnUInt32 i = 0; i < nCount; ++i)
		{
			xnOSEnterCriticalSection(&m_hClientsCriticalSection);
			for (XnClientsList::Iterator it = m_clients.begin(); it != m_clients.end(); ++it)
			{
				XnClient* pClient = *it;

				// check if client has stream open
				if (!pClient->IsStreamOpen(aNewStreamData[i].strName))
				{
					continue;
				}

				// notify about the new data
				const XnChar* strStreamName = pClient->FromServerModule(aNewStreamData[i].strName);
				if (strStreamName != NULL)
				{
					XnSensorServerNewStreamData message;
					xnOSMemSet(&message, 0, sizeof(message));
					message.nTimestamp = aNewStreamData[i].nTimestamp;
					message.nFrameID = aNewStreamData[i].nFrameID;
					strcpy(message.strStreamName, strStreamName);
					DumpMessage("NewData", sizeof(XnSensorServerNewStreamData), 0, strStreamName);
					XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;
					nRetVal = pClient->pPrivateOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_NEW_STREAM_DATA, &message, sizeof(XnSensorServerNewStreamData));
					if (nRetVal != XN_STATUS_OK)
					{
						xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed sending new data event to client %d", pClient->nID);
					} //if (nRetVal != XN_STATUS_OK)
				} //if (strStreamName != NULL)
			} //for (clients)
			xnOSLeaveCriticalSection(&m_hClientsCriticalSection);
		}

	} //while (m_bShouldRun)
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::OnNewStreamData(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnServerStream* pStream;
	nRetVal = m_pServerStreams->Get(StreamName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	pStream->bNewData = TRUE;

	nRetVal = xnOSSetEvent(m_hNewDataEvent);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::SendInitialState(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_PROPERTY_SET_CREATE_ON_STACK(props);

	// get it
	nRetVal = m_sensor.GetAllProperties(&props, TRUE);
	XN_IS_STATUS_OK(nRetVal);

	// and send it
	DumpMessage("InitialState", 0, pClient->nID);
	XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;
	nRetVal = pClient->pPrivateOutgoingPacker->WritePropertySet(&props);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::AddClient(XN_SOCKET_HANDLE hClientSocket)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// create new client
	XnClient* pClient;
	XN_VALIDATE_NEW(pClient, XnClient);
	pClient->hSocket = hClientSocket;
	pClient->pServer = this;
	pClient->pStream = XN_NEW(XnIONetworkStream, pClient->hSocket);
	pClient->pPrivateIncomingPacker = XN_NEW(XnDataPacker, pClient->pStream, XN_SENSOR_SERVER_CONFIG_PACKER_SIZE);
	pClient->pPrivateOutgoingPacker = XN_NEW(XnDataPacker, pClient->pStream, XN_SENSOR_SERVER_CONFIG_PACKER_SIZE);

	if (pClient->pStream == NULL || pClient->pPrivateIncomingPacker == NULL || pClient->pPrivateOutgoingPacker == NULL)
	{
		XN_DELETE(pClient);
		return (XN_STATUS_ALLOC_FAILED);
	}

	nRetVal = XnStreamDataSetCreate(&pClient->pStreamDataSet);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pClient);
		return (XN_STATUS_ALLOC_FAILED);
	}

	nRetVal = xnOSCreateCriticalSection(&pClient->hLock);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pClient);
		return (nRetVal);
	}

	nRetVal = pClient->pPrivateIncomingPacker->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pClient);
		return (nRetVal);
	}

	nRetVal = pClient->pPrivateOutgoingPacker->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pClient);
		return (nRetVal);
	}

	// lock the device (so that configuration wouldn't change before we register to it all)
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;

		m_nLastClientID++;
		pClient->nID = m_nLastClientID;

		// create thread
		nRetVal = xnOSCreateThread(ClientThread, pClient, &pClient->hThread);
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSLeaveCriticalSection(&m_hSensorLock);
			XN_DELETE(pClient);
			return (nRetVal);
		}

		// add it to clients list
		xnOSEnterCriticalSection(&m_hClientsCriticalSection);
		m_clients.AddLast(pClient);
		xnOSLeaveCriticalSection(&m_hClientsCriticalSection);

	} // lock end

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::RemoveClientStream(XnClient* pClient, const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	const XnChar* strServerName = pClient->ToServerModule(strName);
	if (strServerName == NULL)
	{
		return XN_STATUS_NO_MATCH;
	}

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u is releasing stream %s", pClient->nID, strServerName);

	// first of all, if client had a buffer of this stream, release it
	XnStreamData* pStreamData = NULL;
	nRetVal = XnStreamDataSetGet(pClient->pStreamDataSet, strServerName, &pStreamData);
	if (nRetVal == XN_STATUS_OK)
	{
		XnSharedMemoryBufferPool* pBufferPool = NULL;
		nRetVal = m_sensor.GetSharedBufferPool(strServerName, &pBufferPool);
		XN_IS_STATUS_OK(nRetVal);

		if (pStreamData->pInternal->pLockedBuffer != NULL)
		{
			pBufferPool->DecRef(pStreamData->pInternal->pLockedBuffer);
		}

		// free data
		XnStreamDataSetRemove(pClient->pStreamDataSet, pStreamData);
		XnStreamDataDestroy(&pStreamData);
	}

	// and dec ref it
	XnServerStream* pStream;
	nRetVal = m_pServerStreams->Get(strServerName, pStream);
	if (nRetVal == XN_STATUS_OK)
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		--pStream->nRefCount;

		xnLogVerbose(XN_MASK_SENSOR_SERVER, "Stream %s now has %u clients", strServerName, pStream->nRefCount);

		if (pStream->nRefCount == 0)
		{
			m_sensor.CloseStream(strServerName);
			m_sensor.DestroyStream(strServerName);

			// the rest will be done in the OnStreamRemoved event handler...
		}
	}

	// now remove it from client
	pClient->serverToClientNames.Remove(strServerName);
	pClient->clientToServerNames.Remove(strName);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::RemoveClient(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Removing client %u...", pClient->nID);
	
	pClient->bShouldRun = FALSE;

	// dec ref of every stream
	while (!pClient->clientToServerNames.IsEmpty())
	{
		RemoveClientStream(pClient, pClient->clientToServerNames.begin().Key());
	}

	xnOSEnterCriticalSection(&m_hClientsCriticalSection);
	XnClientsList::ConstIterator it = m_clients.Find(pClient);
	m_clients.Remove(it);
	xnOSLeaveCriticalSection(&m_hClientsCriticalSection);

	xnOSWaitAndTerminateThread(&pClient->hThread, 3000);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u removed", pClient->nID);
	XN_DELETE(pClient);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::ReturnToDefaults()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// This is a bit ugly, but we need to manually set back to default DEVICE properties
	// (we know there aren't any streams and clients, but the Device module always remains)
	nRetVal = m_sensor.SetProperty(XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_FRAME_SYNC, (XnUInt64)FALSE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_sensor.ConfigureModuleFromGlobalFile(XN_MODULE_NAME_DEVICE);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::SendReply(XnClient* pClient, XnSensorServerCustomMessages Type, XnStatus nRC, XnUInt32 nDataSize /* = 0 */, void* pAdditionalData /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUChar message[XN_SENSOR_SERVER_MAX_REPLY_SIZE];
	XnSensorClientServerReply* pReply = (XnSensorClientServerReply*)message;
	pReply->nRetVal = nRC;
	pReply->Type = Type;
	pReply->nDataSize = nDataSize;
	xnOSMemCopy(pReply->pData, pAdditionalData, nDataSize);
	XnUChar* pEnd = pReply->pData + nDataSize;

	DumpMessage("Reply", nDataSize, pClient->nID);

	// lock this so that messages don't mix up
	XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;

	nRetVal = pClient->pPrivateOutgoingPacker->WriteCustomData(Type, message, pEnd - message);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleOpenSensor(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strConnectionString[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nDataSize = XN_DEVICE_MAX_STRING_LENGTH;
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(XN_SENSOR_SERVER_MESSAGE_OPEN_SENSOR, strConnectionString, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to open sensor %s", pClient->nID, strConnectionString);

	// make sure sensor is open
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		if (!m_bSensorOpen)
		{
			nRetVal = OpenSensor(strConnectionString);
			m_bSensorOpen = (nRetVal == XN_STATUS_OK);
		}

		if (nRetVal == XN_STATUS_OK)
		{
			// sensor is open. send client its initial state
			nRetVal = SendInitialState(pClient);
		}
	}

	// if an error occurred, send it to the client
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleSetIntProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt64 nValue;
	nRetVal = pClient->pPrivateIncomingPacker->ReadProperty(strModule, strProp, &nValue);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", pClient->nID, strModule, strProp);

	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		nRetVal = m_sensor.SetProperty(pClient->ToServerModule(strModule), strProp, nValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleSetRealProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnDouble dValue;
	nRetVal = pClient->pPrivateIncomingPacker->ReadProperty(strModule, strProp, &dValue);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", pClient->nID, strModule, strProp);

	// set
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		nRetVal = m_sensor.SetProperty(pClient->ToServerModule(strModule), strProp, dValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleSetStringProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = pClient->pPrivateIncomingPacker->ReadProperty(strModule, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", pClient->nID, strModule, strProp);

	// set
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		nRetVal = m_sensor.SetProperty(pClient->ToServerModule(strModule), strProp, strValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleSetGeneralProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnGeneralBuffer gbValue;
	nRetVal = pClient->pPrivateIncomingPacker->ReadProperty(strModule, strProp, &gbValue);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", pClient->nID, strModule, strProp);

	// set
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		nRetVal = m_sensor.SetProperty(pClient->ToServerModule(strModule), strProp, gbValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleGetIntProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XnSensorServerMessageGetPropertyRequest request;
	XnUInt32 nDataSize = sizeof(request);
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY, &request, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);
	if (nDataSize != sizeof(request))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	XnUInt64 nValue;
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;

		nRetVal = m_sensor.GetProperty(pClient->ToServerModule(request.strModuleName), request.strPropertyName, &nValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY, nRetVal, sizeof(nValue), &nValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleGetRealProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnSensorServerMessageGetPropertyRequest request;
	XnUInt32 nDataSize = sizeof(request);
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY, &request, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);
	if (nDataSize != sizeof(request))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	XnDouble dValue;
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;

		nRetVal = m_sensor.GetProperty(pClient->ToServerModule(request.strModuleName), request.strPropertyName, &dValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY, nRetVal, sizeof(dValue), &dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleGetStringProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnSensorServerMessageGetPropertyRequest request;
	XnUInt32 nDataSize = sizeof(request);
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY, &request, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);
	if (nDataSize != sizeof(request))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;

		nRetVal = m_sensor.GetProperty(pClient->ToServerModule(request.strModuleName), request.strPropertyName, strValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY, nRetVal, sizeof(strValue), strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleGetGeneralProperty(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnUChar bufValue[XN_SENSOR_SERVER_MAX_REPLY_SIZE];
	XnSensorServerMessageGetPropertyRequest* pRequest = (XnSensorServerMessageGetPropertyRequest*)bufValue;
	XnUChar* pData = bufValue + sizeof(XnSensorServerMessageGetPropertyRequest);
	XnUInt32 nDataSize = XN_SENSOR_SERVER_MAX_REPLY_SIZE;
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY, bufValue, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	if (nDataSize < sizeof(XnSensorServerMessageGetPropertyRequest))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	XnGeneralBuffer gbValue = XnGeneralBufferPack(pData, pRequest->nSize);
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		nRetVal = m_sensor.GetProperty(pClient->ToServerModule(pRequest->strModuleName), pRequest->strPropertyName, gbValue);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY, nRetVal, pRequest->nSize, pData);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleConfigFromINIFile(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XnSensorServerMessageIniFile message;
	XnUInt32 nDataSize = sizeof(message);
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(XN_SENSOR_SERVER_MESSAGE_INI_FILE, (XnUChar*)&message, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	if (nDataSize != sizeof(message))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		nRetVal = m_sensor.LoadConfigFromFile(message.strFileName, message.strSectionName);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleBatchConfig(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XN_PROPERTY_SET_CREATE_ON_STACK(props);

	nRetVal = pClient->pPrivateIncomingPacker->ReadPropertySet(&props);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested a batch config", pClient->nID);

	XN_PROPERTY_SET_CREATE_ON_STACK(serverProps);
	for (XnPropertySetData::Iterator it = props.pData->begin(); it != props.pData->end(); ++it)
	{
		if (nRetVal == XN_STATUS_OK)
		{
			nRetVal = XnPropertySetCloneModule(&props, &serverProps, it.Key(), pClient->ToServerModule(it.Key()));
		}
	}

	if (nRetVal == XN_STATUS_OK)
	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		nRetVal = m_sensor.BatchConfig(&serverProps);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleNewStream(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XN_PROPERTY_SET_CREATE_ON_STACK(props);

	XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = pClient->pPrivateIncomingPacker->ReadNewStream(strType, strName, &props);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to create stream '%s' (%s)", pClient->nID, strName, strType);

	XnPropertySet* pInitialValues = &props;
	if (props.pData->begin() == props.pData->end())
	{
		pInitialValues = NULL;
	}

	// if a stream of this type already exists, take it instead
	XnServerStream* pServerStream;

	XN_SENSOR_SERVER_LOCK_BLOCK;

	if (XN_STATUS_OK == m_pServerStreams->Get(strType, pServerStream))
	{
		xnLogVerbose(XN_MASK_SENSOR_SERVER, "Stream %s already exists.", strType);
		// configure it
		if (pInitialValues != NULL)
		{
			nRetVal = m_sensor.BatchConfig(pInitialValues);
		}
	}
	else
	{
		// create the stream
		nRetVal = m_sensor.CreateStream(strType, strType, pInitialValues);

		if (nRetVal == XN_STATUS_OK)
		{
			nRetVal = m_pServerStreams->Get(strType, pServerStream);
		}
	}

	// send client the new stream data
	if (nRetVal == XN_STATUS_OK)
	{
		XN_PROPERTY_SET_CREATE_ON_STACK(streamProps);
		XN_PROPERTY_SET_CREATE_ON_STACK(clientStreamProps);

		if (nRetVal == XN_STATUS_OK)
		{
			// take properties
			nRetVal = m_sensor.GetAllProperties(&streamProps, FALSE, strType);
		}

		if (nRetVal == XN_STATUS_OK)
		{
			// copy relevant ones
			nRetVal = XnPropertySetCloneModule(&streamProps, &clientStreamProps, strType, strName);
		}

		if (nRetVal == XN_STATUS_OK)
		{
			// now change the state property. It should be OFF, and not the real value (each client has
			// its own stream state).
			nRetVal = XnPropertySetRemoveProperty(&clientStreamProps, strName, XN_STREAM_PROPERTY_STATE);
		}

		if (nRetVal == XN_STATUS_OK)
		{
			nRetVal = XnPropertySetAddIntProperty(&clientStreamProps, strName, XN_STREAM_PROPERTY_STATE, FALSE);
		}

		if (nRetVal == XN_STATUS_OK)
		{
			DumpMessage("NewStream", 0, pClient->nID, strName);
			{
				XN_SENSOR_SERVER_CLIENT_LOCK_BLOCK;
				nRetVal = pClient->pPrivateOutgoingPacker->WriteNewStream(strType, strName, &clientStreamProps);
			}
		}
	}

	// add ref to it
	if (nRetVal == XN_STATUS_OK)
	{
		xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u adding ref to stream '%s'", pClient->nID, strType);
		++pServerStream->nRefCount;
		xnLogVerbose(XN_MASK_SENSOR_SERVER, "Stream %s now has %u clients.", strType, pServerStream->nRefCount);
	}

	// now add it to client data
	XnStreamData* pStreamData = NULL;
	if (nRetVal == XN_STATUS_OK)
	{
		// add names mapping
		XnDeviceString str;
		strcpy(str.str, strType);
		pClient->clientToServerNames.Set(strName, str);
		strcpy(str.str, strName);
		pClient->serverToClientNames.Set(strType, str);

		// create client stream data
		nRetVal = m_sensor.CreateStreamData(strType, &pStreamData);
	}

	if (nRetVal == XN_STATUS_OK)
	{
		// and add it to set
		nRetVal = XnStreamDataSetAdd(pClient->pStreamDataSet, pStreamData);
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleRemoveStream(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = pClient->pPrivateIncomingPacker->ReadStreamRemoved(strName);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to close stream %s", pClient->nID, strName);

	nRetVal = RemoveClientStream(pClient, strName);

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleReadStream(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nDataSize = XN_DEVICE_MAX_STRING_LENGTH;
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(XN_SENSOR_SERVER_MESSAGE_READ_STREAM, strStreamName, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	const XnChar* strServerName = pClient->ToServerModule(strStreamName);

	// take client stream data and server stream data
	XnStreamData* pStreamData;
	nRetVal = XnStreamDataSetGet(pClient->pStreamDataSet, strServerName, &pStreamData);

	XnServerStream* pServerStream = NULL;

	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = m_pServerStreams->Get(strServerName, pServerStream);
	}

	XnSharedMemoryBufferPool* pBufferPool = NULL;
	XnBuffer* pDecRef = NULL;
	if (nRetVal == XN_STATUS_OK)
	{
		pDecRef = pStreamData->pInternal->pLockedBuffer;
		nRetVal = m_sensor.GetSharedBufferPool(strServerName, &pBufferPool);
	}

	if (nRetVal == XN_STATUS_OK)
	{
		// take server data for this stream, and add ref to it
		XN_SENSOR_SERVER_LOCK_BLOCK;
		pStreamData->nDataSize = pServerStream->pStreamData->nDataSize;
		pStreamData->nFrameID = pServerStream->pStreamData->nFrameID;
		pStreamData->nTimestamp = pServerStream->pStreamData->nTimestamp;
		pStreamData->pData = pServerStream->pStreamData->pData;
		pStreamData->pInternal->pLockedBuffer = pServerStream->pStreamData->pInternal->pLockedBuffer;

		if (pStreamData->pInternal->pLockedBuffer != NULL)
		{
			pBufferPool->AddRef(pStreamData->pInternal->pLockedBuffer);
		}

		if (pDecRef != NULL)
		{
			pBufferPool->DecRef(pDecRef);
		}

		XnSensorServerReadReply reply;
		reply.nDataSize = pStreamData->nDataSize;
		reply.nFrameID = pStreamData->nFrameID;
		reply.nTimestamp = pStreamData->nTimestamp;
		reply.nOffset = pBufferPool->GetBufferOffset(pStreamData->pInternal->pLockedBuffer);

		DumpMessage("Data", sizeof(reply), 0, strStreamName);
		nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_READ_STREAM, XN_STATUS_OK, sizeof(reply), &reply);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::HandleSetStreamState(XnClient* pClient, XnBool bRequestOpen)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nDataSize = XN_DEVICE_MAX_STRING_LENGTH;
	XnUInt32 nType = bRequestOpen ? XN_SENSOR_SERVER_MESSAGE_OPEN_STREAM : XN_SENSOR_SERVER_MESSAGE_CLOSE_STREAM;
	nRetVal = pClient->pPrivateIncomingPacker->ReadCustomData(nType, strStreamName, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	const XnChar* strServerName = pClient->ToServerModule(strStreamName);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to %s stream %s", pClient->nID, bRequestOpen ? "open" : "close", strServerName);

	{
		XN_SENSOR_SERVER_LOCK_BLOCK;
		XnServerStream* pServerStream = NULL;

		if (nRetVal == XN_STATUS_OK)
		{
			nRetVal = m_pServerStreams->Get(strServerName, pServerStream);
		}

		if (nRetVal == XN_STATUS_OK)
		{
			// check if this stream is currently open for this client (meaning, client
			// receives its data)
			XnBool bIsOpenForClient = FALSE;
			XnNamesHash::Iterator it = pClient->openStreams.end();
			bIsOpenForClient = (pClient->openStreams.Find(strServerName, it) == XN_STATUS_OK);

			if (bRequestOpen && !bIsOpenForClient)
			{
				XnDeviceString dummy;
				nRetVal = pClient->openStreams.Set(strServerName, dummy);

				if (nRetVal == XN_STATUS_OK)
				{
					pServerStream->nOpenRefCount++;

					if (pServerStream->nOpenRefCount == 1) // first one to open
					{
						// open it
						nRetVal = m_sensor.OpenStream(strServerName);
					}

					if (nRetVal != XN_STATUS_OK)
					{
						xnLogError(XN_MASK_SENSOR_SERVER, "Failed to open stream: %s", xnGetStatusString(nRetVal));
						XN_ASSERT(pServerStream->nOpenRefCount > 0);
						--pServerStream->nOpenRefCount;
						pClient->openStreams.Remove(strServerName);
					}
					else
					{
						xnLogInfo(XN_MASK_SENSOR_SERVER, "Stream %s is now open by %u clients.", strServerName, pServerStream->nOpenRefCount);
					}
				}
			}
			else if (!bRequestOpen && bIsOpenForClient)
			{
				pClient->openStreams.Remove(strServerName);
				xnLogInfo(XN_MASK_SENSOR_SERVER, "Stream %s is now open by %u clients.", strServerName, pServerStream->nOpenRefCount);
				--pServerStream->nOpenRefCount;

				if (pServerStream->nOpenRefCount == 0)
				{
					m_sensor.CloseStream(strServerName);
				}
			}
			else
			{
				xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client requested to %s stream %s, but this was already the state.", bRequestOpen ? "open" : "close", strServerName);
			}
		}
	}

	nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nRetVal);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}


XnStatus XnSensorServer::HandleSingleRequest(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnPackedDataType nType;
	nRetVal = pClient->pPrivateIncomingPacker->ReadNextObject(&nType);
	XN_IS_STATUS_OK(nRetVal);

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];

	switch (nType)
	{
	case XN_SENSOR_SERVER_MESSAGE_OPEN_SENSOR:
		{
			nRetVal = HandleOpenSensor(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_INT_PROPERTY:
		{
			nRetVal = HandleSetIntProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_REAL_PROPERTY:
		{
			nRetVal = HandleSetRealProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_STRING_PROPERTY:
		{
			nRetVal = HandleSetStringProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_GENERAL_PROPERTY:
		{
			nRetVal = HandleSetGeneralProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY:
		{
			nRetVal = HandleGetIntProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY:
		{
			nRetVal = HandleGetRealProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY:
		{
			nRetVal = HandleGetStringProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY:
		{
			nRetVal = HandleGetGeneralProperty(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_INI_FILE:
		{
			nRetVal = HandleConfigFromINIFile(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_PROPERTY_SET:
		{
			nRetVal = HandleBatchConfig(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_NEW_STREAM:
		{
			nRetVal = HandleNewStream(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_STREAM_REMOVED:
		{
			nRetVal = HandleRemoveStream(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_READ_STREAM:
		{
			nRetVal = HandleReadStream(pClient);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_OPEN_STREAM:
		{
			nRetVal = HandleSetStreamState(pClient, TRUE);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_CLOSE_STREAM:
		{
			nRetVal = HandleSetStreamState(pClient, FALSE);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}

	case XN_SENSOR_SERVER_MESSAGE_BYE:
		{
			xnLogVerbose(XN_MASK_SENSOR_SERVER, "Received BYE from client %u", pClient->nID);
			nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_BYE, nRetVal, 0, NULL);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to send BYE reply to client %u: %s", pClient->nID, xnGetStatusString(nRetVal));
			}
			pClient->bShouldRun = FALSE; //Signal the client to stop 
			
			break;
		}
	
	default:
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Unknown client request: %d", nType);
		nRetVal = SendReply(pClient, XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, XN_STATUS_ERROR);
		XN_IS_STATUS_OK(nRetVal);

		return XN_STATUS_ERROR;
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorServer::ServeClient(XnClient* pClient)
{
	XnStatus nRetVal = XN_STATUS_OK;

	while (IsServerRunning() && pClient->bShouldRun)
	{
		if (!pClient->pStream->IsConnected())
		{
			xnLogWarning(XN_MASK_SENSOR_SERVER, "Client %u socket was closed.", pClient->nID);
			break;
		}

		nRetVal = HandleSingleRequest(pClient);
		if ((nRetVal != XN_STATUS_OK) &&
			(nRetVal != XN_STATUS_OS_NETWORK_CONNECTION_CLOSED) && 
			(nRetVal != XN_STATUS_OS_NETWORK_TIMEOUT))
		{
			xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed processing client request: %s", xnGetStatusString(nRetVal));
		}
	}

	// We cannot RemoveClient from this thread (as part of it is to close the thread). Instead, signal that 
	// client has stopped, and let server main thread wait for exit.
	pClient->bShouldRun = FALSE;

	return (XN_STATUS_OK);
}

XN_THREAD_PROC XnSensorServer::ClientThread(XN_THREAD_PARAM pThreadParam)
{
	XnClient* pClient = (XnClient*)pThreadParam;
	XnStatus nRetVal = pClient->pServer->ServeClient(pClient);
	XN_THREAD_PROC_RETURN(nRetVal);
}

XN_THREAD_PROC XnSensorServer::ReaderThread(XN_THREAD_PARAM pThreadParam)
{
	XnSensorServer* pThis = (XnSensorServer*)pThreadParam;
	XnStatus nRetVal = pThis->ReadStreams();
	XN_THREAD_PROC_RETURN(nRetVal);
}

void XnSensorServer::StreamCollectionChangedCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnStreamsChangeEventType EventType, void* pCookie)
{
	XnSensorServer* pThis = (XnSensorServer*)pCookie;
	pThis->OnStreamCollectionChanged(StreamName, EventType);
}

XnStatus XN_CALLBACK_TYPE XnSensorServer::PropertyChangedCallback(const XnProperty* pProp, void* pCookie)
{
	XnSensorServer* pThis = (XnSensorServer*)pCookie;
	return pThis->OnPropertyChanged(pProp);
}

void XN_CALLBACK_TYPE XnSensorServer::NewServerEventCallback(const XnUChar* pData, XnUInt32 nDataSize, void* pCookie)
{
	XnClient* pClient = (XnClient*)pCookie;
	pClient->pServer->OnNewServerEvent(pData, nDataSize, pClient);
}

void XN_CALLBACK_TYPE XnSensorServer::NewStreamDataCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, void* pCookie)
{
	XnSensorServer* pThis = (XnSensorServer*)pCookie;
	pThis->OnNewStreamData(StreamName);
}

XnStatus XN_CALLBACK_TYPE XnSensorServer::StartNewLogCallback(XnIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Closing current log file...");
	return xnLogStartNewFile();
}

XnStatus XnSensorServer::OnIntPropertyChangedInternally(const XnChar* strPropName, XnUInt64 nValue, XnBool& bPassPropToClients)
{
	XnStatus nRetVal = XN_STATUS_OK;
	bPassPropToClients = TRUE;
	
	if (strcmp(strPropName, XN_STREAM_PROPERTY_STATE) == 0)
	{
		// ignore STATE property (every client has its own value)
		bPassPropToClients = FALSE;
	}
	else if (strcmp(strPropName, XN_MODULE_PROPERTY_ERROR_STATE) == 0)
	{
		XnStatus nOldErrorState = m_nErrorState;
		m_nErrorState = (XnStatus)nValue;
		switch (m_nErrorState)
		{
			case XN_STATUS_DEVICE_NOT_CONNECTED:
				//TODO: Handle disconnection
				break;
			case XN_STATUS_OK:
				if (nOldErrorState == XN_STATUS_DEVICE_NOT_CONNECTED)
				{
					//TODO: Handle re-connection
					break;
				}
		}
	}

	return XN_STATUS_OK;
}

XnStatus XnSensorServer::OnRealPropertyChangedInternally(const XnChar* strPropName, XnDouble dValue, XnBool& bPassPropToClients)
{
	bPassPropToClients = TRUE;
	return XN_STATUS_OK;
}

XnStatus XnSensorServer::OnStringPropertyChangedInternally(const XnChar* strPropName, const XnChar* strValue, XnBool& bPassPropToClients)
{
	bPassPropToClients = TRUE;
	return XN_STATUS_OK;
}

XnStatus XnSensorServer::OnGeneralPropertyChangedInternally(const XnChar* strPropName, const XnGeneralBuffer &gbValue, XnBool& bPassPropToClients)
{
	bPassPropToClients = TRUE;
	return XN_STATUS_OK;
}

XN_DEVICE_API XnStatus XnSensorServerGetGlobalConfigFile(const XnChar* strConfigDir, XnChar* strConfigFile, XnUInt32 nBufSize)
{
	return XnSensor::ResolveGlobalConfigFileName(strConfigFile, nBufSize, strConfigDir);
}

XN_DEVICE_API XnStatus XnSensorServerRun(const XnChar* strConfigFile)
{
	XnSensorServer server;
	return server.Run(strConfigFile);
}

