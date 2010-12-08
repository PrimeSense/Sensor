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





#ifndef __XN_JPEG_CODEC_H__
#define __XN_JPEG_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodecBase.h"
#include <XnFormats/XnStreamCompression.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_FORMATS_CPP_API XnJpegCodec : public XnCodecBase
{
public:
	XnJpegCodec(XnBool bRGB, XnUInt32 nXRes, XnUInt32 nYRes, XnUInt32 nQuality = XN_STREAM_COMPRESSION_JPEG_DEFAULT_QUALITY) :
		m_bRGB(bRGB), m_nXRes(nXRes), m_nYRes(nYRes), m_nQuality(nQuality)
	{}

	~XnJpegCodec()
	{
		XnStreamFreeCompressImageJ(&m_CompJPEGContext);
		XnStreamFreeUncompressImageJ(&m_UncompJPEGContext);
	}

	XnStatus Init()
	{
		XnStatus nRetVal = XN_STATUS_OK;

		nRetVal = XnStreamInitCompressImageJ(&m_CompJPEGContext);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnStreamInitUncompressImageJ(&m_UncompJPEGContext);
		if (nRetVal != XN_STATUS_OK)
		{
			XnStreamFreeCompressImageJ(&m_CompJPEGContext);
			return (nRetVal);
		}

		return (XN_STATUS_OK);
	}

	virtual XnCompressionFormats GetCompressionFormat() const { return XN_COMPRESSION_JPEG; }
	virtual XnFloat GetWorseCompressionRatio() const { return XN_STREAM_COMPRESSION_IMAGEJ_WORSE_RATIO; }
	virtual XnUInt32 GetOverheadSize() const { return 0; }

protected:
	virtual XnStatus CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize)
	{
		if (m_bRGB)
		{
			return XnStreamCompressImage24J(&m_CompJPEGContext, pData, pCompressedData, pnCompressedDataSize, m_nXRes, m_nYRes, m_nQuality);
		}
		else
		{
			return XnStreamCompressImage8J(&m_CompJPEGContext, pData, pCompressedData, pnCompressedDataSize, m_nXRes, m_nYRes, m_nQuality);
		}
	}

	virtual XnStatus DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize)
	{
		return XnStreamUncompressImageJ(&m_UncompJPEGContext, pCompressedData, nCompressedDataSize, pData, pnDataSize);
	}

private:
	const XnBool m_bRGB;
	const XnUInt32 m_nXRes;
	const XnUInt32 m_nYRes;
	const XnUInt32 m_nQuality;
	XnStreamCompJPEGContext m_CompJPEGContext;
	XnStreamUncompJPEGContext m_UncompJPEGContext;
};

#endif //__XN_JPEG_CODEC_H__