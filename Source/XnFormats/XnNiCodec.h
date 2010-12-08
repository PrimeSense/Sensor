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






#ifndef __XN_XN_CODEC_H__
#define __XN_XN_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodec.h"
#include <XnCppWrapper.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_FORMATS_CPP_API XnNiCodec : public XnCodec
{
public:
	XnNiCodec(xn::Codec& codec) : m_codec(codec) {}
	virtual ~XnNiCodec() {}

	virtual XnCompressionFormats GetCompressionFormat() const { return XnCodec::GetCompressionFormatFromCodecID(m_codec.GetCodecID()); }

	virtual XnStatus Compress(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize)
	{
		return m_codec.EncodeData(pData, nDataSize, pCompressedData, *pnCompressedDataSize, pnCompressedDataSize);
	}

	virtual XnStatus Decompress(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize)
	{
		return m_codec.DecodeData(pCompressedData, nCompressedDataSize, pData, *pnDataSize, pnDataSize);
	}

private:
	xn::Codec m_codec;
};


#endif // __XN_XN_CODEC_H__