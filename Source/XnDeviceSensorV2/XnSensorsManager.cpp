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
#include "XnSensorsManager.h"
#include "XnSensorClientServer.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_DEFAULT_SERVER_WAIT_FOR_CLIENT_TIME	10000

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSensorsManager::XnSensorsManager(const XnChar* strGlobalConfigFile) :
	m_noClientTimeout(XN_MODULE_PROPERTY_SERVER_NO_CLIENTS_TIMEOUT, XN_SENSOR_DEFAULT_SERVER_WAIT_FOR_CLIENT_TIME),
	m_startNewLog(XN_MODULE_PROPERTY_SERVER_START_NEW_LOG_FILE),
	m_hLock(NULL)
{
	m_noClientTimeout.UpdateSetCallbackToDefault();
	m_startNewLog.UpdateSetCallback(StartNewLogCallback, this);

	strcpy(m_strGlobalConfigFile, strGlobalConfigFile);
}

XnSensorsManager::~XnSensorsManager()
{
	Free();
}

XnStatus XnSensorsManager::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnOSCreateCriticalSection(&m_hLock);
	XN_IS_STATUS_OK(nRetVal);

	// read default timeout from file
	nRetVal = m_noClientTimeout.ReadValueFromFile(m_strGlobalConfigFile, XN_SENSOR_SERVER_CONFIG_FILE_SECTION);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnSensorsManager::Free()
{
	// close all sensors
	while (m_sensors.begin() != m_sensors.end())
	{
		ReferencedSensor& sensor = m_sensors.begin().Value();
		XN_DELETE(sensor.pInvoker);
	}

	if (m_hLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hLock);
		m_hLock = NULL;
	}
}

XnStatus XnSensorsManager::GetSensor(const XnChar* strDevicePath, XnServerSensorInvoker** ppInvoker)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check if the sensor is already open
	XnAutoCSLocker locker(m_hLock);
	ReferencedSensor* pSensor;
	nRetVal = m_sensors.Get(strDevicePath, pSensor);
	if (nRetVal == XN_STATUS_NO_MATCH)
	{
		// not open. open it now
		xnLogInfo(XN_MASK_SENSOR_SERVER, "Opening sensor '%s'...", strDevicePath);

		ReferencedSensor sensor;
		sensor.nRefCount = 0;
		XN_VALIDATE_NEW(sensor.pInvoker, XnServerSensorInvoker);

		XnProperty* aAdditionalProps[] = { &m_noClientTimeout, &m_startNewLog };
		nRetVal = sensor.pInvoker->Init(strDevicePath, m_strGlobalConfigFile, sizeof(aAdditionalProps)/sizeof(XnProperty*), aAdditionalProps);
		XN_IS_STATUS_OK(nRetVal);

		// add it to map
		nRetVal = m_sensors.Set(sensor.pInvoker->GetDevicePath(), sensor);
		XN_IS_STATUS_OK(nRetVal);

		// and take a reference to it
		nRetVal = m_sensors.Get(sensor.pInvoker->GetDevicePath(), pSensor);
		XN_IS_STATUS_OK(nRetVal);
	}

	++pSensor->nRefCount;
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Sensor '%s' now has %u sessions", pSensor->pInvoker->GetDevicePath(), pSensor->nRefCount);

	*ppInvoker = pSensor->pInvoker;
	
	return (XN_STATUS_OK);
}

void XnSensorsManager::ReleaseSensor(XnServerSensorInvoker* pInvoker)
{
	XnAutoCSLocker locker(m_hLock);
	ReferencedSensor* pSensor;
	XnStatus nRetVal = m_sensors.Get(pInvoker->GetDevicePath(), pSensor);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_SENSOR_SERVER, "Trying to release a sensor that is not in the map!");
		return;
	}

	--pSensor->nRefCount;
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Sensor '%s' now has %u sessions", pInvoker->GetDevicePath(), pSensor->nRefCount);

	if (pSensor->nRefCount == 0)
	{
		// store current time. Then, in CleanUp() if timeout passed, sensor will be closed
		xnOSGetTimeStamp(&pSensor->nNoClientsTime);

		// do some clean-up (so that next client will behave as if it started the server)

		// This is a bit ugly, but we need to manually set back to default DEVICE properties
		// (we know there aren't any streams and clients, but the Device module always remains)
		nRetVal = pSensor->pInvoker->SetIntProperty(XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_FRAME_SYNC, (XnUInt64)FALSE);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_SERVER, "Failed resetting FrameSync: %s", xnGetStatusString(nRetVal));
		}

		nRetVal = pSensor->pInvoker->ConfigureModuleFromGlobalFile(XN_MODULE_NAME_DEVICE);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_SERVER, "Failed configuring device from global config file: %s", xnGetStatusString(nRetVal));
		}
	}
}

void XnSensorsManager::CleanUp()
{
	// go over sensors list. each sensor that is not open by any session, and timeout has passed should
	// be closed and removed
	XnAutoCSLocker locker(m_hLock);

	XnUInt64 nNow;
	xnOSGetTimeStamp(&nNow);
	XnSensorsHash::Iterator it = m_sensors.begin();
	while (it != m_sensors.end())
	{
		XnSensorsHash::Iterator curr = it;
		++it;

		ReferencedSensor& sensor = curr.Value();
		if (sensor.nRefCount == 0 && (nNow - sensor.nNoClientsTime) > m_noClientTimeout.GetValue())
		{
			xnLogInfo(XN_MASK_SENSOR_SERVER, "No session holding sensor '%s' for %u ms. Shutting down...", curr.Key(), m_noClientTimeout.GetValue());
			XN_DELETE(sensor.pInvoker);
			m_sensors.Remove(curr);
		}
	}
}

XnStatus XN_CALLBACK_TYPE XnSensorsManager::StartNewLogCallback(XnIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Closing current log file...");
	return xnLogStartNewFile();
}

