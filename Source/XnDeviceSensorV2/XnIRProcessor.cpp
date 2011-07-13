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
#include "XnIRProcessor.h"
#include <XnProfiling.h>
#include "XnSensor.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------

/* The size of an input element for unpacking. */
#define XN_INPUT_ELEMENT_SIZE 5
/* The size of an output element for unpacking. */
#define XN_OUTPUT_ELEMENT_SIZE 8

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnIRProcessor::XnIRProcessor(XnSensorIRStream* pStream, XnSensorStreamHelper* pHelper) :
	XnFrameStreamProcessor(pStream, pHelper, XN_SENSOR_PROTOCOL_RESPONSE_IMAGE_START, XN_SENSOR_PROTOCOL_RESPONSE_IMAGE_END),
	m_nRefTimestamp(0)
{
}

XnIRProcessor::~XnIRProcessor()
{
}

XnStatus XnIRProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnFrameStreamProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_BUFFER_ALLOCATE(m_ContinuousBuffer, XN_INPUT_ELEMENT_SIZE);

	switch (GetStream()->GetOutputFormat())
	{
	case XN_OUTPUT_FORMAT_GRAYSCALE16:
		break;
	case XN_OUTPUT_FORMAT_RGB24:
		XN_VALIDATE_BUFFER_ALLOCATE(m_UnpackedBuffer, GetExpectedOutputSize());
		break;
	}

	return (XN_STATUS_OK);
}

XnStatus XnIRProcessor::Unpack10to16(const XnUInt8* pcInput, const XnUInt32 nInputSize, XnUInt16* pnOutput, XnUInt32* pnActualRead, XnUInt32* pnOutputSize)
{
	XnInt32 cInput = 0;
	const XnUInt8* pOrigInput = pcInput;

	XnUInt32 nElements = nInputSize / XN_INPUT_ELEMENT_SIZE; // floored
	XnUInt32 nNeededOutput = nElements * XN_OUTPUT_ELEMENT_SIZE;

	*pnActualRead = 0;

	if (*pnOutputSize < nNeededOutput)
	{
		*pnOutputSize = 0;
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	// Convert the 10bit packed data into 16bit shorts

	for (XnUInt32 nElem = 0; nElem < nElements; ++nElem)
	{
		//1a 
		cInput = *pcInput;
		*pnOutput = (cInput & 0xFF) << 2;

		//1b
		pcInput++;
		cInput = *pcInput;
		*pnOutput = *pnOutput | ((cInput & 0xC0) >> 6);
		pnOutput++;

		//2a
		*pnOutput = (cInput & 0x3F) << 4;

		//2b
		pcInput++;
		cInput = *pcInput;
		*pnOutput = *pnOutput | ((cInput & 0xF0) >> 4);
		pnOutput++;

		//3a
		*pnOutput = (cInput & 0x0F) << 6;

		//3b
		pcInput++;
		cInput = *pcInput;
		*pnOutput = *pnOutput | ((cInput & 0xFC) >> 2);
		pnOutput++;

		//4a
		*pnOutput = (cInput & 0x3) << 8;

		//4b
		pcInput++;
		cInput = *pcInput;
		*pnOutput = *pnOutput | (cInput & 0xFF);
		pnOutput++;

		pcInput++;
	}

	*pnActualRead = pcInput - pOrigInput;
	*pnOutputSize = nNeededOutput;
	return XN_STATUS_OK;
}

void XnIRProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	XN_PROFILING_START_SECTION("XnIRProcessor::ProcessFramePacketChunk")

	// if output format is Gray16, we can write directly to output buffer. otherwise, we need
	// to write to a temp buffer.
	XnBuffer* pWriteBuffer = (GetStream()->GetOutputFormat() == XN_OUTPUT_FORMAT_GRAYSCALE16) ? GetWriteBuffer() : &m_UnpackedBuffer;

	if (m_ContinuousBuffer.GetSize() != 0)
	{
		// fill in to a whole element
		XnUInt32 nReadBytes = XN_MIN(nDataSize, XN_INPUT_ELEMENT_SIZE - m_ContinuousBuffer.GetSize());
		m_ContinuousBuffer.UnsafeWrite(pData, nReadBytes);
		pData += nReadBytes;
		nDataSize -= nReadBytes;

		if (m_ContinuousBuffer.GetSize() == XN_INPUT_ELEMENT_SIZE)
		{
			// process it
			XnUInt32 nActualRead = 0;
			XnUInt32 nOutputSize = pWriteBuffer->GetFreeSpaceInBuffer();
			if (XN_STATUS_OK != Unpack10to16(m_ContinuousBuffer.GetData(), XN_INPUT_ELEMENT_SIZE, (XnUInt16*)pWriteBuffer->GetUnsafeWritePointer(), &nActualRead, &nOutputSize))
				WriteBufferOverflowed();
			else
				pWriteBuffer->UnsafeUpdateSize(nOutputSize);

			m_ContinuousBuffer.Reset();
		}
	}

	XnUInt32 nActualRead = 0;
	XnUInt32 nOutputSize = pWriteBuffer->GetFreeSpaceInBuffer();
	if (XN_STATUS_OK != Unpack10to16(pData, nDataSize, (XnUInt16*)pWriteBuffer->GetUnsafeWritePointer(), &nActualRead, &nOutputSize))
	{
		WriteBufferOverflowed();
	}
	else
	{
		pWriteBuffer->UnsafeUpdateSize(nOutputSize);

		pData += nActualRead;
		nDataSize -= nActualRead;

		// if we have any bytes left, store them for next packet
		if (nDataSize > 0)
		{
			// no need to check for overflow. there can not be a case in which more than XN_INPUT_ELEMENT_SIZE
			// are left.
			m_ContinuousBuffer.UnsafeWrite(pData, nDataSize);
		}
	}

	XN_PROFILING_END_SECTION
}

void IRto888(XnUInt16* pInput, XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize)
{
	XnUInt16* pInputEnd = pInput + nInputSize;
	XnUInt8* pOutputOrig = pOutput;
	XnUInt8* pOutputEnd = pOutput + *pnOutputSize;

	while (pInput != pInputEnd && pOutput < pOutputEnd)
	{
		*pOutput = (*pInput)>>2;
		*(pOutput+1) = *pOutput;
		*(pOutput+2) = *pOutput;

		pOutput+=3;
		pInput++;
	}

	*pnOutputSize = pOutput - pOutputOrig;
}

XnUInt32 XnIRProcessor::CalculateExpectedSize()
{
	XnUInt32 nXRes = GetStream()->GetXRes();
	XnUInt32 nYRes = GetStream()->GetYRes();

	// when cropping is turned on, actual depth size is smaller
	if (GetStream()->m_FirmwareCropEnabled.GetValue() == TRUE)
	{
		nXRes = (XnUInt32)GetStream()->m_FirmwareCropSizeX.GetValue();
		nYRes = (XnUInt32)GetStream()->m_FirmwareCropSizeY.GetValue();
	}
	else if (GetStream()->GetResolution() != XN_RESOLUTION_SXGA)
	{
		// there are additional 8 rows
		nYRes += 8;
	}

	return nXRes * nYRes * GetStream()->GetBytesPerPixel();
}

void XnIRProcessor::OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XN_PROFILING_START_SECTION("XnIRProcessor::OnEndOfFrame")

	// if there are bytes left in continuous buffer, then we have a corrupt frame
	if (m_ContinuousBuffer.GetSize() != 0)
	{
		xnLogWarning(XN_MASK_SENSOR_READ, "IR buffer is corrupt. There are left over bytes (invalid size)");
		FrameIsCorrupted();
	}

	// if data was written to temp buffer, convert it now
	switch (GetStream()->GetOutputFormat())
	{
	case XN_OUTPUT_FORMAT_GRAYSCALE16:
		break;
	case XN_OUTPUT_FORMAT_RGB24:
		{
			XnUInt32 nOutputSize = GetWriteBuffer()->GetFreeSpaceInBuffer();
			IRto888((XnUInt16*)m_UnpackedBuffer.GetData(), m_UnpackedBuffer.GetSize() / sizeof(XnUInt16), GetWriteBuffer()->GetUnsafeWritePointer(), &nOutputSize);
			GetWriteBuffer()->UnsafeUpdateSize(nOutputSize);
			m_UnpackedBuffer.Reset();
		}
		break;
	}

	XnUInt32 nExpectedBufferSize = CalculateExpectedSize();
	if (GetWriteBuffer()->GetSize() != nExpectedBufferSize)
	{
		xnLogWarning(XN_MASK_SENSOR_READ, "IR buffer is corrupt. Size is %u (!= %u)", GetWriteBuffer()->GetSize(), nExpectedBufferSize);
		FrameIsCorrupted();
	}

	XnFrameStreamProcessor::OnEndOfFrame(pHeader);
	m_ContinuousBuffer.Reset();

	XN_PROFILING_END_SECTION
}

XnUInt64 XnIRProcessor::GetTimeStamp(XnUInt32 nDeviceTimeStamp)
{
	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);

	// There's a firmware bug, causing IR timestamps not to advance if depth stream is off.
	// If so, we need to create our own timestamps.
	if (m_pDevicePrivateData->pSensor->GetFirmware()->GetParams()->m_Stream1Mode.GetValue() != XN_VIDEO_STREAM_DEPTH)
	{
		if (m_nRefTimestamp == 0)
		{
			m_nRefTimestamp = nNow;
		}

		return nNow - m_nRefTimestamp;
	}
	else
	{
		XnUInt64 nResult = XnFrameStreamProcessor::GetTimeStamp(nDeviceTimeStamp);

		// keep it as ref so that if depth is turned off, we'll continue from there
		m_nRefTimestamp = nNow - nResult;

		return nResult;
	}
}