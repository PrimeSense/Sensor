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





#ifndef __XN_UNCOMPRESSED_CODEC_H__
#define __XN_UNCOMPRESSED_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodecBase.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_FORMATS_CPP_API XnUncompressedCodec : public XnCodecBase
{
public:
	XnUncompressedCodec() {}
	virtual ~XnUncompressedCodec() {}

	virtual XnCompressionFormats GetCompressionFormat() const { return XN_COMPRESSION_NONE; }
	virtual XnFloat GetWorseCompressionRatio() const { return 1.0; }
	virtual XnUInt32 GetOverheadSize() const { return 0; }

protected:
	virtual XnStatus CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize)
	{
		if (nDataSize > *pnCompressedDataSize)
		{
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		}

		xnOSMemCopy(pCompressedData, pData, nDataSize);
		*pnCompressedDataSize = nDataSize;
		return (XN_STATUS_OK);
	}

	virtual XnStatus DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize)
	{
		if (nCompressedDataSize > *pnDataSize)
		{
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		}

		xnOSMemCopy(pData, pCompressedData, nCompressedDataSize);
		*pnDataSize = nCompressedDataSize;
		return (XN_STATUS_OK);
	}
};

#endif //__XN_UNCOMPRESSED_CODEC_H__