/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorFixedParams.h"
#include "XnHostProtocol.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSensorFixedParams::XnSensorFixedParams(XnSensorFirmware* pFirmware, XnDevicePrivateData* pDevicePrivateData) :
	m_pFirmware(pFirmware),
	m_pDevicePrivateData(pDevicePrivateData)
{
}

XnStatus XnSensorFixedParams::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get fixed params
	XnFixedParams FixedParams;
	nRetVal = XnHostProtocolGetFixedParams(m_pDevicePrivateData, FixedParams);
	if (nRetVal != XN_STATUS_OK)
	{
	}

	if (m_pDevicePrivateData->FWInfo.nFWVer < XN_SENSOR_FW_VER_5_4)
	{
		sprintf(m_strSensorSerial, "%d", FixedParams.nSerialNumber);
	}
	else
	{	
		nRetVal = XnHostProtocolGetSerialNumber(m_pDevicePrivateData, m_strSensorSerial);

		if (nRetVal != XN_STATUS_OK)
		{
			return nRetVal;
		}
	}

	xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Sensor serial number: %s", m_strSensorSerial);

	// fill in properties
	m_nZeroPlaneDistance = (XnDepthPixel)FixedParams.fReferenceDistance;
	m_dZeroPlanePixelSize = FixedParams.fReferencePixelSize;
	m_dEmitterDCmosDistance = FixedParams.fDCmosEmitterDistance;
	m_dDCmosRCmosDistance = FixedParams.fDCmosRCmosDistance;
	


	return (XN_STATUS_OK);
}