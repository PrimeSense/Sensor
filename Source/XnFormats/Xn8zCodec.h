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





#ifndef __XN_8Z_CODEC_H__
#define __XN_8Z_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodecBase.h"
#include <XnFormats/XnStreamCompression.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_FORMATS_CPP_API Xn8zCodec : public XnCodecBase
{
public:
	virtual XnCompressionFormats GetCompressionFormat() const { return XN_COMPRESSION_COLOR_8Z; }
	virtual XnFloat GetWorseCompressionRatio() const { return XN_STREAM_COMPRESSION_IMAGE8Z_WORSE_RATIO; }
	virtual XnUInt32 GetOverheadSize() const { return 0; }

protected:
	virtual XnStatus CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize)
	{
		return XnStreamCompressImage8Z(pData, nDataSize, pCompressedData, pnCompressedDataSize);
	}

	virtual XnStatus DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize)
	{
		return XnStreamUncompressImage8Z(pCompressedData, nCompressedDataSize, pData, pnDataSize);
	}
};

#endif //__XN_8Z_CODEC_H__