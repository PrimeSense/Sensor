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
#include "XnExportedSensorDevice.h"
#include <XnPsVersion.h>
#include "XnSensorDevice.h"
#include <XnOpenNI.h>
#include <XnCommon.h>
#include "XnSensorServer.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
// On weak platforms (like Arm), the default is not to use multi-process. 
#if (XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM || XN_PLATFORM == XN_PLATFORM_ANDROID_X86)
	#define XN_SENSOR_DEFAULT_MULTI_PROCESS	(FALSE)
#else
	#define XN_SENSOR_DEFAULT_MULTI_PROCESS	(TRUE)
#endif


//---------------------------------------------------------------------------
// XnExportedSensorDevice class
//---------------------------------------------------------------------------
XnExportedSensorDevice::XnExportedSensorDevice()
{}

void XnExportedSensorDevice::FillCommonDescriptionFields(XnProductionNodeDescription* pDescription)
{
	strcpy(pDescription->strName, XN_DEVICE_NAME);
	strcpy(pDescription->strVendor, XN_VENDOR_PRIMESENSE);

	pDescription->Version.nMajor = XN_PS_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_PS_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_PS_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_PS_BUILD_VERSION;
}

void XnExportedSensorDevice::GetDescription(XnProductionNodeDescription* pDescription)
{
	FillCommonDescriptionFields(pDescription);
	pDescription->Type = XN_NODE_TYPE_DEVICE;
}

XnStatus XnExportedSensorDevice::EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* /*pErrors*/)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// enumerate connected sensors
	XnUInt32 nCount = 0;

	// check if sensor is connected
	nRetVal = XnSensor::Enumerate(NULL, &nCount);
	if (nRetVal != XN_STATUS_OUTPUT_BUFFER_OVERFLOW)
	{
		// no sensor connected
		return XN_STATUS_DEVICE_NOT_CONNECTED;
	}

	// allocate according to count
	XnConnectionString* pConnStrings;
	XN_VALIDATE_CALLOC(pConnStrings, XnConnectionString, nCount);

	nRetVal = XnSensor::Enumerate(pConnStrings, &nCount);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pConnStrings);
		return (nRetVal);
	}

	XnProductionNodeDescription Description;
	GetDescription(&Description);

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		// Each connection string is a sensor. Return it if it wasn't created already.
		if (FindCreatedDevice(context.GetUnderlyingObject(), pConnStrings[i]) == m_createdDevices.End())
		{
			nRetVal = TreesList.Add(Description, pConnStrings[i], NULL);
			if (nRetVal != XN_STATUS_OK)
			{
				xnOSFree(pConnStrings);
				return (nRetVal);
			}
		}
	}

	xnOSFree(pConnStrings);
	return (XN_STATUS_OK);
}

XnStatus XnExportedSensorDevice::Create(xn::Context& context, 
										const XnChar* strInstanceName, 
										const XnChar* strCreationInfo, 
										xn::NodeInfoList* /*pNeededTrees*/, 
										const XnChar* strConfigurationDir, 
										xn::ModuleProductionNode** ppInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strGlobalConfigFile[XN_FILE_MAX_PATH];
	nRetVal = XnSensor::ResolveGlobalConfigFileName(strGlobalConfigFile, XN_FILE_MAX_PATH, strConfigurationDir);
	XN_IS_STATUS_OK(nRetVal);

	// multi-process is not supported on Mac
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	XnBool bEnableMultiProcess = FALSE;
#else
	XnBool bEnableMultiProcess = XN_SENSOR_DEFAULT_MULTI_PROCESS;
	XnUInt32 nValue;
	if (XN_STATUS_OK == xnOSReadIntFromINI(strGlobalConfigFile, XN_SENSOR_SERVER_CONFIG_FILE_SECTION, XN_MODULE_PROPERTY_ENABLE_MULTI_PROCESS, &nValue))
	{
		bEnableMultiProcess = (nValue == TRUE);
	}
#endif

	XnDeviceBase* pSensor = NULL;

	if (bEnableMultiProcess)
	{
		XN_VALIDATE_NEW(pSensor, XnSensorClient);
	}
	else
	{
		XN_VALIDATE_NEW(pSensor, XnSensor);
	}

	XnDeviceConfig config;
	config.DeviceMode = XN_DEVICE_MODE_READ;
	config.cpConnectionString = strCreationInfo;
	config.SharingMode = XN_DEVICE_EXCLUSIVE;
	config.pInitialValues = NULL;

	if (strConfigurationDir != NULL)
	{
		if (bEnableMultiProcess)
		{
			XnSensorClient* pClient = (XnSensorClient*)pSensor;
			pClient->SetConfigDir(strConfigurationDir);
		}
		else
		{
			XnSensor* pActualSensor = (XnSensor*)pSensor;
			pActualSensor->SetGlobalConfigFile(strGlobalConfigFile);
		}
	}

	nRetVal = pSensor->Init(&config);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pSensor);
		return (nRetVal);
	}

	XnSensorDevice* pDevice = XN_NEW(XnSensorDevice, context, pSensor, strInstanceName);
	if (pDevice == NULL)
	{
		XN_DELETE(pSensor);
		return (XN_STATUS_ALLOC_FAILED);
	}

	nRetVal = pDevice->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pDevice);
		XN_DELETE(pSensor);
		return (nRetVal);
	}

	nRetVal = m_createdDevices.AddLast(DeviceKey(context.GetUnderlyingObject(), strCreationInfo));
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pDevice);
		XN_DELETE(pSensor);
		return (nRetVal);
	}

	*ppInstance = pDevice;

	return (XN_STATUS_OK);
}

void XnExportedSensorDevice::Destroy(xn::ModuleProductionNode* pInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnSensorDevice* pDevice = dynamic_cast<XnSensorDevice*>(pInstance);
	XnChar strConnStr[XN_MAX_CREATION_INFO_LENGTH];
	nRetVal = pDevice->GetStringProperty(XN_MODULE_PROPERTY_USB_PATH, strConnStr, sizeof(strConnStr));
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_DEVICE_SENSOR, "Couldn't get usb path property ?! :(");
		XN_ASSERT(FALSE);
	}
	XnContext* pContext = pDevice->GetContext().GetUnderlyingObject();
	CreatedDevices::Iterator it = FindCreatedDevice(pContext, strConnStr);
	if (it == m_createdDevices.End())
	{
		xnLogWarning(XN_MASK_DEVICE_SENSOR, "Couldn't find device in created devices ?! :(");
		XN_ASSERT(FALSE);
	}
	else
	{
		m_createdDevices.Remove(it);
	}

	XnDeviceBase* pSensor = pDevice->GetSensor();
	pSensor->Destroy();
	XN_DELETE(pSensor);
	XN_DELETE(pDevice);

}

XnExportedSensorDevice::DeviceKey::DeviceKey(XnContext* pContext, const XnChar* strConnStr)
{
	m_pContext = pContext;
	xnOSStrCopy(m_strConnStr, strConnStr, sizeof(m_strConnStr));
}

XnExportedSensorDevice::CreatedDevices::Iterator XnExportedSensorDevice::FindCreatedDevice(XnContext* pContext, 
																                           const XnChar* strConnStr)
{
	CreatedDevices::Iterator it = m_createdDevices.Begin();
	for (; it != m_createdDevices.End(); it++)
	{
		if ((it->m_pContext == pContext) && 
			 (xnOSStrCmp(it->m_strConnStr, strConnStr) == 0))
		{
			break;
		}
	}

	return it;
}
