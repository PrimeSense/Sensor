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





#ifndef __XN_CODEC_FACTORY_H__
#define __XN_CODEC_FACTORY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnFormats/XnCodec.h>
#include <XnStreamParams.h>
#include <XnDevice.h>
#include <XnDDK/XnDeviceModule.h>
#include <XnDDK/XnDeviceFunctionsTypedefs.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnCodecFactory
{
public:
	static XnStatus Create(XnCompressionFormats nFormat, XnDeviceModule* pStream, const XnChar* StreamName, XnCodec** ppCodec);
	static void Destroy(XnCodec* pCodec);
};

#endif //__XN_CODEC_FACTORY_H__