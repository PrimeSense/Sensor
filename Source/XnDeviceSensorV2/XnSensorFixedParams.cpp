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

	xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Sensor serial number: %d", FixedParams.nSerialNumber);

	// fill in properties
	m_nZeroPlaneDistance = (XnDepthPixel)FixedParams.fReferenceDistance;
	m_dZeroPlanePixelSize = FixedParams.fReferencePixelSize;
	m_dEmitterDCmosDistance = FixedParams.fDCmosEmitterDistance;
	sprintf(m_strSensorSerial, "%d", FixedParams.nSerialNumber);


	return (XN_STATUS_OK);
}