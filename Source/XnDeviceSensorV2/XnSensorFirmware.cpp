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
#include "XnSensorFirmware.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSensorFirmware::XnSensorFirmware(XnDevicePrivateData* pDevicePrivateData) :
	m_pInfo(&pDevicePrivateData->FWInfo),
	m_Commands(pDevicePrivateData),
	m_Params(m_pInfo, &m_Commands),
	m_Streams(pDevicePrivateData),
	m_pDevicePrivateData(pDevicePrivateData)
{
}

XnStatus XnSensorFirmware::Init(XnBool bReset)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check current mode
	XnUInt16 nMode;
	nRetVal = XnHostProtocolGetMode(m_pDevicePrivateData, nMode);
	XN_IS_STATUS_OK(nRetVal);

	if (bReset)
	{
		// check if safe mode
		if (nMode == XN_HOST_PROTOCOL_MODE_SAFE_MODE)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_SAFE_MODE, XN_MASK_DEVICE_SENSOR, "Device is in safe mode. Cannot start any stream!");
		}

		// check if device is alive
		XnUInt32 nCounter = 5;
		while (nCounter)
		{
			nRetVal = XnHostProtocolKeepAlive(m_pDevicePrivateData);
			if (nRetVal != XN_STATUS_OK)
			{
				nCounter--;
			}
			else
				nCounter = 0;
		}
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Keep alive failed!\n");
			return nRetVal;
		}

		// perform a soft reset (to start clean)
		nRetVal = XnHostProtocolReset(m_pDevicePrivateData, XN_RESET_TYPE_SOFT_FIRST);
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Couldn't reset the device!\n");
			return nRetVal;
		}

		// wait for sensor to recover from reset
		xnOSSleep(m_pDevicePrivateData->FWInfo.nUSBDelaySoftReset);

		// send keep alive again to see sensor is up
		nCounter = 10;
		while (nCounter)
		{
			nRetVal = XnHostProtocolKeepAlive(m_pDevicePrivateData);
			if (nRetVal != XN_STATUS_OK)
			{
				nCounter--;
				xnOSSleep(10);
			}
			else
				break;
		}
		if (nCounter == 0)
		{
			printf("10 keep alives is too much - stopping\n");
			return nRetVal;
		}

		nRetVal = XnHostProtocolGetMode(m_pDevicePrivateData, nMode);
		XN_IS_STATUS_OK(nRetVal);

		// check if safe mode
		if (nMode == XN_HOST_PROTOCOL_MODE_SAFE_MODE)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_SAFE_MODE, XN_MASK_DEVICE_SENSOR, "Device is in safe mode. Cannot start any stream!");
		}
	}

	nRetVal = m_Params.Init();
	XN_IS_STATUS_OK(nRetVal);

	if (nMode == XN_HOST_PROTOCOL_MODE_PS)
	{
		nRetVal = m_Params.UpdateAllProperties();
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = m_Streams.Init();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnSensorFirmware::Free()
{
	m_Params.Free();
}