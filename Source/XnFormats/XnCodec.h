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





#ifndef __XN_CODEC_H__
#define __XN_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnOS.h>
#include <XnStreamFormats.h>
#include <XnCodecIDs.h>
#include "XnFormats.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_FORMATS_CPP_API XnCodec
{
public:
	static XnCompressionFormats GetCompressionFormatFromCodecID(XnCodecID codecID);
	static XnCodecID GetCodecIDFromCompressionFormat(XnCompressionFormats format);

	XnCodec() {}
	virtual ~XnCodec() {}

	virtual XnStatus Init() { return XN_STATUS_OK; }

	virtual XnCompressionFormats GetCompressionFormat() const = 0;

	virtual XnStatus Compress(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) = 0;

	virtual XnStatus Decompress(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) = 0;
};

#endif //__XN_CODEC_H__
