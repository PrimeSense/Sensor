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





#ifndef __XN_SENSOR_FIXED_PARAMS_H__
#define __XN_SENSOR_FIXED_PARAMS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorFirmware.h"

//---------------------------------------------------------------------------
// XnSensorFixedParams class
//---------------------------------------------------------------------------
class XnSensorFixedParams
{
public:
	XnSensorFixedParams(XnSensorFirmware* pFirmware, XnDevicePrivateData* pDevicePrivateData);

	XnStatus Init();


	inline XnDepthPixel GetZeroPlaneDistance() const { return m_nZeroPlaneDistance; }
	inline XnDouble GetZeroPlanePixelSize() const { return m_dZeroPlanePixelSize; }
	inline XnDouble GetEmitterDCmosDistance() const { return m_dEmitterDCmosDistance; }

	inline const XnChar* GetSensorSerial() const { return m_strSensorSerial; }

private:
	XnSensorFirmware* m_pFirmware;
	XnDevicePrivateData* m_pDevicePrivateData;


	XnDepthPixel m_nZeroPlaneDistance;
	XnDouble m_dZeroPlanePixelSize;
	XnDouble m_dEmitterDCmosDistance;

	XnChar m_strSensorSerial[XN_DEVICE_MAX_STRING_LENGTH];
};

#endif //__XN_SENSOR_FIXED_PARAMS_H__