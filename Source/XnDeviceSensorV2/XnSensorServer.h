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
#ifndef __XN_SENSOR_SERVER_H__
#define __XN_SENSOR_SERVER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include "XnSensor.h"
#include <XnDDK/XnDataPacker.h>
#include "XnSensorClientServer.h"
#include "XnServerLogger.h"
#include "XnSensorsManager.h"
#include "XnServerSession.h"

//---------------------------------------------------------------------------
// XnSensorServer class
//---------------------------------------------------------------------------
class XnServerStreamsHash;

class XnSensorServer
{
public:
	XnSensorServer(const XnChar* strConfigFile);
	~XnSensorServer();

	XnStatus Run();
	XnBool IsServerRunning();

private:
	XN_DECLARE_LIST(XnServerSession*, XnSessionsList);

	XnStatus InitServer();
	XnStatus ServerMainLoop();
	void ShutdownServer();
	void CheckForNewClients(XnUInt32 nTimeout);
	void CleanUpSessions();
	XnBool ShutdownIfPossible();
	XnBool CanShutdown();
	void Free();
	XnStatus AddSession(XN_SOCKET_HANDLE hClientSocket);
	XnStatus RemoveSession(XnSessionsList::ConstIterator it);
	XnStatus ReturnToDefaults();

	XN_SOCKET_HANDLE m_hListenSocket;

	XN_EVENT_HANDLE m_hServerRunningEvent; //This event is set as long as the server is running and servicing requests
	XN_MUTEX_HANDLE m_hServerRunningMutex; //This mutex protects m_hServerRunningEvent
	XN_CRITICAL_SECTION_HANDLE m_hSessionsLock;

	XnSessionsList m_sessions;
	XnUInt32 m_nLastClientID;
	XnStatus m_nErrorState;

	XnSensorsManager m_sensorsManager;
	XnServerLogger m_logger;
	XnUInt64 m_nLastSessionActivity;
};

#endif //__XN_SENSOR_SERVER_H__
