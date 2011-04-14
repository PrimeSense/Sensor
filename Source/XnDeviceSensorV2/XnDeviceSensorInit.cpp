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
#include "XnDeviceSensor.h"
#include "XnDeviceSensorInit.h"
#include "XnDeviceSensorProtocol.h"
#include "Bayer.h"
#include "Registration.h"
#include "XnHostProtocol.h"
#include <XnLog.h>
#include "XnSensor.h"

#define XN_HOST_PROTOCOL_MUTEX_NAME		"HostProtocolMutex"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnDeviceSensorInit(XnDevicePrivateData* pDevicePrivateData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnDeviceSensorAllocateBuffers(pDevicePrivateData);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateNamedMutex(&pDevicePrivateData->hExecuteMutex, XN_HOST_PROTOCOL_MUTEX_NAME);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnDeviceSensorConfigureVersion(pDevicePrivateData);
	XN_IS_STATUS_OK(nRetVal);

	BayerUpdateGamma(1.0);

	nRetVal = xnOSCreateCriticalSection(&pDevicePrivateData->hAudioBufferCriticalSection);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceSensorOpenInputThreads(XnDevicePrivateData* pDevicePrivateData, XnBool bOpen1, XnBool bOpen2, XnBool bOpen3)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (bOpen2)
	{
		// Depth
		pDevicePrivateData->pSpecificDepthUsb = (XnSpecificUsbDevice*)xnOSMallocAligned(sizeof(XnSpecificUsbDevice), XN_DEFAULT_MEM_ALIGN);
		pDevicePrivateData->pSpecificDepthUsb->pDevicePrivateData = pDevicePrivateData;
		pDevicePrivateData->pSpecificDepthUsb->pUsbConnection = &pDevicePrivateData->SensorHandle.DepthConnection;
		pDevicePrivateData->pSpecificDepthUsb->CurrState.State = XN_WAITING_FOR_CONFIGURATION;

		if (pDevicePrivateData->pSpecificDepthUsb->pUsbConnection->bIsISO == TRUE)
		{
			if (pDevicePrivateData->pSensor->IsLowBandwidth())
			{
				pDevicePrivateData->pSpecificDepthUsb->nChunkReadBytes = XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO * pDevicePrivateData->SensorHandle.DepthConnection.nMaxPacketSize;
			}
			else
			{
				pDevicePrivateData->pSpecificDepthUsb->nChunkReadBytes = XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_ISO * pDevicePrivateData->SensorHandle.DepthConnection.nMaxPacketSize;
			}

			pDevicePrivateData->pSpecificDepthUsb->nTimeout = XN_SENSOR_READ_THREAD_TIMEOUT_ISO;
		}
		else
		{
			pDevicePrivateData->pSpecificDepthUsb->nChunkReadBytes = XN_SENSOR_USB_DEPTH_BUFFER_SIZE_MULTIPLIER_BULK * pDevicePrivateData->SensorHandle.DepthConnection.nMaxPacketSize;

			pDevicePrivateData->pSpecificDepthUsb->nTimeout = XN_SENSOR_READ_THREAD_TIMEOUT_BULK;
		}

		pDevicePrivateData->pSpecificDepthUsb->nIgnoreBytes = (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_5_0) ? 0 : pDevicePrivateData->pSpecificDepthUsb->nChunkReadBytes;
	}

	if (bOpen1)
	{
		// Image
		pDevicePrivateData->pSpecificImageUsb = (XnSpecificUsbDevice*)xnOSMallocAligned(sizeof(XnSpecificUsbDevice), XN_DEFAULT_MEM_ALIGN);
		pDevicePrivateData->pSpecificImageUsb->pDevicePrivateData = pDevicePrivateData;
		pDevicePrivateData->pSpecificImageUsb->pUsbConnection = &pDevicePrivateData->SensorHandle.ImageConnection;
		pDevicePrivateData->pSpecificImageUsb->CurrState.State = XN_WAITING_FOR_CONFIGURATION;

		if (pDevicePrivateData->pSpecificImageUsb->pUsbConnection->bIsISO == TRUE)
		{
			if (pDevicePrivateData->pSensor->IsLowBandwidth())
			{
				pDevicePrivateData->pSpecificImageUsb->nChunkReadBytes = XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO * pDevicePrivateData->SensorHandle.ImageConnection.nMaxPacketSize;
			}
			else
			{
				pDevicePrivateData->pSpecificImageUsb->nChunkReadBytes = XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_ISO * pDevicePrivateData->SensorHandle.ImageConnection.nMaxPacketSize;
			}

			pDevicePrivateData->pSpecificImageUsb->nTimeout = XN_SENSOR_READ_THREAD_TIMEOUT_ISO;
		}
		else
		{
			pDevicePrivateData->pSpecificImageUsb->nChunkReadBytes = XN_SENSOR_USB_IMAGE_BUFFER_SIZE_MULTIPLIER_BULK * pDevicePrivateData->SensorHandle.ImageConnection.nMaxPacketSize;

			pDevicePrivateData->pSpecificImageUsb->nTimeout = XN_SENSOR_READ_THREAD_TIMEOUT_BULK;
		}

		pDevicePrivateData->pSpecificImageUsb->nIgnoreBytes = (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_5_0) ? 0 : pDevicePrivateData->pSpecificImageUsb->nChunkReadBytes;
	}

	// Misc
	if (bOpen3 && pDevicePrivateData->pSensor->IsMiscSupported())
	{
		pDevicePrivateData->pSpecificMiscUsb = (XnSpecificUsbDevice*)xnOSMallocAligned(sizeof(XnSpecificUsbDevice), XN_DEFAULT_MEM_ALIGN);
		pDevicePrivateData->pSpecificMiscUsb->pDevicePrivateData = pDevicePrivateData;
		pDevicePrivateData->pSpecificMiscUsb->pUsbConnection = &pDevicePrivateData->SensorHandle.MiscConnection;
		pDevicePrivateData->pSpecificMiscUsb->CurrState.State = XN_WAITING_FOR_CONFIGURATION;

		if (pDevicePrivateData->pSpecificMiscUsb->pUsbConnection->bIsISO == TRUE)
		{
			if (pDevicePrivateData->pSensor->IsLowBandwidth())
			{
				pDevicePrivateData->pSpecificMiscUsb->nChunkReadBytes = XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_LOWBAND_ISO * pDevicePrivateData->SensorHandle.MiscConnection.nMaxPacketSize;
			}
			else
			{
				pDevicePrivateData->pSpecificMiscUsb->nChunkReadBytes = XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_ISO * pDevicePrivateData->SensorHandle.MiscConnection.nMaxPacketSize;
			}

			pDevicePrivateData->pSpecificMiscUsb->nTimeout = XN_SENSOR_READ_THREAD_TIMEOUT_ISO;
		}
		else
		{
			pDevicePrivateData->pSpecificMiscUsb->nChunkReadBytes = XN_SENSOR_USB_MISC_BUFFER_SIZE_MULTIPLIER_BULK * pDevicePrivateData->SensorHandle.MiscConnection.nMaxPacketSize;

			pDevicePrivateData->pSpecificMiscUsb->nTimeout = XN_SENSOR_READ_THREAD_TIMEOUT_BULK;
		}

		pDevicePrivateData->pSpecificMiscUsb->nIgnoreBytes = (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_5_0) ? 0 : pDevicePrivateData->pSpecificMiscUsb->nChunkReadBytes;
	}

	// Switch depth & image EPs for older FWs
	if (pDevicePrivateData->FWInfo.nFWVer <= XN_SENSOR_FW_VER_5_1)
	{
		XnSpecificUsbDevice* pTempUsbDevice = pDevicePrivateData->pSpecificDepthUsb;
		pDevicePrivateData->pSpecificDepthUsb = pDevicePrivateData->pSpecificImageUsb;
		pDevicePrivateData->pSpecificImageUsb = pTempUsbDevice;
	}

	return XN_STATUS_OK;
}

XnStatus XnDeviceSensorAllocateBuffers(XnDevicePrivateData* pDevicePrivateData)
{
	pDevicePrivateData->SensorHandle.DepthConnection.pUSBBuffer = (XnUInt8*)xnOSCallocAligned(XN_SENSOR_PROTOCOL_USB_BUFFER_SIZE, sizeof(XnUInt8), XN_DEFAULT_MEM_ALIGN);
	pDevicePrivateData->SensorHandle.DepthConnection.nUSBBufferReadOffset = 0;
	pDevicePrivateData->SensorHandle.DepthConnection.nUSBBufferWriteOffset = 0;

	pDevicePrivateData->SensorHandle.ImageConnection.pUSBBuffer = (XnUInt8*)xnOSCallocAligned(XN_SENSOR_PROTOCOL_USB_BUFFER_SIZE, sizeof(XnUInt8), XN_DEFAULT_MEM_ALIGN);
	pDevicePrivateData->SensorHandle.ImageConnection.nUSBBufferReadOffset = 0;
	pDevicePrivateData->SensorHandle.ImageConnection.nUSBBufferWriteOffset = 0;

	if (pDevicePrivateData->pSensor->IsMiscSupported())
	{
		pDevicePrivateData->SensorHandle.MiscConnection.pUSBBuffer = (XnUInt8*)xnOSCallocAligned(XN_SENSOR_PROTOCOL_USB_BUFFER_SIZE, sizeof(XnUInt8), XN_DEFAULT_MEM_ALIGN);
		pDevicePrivateData->SensorHandle.MiscConnection.nUSBBufferReadOffset = 0;
		pDevicePrivateData->SensorHandle.MiscConnection.nUSBBufferWriteOffset = 0;
	}
	else
	{
		pDevicePrivateData->SensorHandle.MiscConnection.pUSBBuffer = NULL;
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceSensorFreeBuffers(XnDevicePrivateData* pDevicePrivateData)
{
	if (pDevicePrivateData->pTempDepth1 != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->pTempDepth1);
	}

	if (pDevicePrivateData->pTempImage1 != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->pTempImage1);
	}

	if (pDevicePrivateData->SensorHandle.DepthConnection.pUSBBuffer != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->SensorHandle.DepthConnection.pUSBBuffer);
	}

	if (pDevicePrivateData->SensorHandle.ImageConnection.pUSBBuffer != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->SensorHandle.ImageConnection.pUSBBuffer);
	}

	if (pDevicePrivateData->SensorHandle.MiscConnection.pUSBBuffer != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->SensorHandle.MiscConnection.pUSBBuffer);
	}

	if (pDevicePrivateData->pSpecificDepthUsb != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->pSpecificDepthUsb);
	}

	if (pDevicePrivateData->pSpecificImageUsb != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->pSpecificImageUsb);
	}

	if (pDevicePrivateData->pSpecificMiscUsb != NULL)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevicePrivateData->pSpecificMiscUsb);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceSensorConfigureVersion(XnDevicePrivateData* pDevicePrivateData)
{
	XnVersions Versions;
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnHostProtocolGetVersion(pDevicePrivateData, pDevicePrivateData->Version);
	
	// Strange bug: sometimes, when sending first command to device, no reply is received, so try again
	if (nRetVal == XN_STATUS_USB_TRANSFER_TIMEOUT)
	{
		xnOSSleep(2000);
		nRetVal = XnHostProtocolGetVersion(pDevicePrivateData, pDevicePrivateData->Version);
	}
	
	if (nRetVal != XN_STATUS_OK)
	{
		return nRetVal;
	}

	// Sensor HW Version is always 1.0 now...
	pDevicePrivateData->SensorInfo.nSensorVer = XN_SENSOR_VER_2_0;

	return (XN_STATUS_OK);
}
