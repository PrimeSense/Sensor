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
#include "XnJpegImageProcessor.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnJpegImageProcessor::XnJpegImageProcessor(XnSensorImageStream* pStream, XnSensorStreamHelper* pHelper) :
	XnImageProcessor(pStream, pHelper)
{
}

XnJpegImageProcessor::~XnJpegImageProcessor()
{
	XnStreamFreeUncompressImageJ(&m_JPEGContext);
}

XnStatus XnJpegImageProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnImageProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_BUFFER_ALLOCATE(m_RawData, GetExpectedOutputSize());

	nRetVal = XnStreamInitUncompressImageJ(&m_JPEGContext);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnJpegImageProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	XN_PROFILING_START_SECTION("XnJpegImageProcessor::ProcessFramePacketChunk")

	// append to raw buffer
	if (m_RawData.GetFreeSpaceInBuffer() < nDataSize)
	{
		xnLogWarning(XN_MASK_SENSOR_PROTOCOL_IMAGE, "Bad overflow image! %d", m_RawData.GetSize());
		FrameIsCorrupted();
		m_RawData.Reset();
	}
	else
	{
		m_RawData.UnsafeWrite(pData, nDataSize);
	}

	XN_PROFILING_END_SECTION
}

void XnJpegImageProcessor::OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XnImageProcessor::OnStartOfFrame(pHeader);
	m_RawData.Reset();
}

void XnJpegImageProcessor::OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XN_PROFILING_START_SECTION("XnJpegImageProcessor::OnEndOfFrame")

	XnBuffer* pWriteBuffer = GetWriteBuffer();

	XnUInt32 nOutputSize = pWriteBuffer->GetMaxSize();
	XnStatus nRetVal = XnStreamUncompressImageJ(&m_JPEGContext, m_RawData.GetData(), m_RawData.GetSize(), pWriteBuffer->GetUnsafeWritePointer(), &nOutputSize);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SENSOR_PROTOCOL_IMAGE, "Failed to uncompress JPEG for frame %d: %s (%d)\n", GetCurrentFrameID(), xnGetStatusString(nRetVal), pWriteBuffer->GetSize());
		FrameIsCorrupted();

		XnDump badImageDump = XN_DUMP_CLOSED;
		xnDumpInit(&badImageDump, XN_DUMP_BAD_IMAGE, NULL, "BadImage_%d.jpeg", GetCurrentFrameID());
		xnDumpWriteBuffer(badImageDump, m_RawData.GetData(), m_RawData.GetSize());
		xnDumpClose(&badImageDump);
	}

	pWriteBuffer->UnsafeUpdateSize(nOutputSize);

	m_RawData.Reset();
	XnImageProcessor::OnEndOfFrame(pHeader);

	XN_PROFILING_END_SECTION
}

