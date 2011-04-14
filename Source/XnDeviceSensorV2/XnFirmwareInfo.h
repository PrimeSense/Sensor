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





#ifndef __XN_FIRMWARE_INFO_H__
#define __XN_FIRMWARE_INFO_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStreamParams.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnFirmwareInfo
{
public:
	XnFWVer nFWVer;
	XnUInt16 nHostMagic;
	XnUInt16 nFWMagic;
	XnUInt16 nProtocolHeaderSize;
	XnUInt16 nProtocolMaxPacketSize;

	XnParamCurrentMode nCurrMode;

	XnBool bAudioSupported;

	XnUInt16 nOpcodeGetVersion;
	XnUInt16 nOpcodeKeepAlive;
	XnUInt16 nOpcodeGetParam;
	XnUInt16 nOpcodeSetParam;
	XnUInt16 nOpcodeGetFixedParams;
	XnUInt16 nOpcodeGetMode;
	XnUInt16 nOpcodeSetMode;
	XnUInt16 nOpcodeAlgorithmParams;
	XnUInt16 nOpcodeReset;
	XnUInt16 nOpcodeSetCmosBlanking;
	XnUInt16 nOpcodeGetCmosBlanking;
	XnUInt16 nOpcodeGetCmosPresets;


	XnBool bMirrorSupported;

	XnUInt16 nUSBDelayReceive;
	XnUInt16 nUSBDelayExecutePreSend;
	XnUInt16 nUSBDelayExecutePostSend;
	XnUInt16 nUSBDelaySoftReset;
	XnUInt16 nUSBDelaySetParamFlicker;
	XnUInt16 nUSBDelaySetParamStream0Mode;
	XnUInt16 nUSBDelaySetParamStream1Mode;
	XnUInt16 nUSBDelaySetParamStream2Mode;
};

#endif //__XN_FIRMWARE_INFO_H__