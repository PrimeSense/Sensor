/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnUncompressedDepthProcessor.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnUncompressedDepthProcessor::XnUncompressedDepthProcessor(XnSensorDepthStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager) :
	XnDepthProcessor(pStream, pHelper, pBufferManager)
{
}

XnUncompressedDepthProcessor::~XnUncompressedDepthProcessor()
{
}

void XnUncompressedDepthProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* /*pHeader*/, const XnUChar* pData, XnUInt32 /*nDataOffset*/, XnUInt32 nDataSize)
{
	XN_PROFILING_START_SECTION("XnUncompressedDepthProcessor::ProcessFramePacketChunk")

	// when depth is uncompressed, we can just copy it directly to write buffer
	XnBuffer* pWriteBuffer = GetWriteBuffer();

	// Check there is enough room for the depth pixels
	if (CheckDepthBufferForOverflow(nDataSize))
	{
		// sometimes, when packets are lost, we get uneven number of bytes, so we need to complete
		// one byte, in order to keep UINT16 alignment
		if (nDataSize % 2 != 0)
		{
			nDataSize--;
			pData++;
		}

		// copy values. Make sure we do not get corrupted shifts
		XnUInt16* pRaw = (XnUInt16*)(pData);
		XnUInt16* pRawEnd = (XnUInt16*)(pData + nDataSize);
		XnDepthPixel* pDepthBuf = GetDepthOutputBuffer();
		XnDepthPixel* pShiftBuf = GetShiftsOutputBuffer();

		XnUInt16 shift;
		while (pRaw < pRawEnd)
		{
			shift = (((*pRaw) < (XN_DEVICE_SENSOR_MAX_SHIFT_VALUE-1)) ? (*pRaw) : 0);
			*pShiftBuf = shift;
			*pDepthBuf = GetOutput(shift);

			++pRaw;
			++pDepthBuf;
			++pShiftBuf;

		}

 		pWriteBuffer->UnsafeUpdateSize(nDataSize);
	}

	XN_PROFILING_END_SECTION
}

