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
#ifndef __XN_SENSORS_MANAGER_H__
#define __XN_SENSORS_MANAGER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnServerSensorInvoker.h"
#include <XnStringsHashT.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnSensorsManager
{
public:
	XnSensorsManager(const XnChar* strGlobalConfigFile);
	~XnSensorsManager();

	XnStatus Init();
	void Free();

	XnUInt64 GetTimeout() { return m_noClientTimeout.GetValue(); }

	XnStatus GetSensor(const XnChar* strDevicePath, XnServerSensorInvoker** ppInvoker);
	void ReleaseSensor(XnServerSensorInvoker* pInvoker);

	void CleanUp();
	inline XnBool HasOpenSensors() { return m_sensors.Begin() != m_sensors.End(); }

private:
	typedef struct
	{
		XnUInt64 nNoClientsTime;
		XnServerSensorInvoker* pInvoker;
		XnUInt32 nRefCount;
	} ReferencedSensor;

	typedef XnStringsHashT<ReferencedSensor> XnSensorsHash;

	static XnStatus XN_CALLBACK_TYPE StartNewLogCallback(XnIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetLogCallback(const XnStringProperty* pSender, XnChar* csValue, void* pCookie);

	XnChar m_strGlobalConfigFile[XN_FILE_MAX_PATH];
	XN_CRITICAL_SECTION_HANDLE m_hLock;
	XnSensorsHash m_sensors;
	XnActualIntProperty m_noClientTimeout;
	XnStringProperty m_logFile;
	XnIntProperty m_startNewLog;
	XnUInt64 nNoClientsTime;
};

#endif // __XN_SENSORS_MANAGER_H__