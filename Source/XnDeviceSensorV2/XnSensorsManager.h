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
#ifndef __XN_SENSORS_MANAGER_H__
#define __XN_SENSORS_MANAGER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnServerSensorInvoker.h"
#include <XnStringsHash.h>

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
	inline XnBool HasOpenSensors() { return m_sensors.begin() != m_sensors.end(); }

private:
	typedef struct
	{
		XnUInt64 nNoClientsTime;
		XnServerSensorInvoker* pInvoker;
		XnUInt32 nRefCount;
	} ReferencedSensor;

	XN_DECLARE_STRINGS_HASH(ReferencedSensor, XnSensorsHash);

	static XnStatus XN_CALLBACK_TYPE StartNewLogCallback(XnIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	XnChar m_strGlobalConfigFile[XN_FILE_MAX_PATH];
	XN_CRITICAL_SECTION_HANDLE m_hLock;
	XnSensorsHash m_sensors;
	XnActualIntProperty m_noClientTimeout;
	XnIntProperty m_startNewLog;
	XnUInt64 nNoClientsTime;
};

#endif // __XN_SENSORS_MANAGER_H__