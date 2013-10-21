/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.x Alpha                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Sensor                                    *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
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
#include <XnListT.h>

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
	typedef XnListT<XnServerSession*> XnSessionsList;

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
	const XnChar* m_strConfigFile;
};

#endif //__XN_SENSOR_SERVER_H__
