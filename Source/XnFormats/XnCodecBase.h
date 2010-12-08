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






#ifndef __XN_CODEC_BASE_H__
#define __XN_CODEC_BASE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodec.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_FORMATS_CPP_API XnCodecBase : public XnCodec
{
public:
	static XnCompressionFormats GetCompressionFormatFromCodecID(XnCodecID codecID);
	static XnCodecID GetCodecIDFromCompressionFormat(XnCompressionFormats format);

	XnCodecBase() {}
	virtual ~XnCodecBase() {}

	virtual XnStatus Init() { return XN_STATUS_OK; }

	virtual XnCompressionFormats GetCompressionFormat() const = 0;

	XnStatus Compress(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		XN_VALIDATE_INPUT_PTR(pData);
		XN_VALIDATE_INPUT_PTR(pCompressedData);
		XN_VALIDATE_OUTPUT_PTR(pnCompressedDataSize);

		if ((nDataSize * GetWorseCompressionRatio() + GetOverheadSize()) > *pnCompressedDataSize)
		{
			return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
		}

		nRetVal = CompressImpl(pData, nDataSize, pCompressedData, pnCompressedDataSize);
		XN_IS_STATUS_OK(nRetVal);

		return (XN_STATUS_OK);
	}

	XnStatus Decompress(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		XN_VALIDATE_INPUT_PTR(pCompressedData);
		XN_VALIDATE_INPUT_PTR(pData);
		XN_VALIDATE_OUTPUT_PTR(pnDataSize);

		nRetVal = DecompressImpl(pCompressedData, nCompressedDataSize, pData, pnDataSize);
		XN_IS_STATUS_OK(nRetVal);

		return (XN_STATUS_OK);
	}

	virtual XnUInt32 GetOverheadSize() const = 0;
	virtual XnFloat GetWorseCompressionRatio() const = 0;

protected:
	virtual XnStatus CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) = 0;
	virtual XnStatus DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) = 0;
};

#endif // __XN_CODEC_BASE_H__