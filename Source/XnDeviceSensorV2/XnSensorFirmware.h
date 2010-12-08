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





#ifndef __XN_SENSOR_FIRMWARE_H__
#define __XN_SENSOR_FIRMWARE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFirmwareInfo.h"
#include "XnFirmwareCommands.h"
#include "XnSensorFirmwareParams.h"
#include "XnFirmwareStreams.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnSensorFirmware
{
public:
	XnSensorFirmware(XnDevicePrivateData* pDevicePrivateData);
	XnStatus Init(XnBool bReset);
	void Free();

	inline XnFirmwareInfo* GetInfo() { return m_pInfo; }
	inline XnFirmwareCommands* GetCommands() { return &m_Commands; }
	inline XnSensorFirmwareParams* GetParams() { return &m_Params; }
	inline XnFirmwareStreams* GetStreams() { return &m_Streams; }

private:
	XnFirmwareInfo* m_pInfo;
	XnFirmwareCommands m_Commands;
	XnSensorFirmwareParams m_Params;
	XnFirmwareStreams m_Streams;
	XnDevicePrivateData* m_pDevicePrivateData;
};

#endif //__XN_SENSOR_FIRMWARE_H__