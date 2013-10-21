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
	m_logFile(XN_MODULE_PROPERTY_SERVER_LOG_FILE, NULL),
	m_hLock(NULL)
{
	m_noClientTimeout.UpdateSetCallbackToDefault();
	m_startNewLog.UpdateSetCallback(StartNewLogCallback, this);
	m_logFile.UpdateGetCallback(GetLogCallback, this);
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
	while (m_sensors.Begin() != m_sensors.End())
	{
		ReferencedSensor& sensor = m_sensors.Begin()->Value();
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

		XnProperty* aAdditionalProps[] = { &m_noClientTimeout, &m_startNewLog, &m_logFile };
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
	XnSensorsHash::Iterator it = m_sensors.Begin();
	while (it != m_sensors.End())
	{
		XnSensorsHash::Iterator curr = it;
		++it;

		ReferencedSensor& sensor = curr->Value();
		if (sensor.nRefCount == 0)
		{
			// if timeout have passed, or the device was disconnected, remote this sensor
			XnUInt64 nErrorState = XN_STATUS_OK;
			sensor.pInvoker->GetIntProperty(XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_ERROR_STATE, &nErrorState);

			if (nErrorState == XN_STATUS_DEVICE_NOT_CONNECTED || (nNow - sensor.nNoClientsTime) > m_noClientTimeout.GetValue())
			{
				xnLogInfo(XN_MASK_SENSOR_SERVER, "No session holding sensor '%s'. Shutting down the sensor...", curr->Key(), m_noClientTimeout.GetValue());
				XN_DELETE(sensor.pInvoker);
				m_sensors.Remove(curr);
			}
		}
	}
}

XnStatus XN_CALLBACK_TYPE XnSensorsManager::StartNewLogCallback(XnIntProperty* /*pSender*/, XnUInt64 /*nValue*/, void* /*pCookie*/)
{
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Closing current log file...");
	return xnLogStartNewFile();
}

XnStatus XN_CALLBACK_TYPE XnSensorsManager::GetLogCallback(const XnStringProperty* /*pSender*/, XnChar* csValue, void* /*pCookie*/)
{
	return xnLogGetFileName(csValue, XN_DEVICE_MAX_STRING_LENGTH);
}

