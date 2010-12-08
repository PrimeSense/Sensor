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
#include "XnUncompressedYUVtoRGBImageProcessor.h"
#include "YUV.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnUncompressedYUVtoRGBImageProcessor::XnUncompressedYUVtoRGBImageProcessor(XnSensorImageStream* pStream, XnSensorStreamHelper* pHelper) :
	XnImageProcessor(pStream, pHelper)
{
}

XnUncompressedYUVtoRGBImageProcessor::~XnUncompressedYUVtoRGBImageProcessor()
{
}

XnStatus XnUncompressedYUVtoRGBImageProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnImageProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_BUFFER_ALLOCATE(m_ContinuousBuffer, XN_YUV_TO_RGB_INPUT_ELEMENT_SIZE)
	
	return (XN_STATUS_OK);
}

void XnUncompressedYUVtoRGBImageProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	XN_PROFILING_START_SECTION("XnUncompressedYUVtoRGBImageProcessor::ProcessFramePacketChunk")

	XnBuffer* pWriteBuffer = GetWriteBuffer();

	if (m_ContinuousBuffer.GetSize() != 0)
	{
		// fill in to a whole element
		XnUInt32 nReadBytes = XN_MIN(nDataSize, XN_YUV_TO_RGB_INPUT_ELEMENT_SIZE - m_ContinuousBuffer.GetSize());
		m_ContinuousBuffer.UnsafeWrite(pData, nReadBytes);
		pData += nReadBytes;
		nDataSize -= nReadBytes;

		if (m_ContinuousBuffer.GetSize() == XN_YUV_TO_RGB_INPUT_ELEMENT_SIZE)
		{
			if (CheckWriteBufferForOverflow(XN_YUV_TO_RGB_OUTPUT_ELEMENT_SIZE))
			{
				// process it
				XnUInt32 nActualRead = 0;
				XnUInt32 nOutputSize = pWriteBuffer->GetFreeSpaceInBuffer();
				YUV422ToRGB888(m_ContinuousBuffer.GetData(), pWriteBuffer->GetUnsafeWritePointer(), XN_YUV_TO_RGB_INPUT_ELEMENT_SIZE, &nActualRead, &nOutputSize);
				pWriteBuffer->UnsafeUpdateSize(XN_YUV_TO_RGB_OUTPUT_ELEMENT_SIZE);
			}

			m_ContinuousBuffer.Reset();
		}
	}

	if (CheckWriteBufferForOverflow(nDataSize / XN_YUV_TO_RGB_INPUT_ELEMENT_SIZE * XN_YUV_TO_RGB_OUTPUT_ELEMENT_SIZE))
	{
		XnUInt32 nActualRead = 0;
		XnUInt32 nOutputSize = pWriteBuffer->GetFreeSpaceInBuffer();
		YUV422ToRGB888(pData, pWriteBuffer->GetUnsafeWritePointer(), nDataSize, &nActualRead, &nOutputSize);
		pWriteBuffer->UnsafeUpdateSize(nOutputSize);
		pData += nActualRead;
		nDataSize -= nActualRead;

		// if we have any bytes left, store them for next packet.
		if (nDataSize > 0)
		{
			// no need to check for overflow. there can not be a case in which more than XN_INPUT_ELEMENT_SIZE
			// are left.
			m_ContinuousBuffer.UnsafeWrite(pData, nDataSize);
		}
	}

	XN_PROFILING_END_SECTION
}

void XnUncompressedYUVtoRGBImageProcessor::OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XnImageProcessor::OnStartOfFrame(pHeader);
	m_ContinuousBuffer.Reset();
}

void XnUncompressedYUVtoRGBImageProcessor::OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XnImageProcessor::OnEndOfFrame(pHeader);
	m_ContinuousBuffer.Reset();
}
