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





#ifndef __XN_FIRMWARE_COMMANDS_H__
#define __XN_FIRMWARE_COMMANDS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* Implements the various firmware opcodes.
* TODO: all opcodes should move here from XnHostProtocol.cpp.
*/
class XnFirmwareCommands
{
public:
	XnFirmwareCommands(XnDevicePrivateData* pDevicePrivateData);

	XnStatus GetFirmwareParam(XnUInt16 nParam, XnUInt16* pnValue);
	XnStatus SetFirmwareParam(XnUInt16 nParam, XnUInt16 nValue);
	XnStatus SetMultipleFirmwareParams(XnInnerParamData* aParams, XnUInt32 nCount);

private:
	XnDevicePrivateData* m_pDevicePrivateData;
};

#endif //__XN_FIRMWARE_COMMANDS_H__