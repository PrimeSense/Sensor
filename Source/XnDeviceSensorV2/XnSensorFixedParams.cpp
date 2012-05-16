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
XnSensorFixedParams::XnSensorFixedParams(XnDevicePrivateData* pDevicePrivateData) :
	m_pDevicePrivateData(pDevicePrivateData),
	m_nSensorDepthCMOSI2CBus(0),
	m_nSensorDepthCMOSI2CSlaveAddress(0),
	m_nSensorImageCMOSI2CBus(0),
	m_nSensorImageCMOSI2CSlaveAddress(0),
	m_nZeroPlaneDistance(0),
	m_dZeroPlanePixelSize(0),
	m_dEmitterDCmosDistance(0),
	m_dDCmosRCmosDistance(0),
	m_nImageCmosType(0)
{
	m_strSensorSerial[0] = '\0';
	m_deviceInfo.strDeviceName[0] = '\0';
	m_deviceInfo.strVendorData[0] = '\0';
	m_strPlatformString[0] = '\0';
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

	m_nSensorDepthCMOSI2CBus = (XnUInt16)FixedParams.nDepthCmosI2CBus;
	m_nSensorDepthCMOSI2CSlaveAddress = (XnUInt16)FixedParams.nDepthCmosI2CAddress;
	m_nSensorImageCMOSI2CBus = (XnUInt16)FixedParams.nImageCmosI2CBus;
	m_nSensorImageCMOSI2CSlaveAddress = (XnUInt16)FixedParams.nImageCmosI2CAddress;
	
	m_nImageCmosType = (XnUInt32)FixedParams.nImageCmosType;

	nRetVal = XnHostProtocolAlgorithmParams(m_pDevicePrivateData, XN_HOST_PROTOCOL_ALGORITHM_DEVICE_INFO, 
		&m_deviceInfo, sizeof(m_deviceInfo), (XnResolutions)0, 0);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnHostProtocolGetPlatformString(m_pDevicePrivateData, m_strPlatformString);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}