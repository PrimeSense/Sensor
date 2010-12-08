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






//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnUncompressedYUVImageProcessor.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnUncompressedYUVImageProcessor::XnUncompressedYUVImageProcessor(XnSensorImageStream* pStream, XnSensorStreamHelper* pHelper) :
	XnImageProcessor(pStream, pHelper)
{
}

XnUncompressedYUVImageProcessor::~XnUncompressedYUVImageProcessor()
{
}

void XnUncompressedYUVImageProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	XN_PROFILING_START_SECTION("XnUncompressedYUVImageProcessor::ProcessFramePacketChunk")

	// when image is uncompressed, we can just copy it directly to write buffer
	XnBuffer* pWriteBuffer = GetWriteBuffer();

	// make sure we have enough room
	if (CheckWriteBufferForOverflow(nDataSize))
	{
		pWriteBuffer->UnsafeWrite(pData, nDataSize);
	}

	XN_PROFILING_END_SECTION
}
