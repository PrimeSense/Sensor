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
#include "XnUncompressedBayerProcessor.h"
#include "Uncomp.h"
#include "Bayer.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnUncompressedBayerProcessor::XnUncompressedBayerProcessor(XnSensorImageStream* pStream, XnSensorStreamHelper* pHelper) :
	XnImageProcessor(pStream, pHelper)
{
}

XnUncompressedBayerProcessor::~XnUncompressedBayerProcessor()
{
}

XnStatus XnUncompressedBayerProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnImageProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	switch (GetStream()->GetOutputFormat())
	{
	case XN_OUTPUT_FORMAT_GRAYSCALE8:
		break;
	case XN_OUTPUT_FORMAT_RGB24:
		XN_VALIDATE_BUFFER_ALLOCATE(m_UncompressedBayerBuffer, GetExpectedOutputSize());
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_PROTOCOL_IMAGE, "Unsupported image output format: %d", GetStream()->GetOutputFormat());
	}

	return (XN_STATUS_OK);
}

void XnUncompressedBayerProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	XN_PROFILING_START_SECTION("XnUncompressedBayerProcessor::ProcessFramePacketChunk")

	// if output format is Gray8, we can write directly to output buffer. otherwise, we need
	// to write to a temp buffer.
	XnBuffer* pWriteBuffer = (GetStream()->GetOutputFormat() == XN_OUTPUT_FORMAT_GRAYSCALE8) ? GetWriteBuffer() : &m_UncompressedBayerBuffer;

	// make sure we have enough room
	if (CheckWriteBufferForOverflow(nDataSize))
	{
		pWriteBuffer->UnsafeWrite(pData, nDataSize);
	}

	XN_PROFILING_END_SECTION
}

void XnUncompressedBayerProcessor::OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XN_PROFILING_START_SECTION("XnUncompressedBayerProcessor::OnEndOfFrame")

		// if data was written to temp buffer, convert it now
		switch (GetStream()->GetOutputFormat())
	{
		case XN_OUTPUT_FORMAT_GRAYSCALE8:
			break;
		case XN_OUTPUT_FORMAT_RGB24:
			{
				Bayer2RGB888(m_UncompressedBayerBuffer.GetData(), GetWriteBuffer()->GetUnsafeWritePointer(), GetActualXRes(), GetActualYRes(), 1, 0);
				GetWriteBuffer()->UnsafeUpdateSize(GetActualXRes()*GetActualYRes()*3);
				m_UncompressedBayerBuffer.Reset();
			}
			break;
	}

	XnImageProcessor::OnEndOfFrame(pHeader);

	XN_PROFILING_END_SECTION
}
