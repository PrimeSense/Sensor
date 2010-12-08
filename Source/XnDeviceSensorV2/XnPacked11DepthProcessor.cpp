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
#include "XnPacked11DepthProcessor.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/* The size of an input element in the stream. */
#define XN_INPUT_ELEMENT_SIZE 11
/* The size of an output element in the stream. */
#define XN_OUTPUT_ELEMENT_SIZE 16

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
/* Returns a set of <count> bits. For example XN_ON_BITS(4) returns 0xF */
#define XN_ON_BITS(count)				((1 << count)-1)

/* Creates a mask of <count> bits in offset <offset> */
#define XN_CREATE_MASK(count, offset)	(XN_ON_BITS(count) << offset)

/* Takes the <count> bits in offset <offset> from <source>.
*  For example: 
*  If we want 3 bits located in offset 2 from 0xF4:
*  11110100
*     ---
*  we get 101, which is 0x5.
*  and so, XN_TAKE_BITS(0xF4,3,2) == 0x5.
*/
#define XN_TAKE_BITS(source, count, offset)		((source & XN_CREATE_MASK(count, offset)) >> offset)

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnPacked11DepthProcessor::XnPacked11DepthProcessor(XnSensorDepthStream* pStream, XnSensorStreamHelper* pHelper) :
	XnDepthProcessor(pStream, pHelper)
{
}

XnStatus XnPacked11DepthProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnDepthProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_BUFFER_ALLOCATE(m_ContinuousBuffer, XN_INPUT_ELEMENT_SIZE);

	return (XN_STATUS_OK);
}

XnPacked11DepthProcessor::~XnPacked11DepthProcessor()
{
}

XnStatus XnPacked11DepthProcessor::Unpack11to16(const XnUInt8* pcInput, const XnUInt32 nInputSize, XnUInt32* pnActualRead)
{
	const XnUInt8* pOrigInput = pcInput;

	XnUInt32 nElements = nInputSize / XN_INPUT_ELEMENT_SIZE; // floored
	XnUInt32 nNeededOutput = nElements * XN_OUTPUT_ELEMENT_SIZE;

	*pnActualRead = 0;
	XnBuffer* pWriteBuffer = GetWriteBuffer();

	if (!CheckWriteBufferForOverflow(nNeededOutput))
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	XnUInt16* pnOutput = (XnUInt16*)pWriteBuffer->GetUnsafeWritePointer();

	// Convert the 11bit packed data into 16bit shorts
	for (XnUInt32 nElem = 0; nElem < nElements; ++nElem)
	{
		// input:	0,  1,  2,3,  4,  5,  6,7,  8,  9,10
		//			-,---,---,-,---,---,---,-,---,---,-
		// bits:	8,3,5,6,2,8,1,7,4,4,7,1,8,2,6,5,3,8
		//			---,---,-----,---,---,-----,---,---
		// output:	  0,  1,    2,  3,  4,    5,  6,  7

		pnOutput[0] = GetOutput((XN_TAKE_BITS(pcInput[0],8,0) << 3) | XN_TAKE_BITS(pcInput[1],3,5));
		pnOutput[1] = GetOutput((XN_TAKE_BITS(pcInput[1],5,0) << 6) | XN_TAKE_BITS(pcInput[2],6,2));
		pnOutput[2] = GetOutput((XN_TAKE_BITS(pcInput[2],2,0) << 9) | (XN_TAKE_BITS(pcInput[3],8,0) << 1) | XN_TAKE_BITS(pcInput[4],1,7));
		pnOutput[3] = GetOutput((XN_TAKE_BITS(pcInput[4],7,0) << 4) | XN_TAKE_BITS(pcInput[5],4,4));
		pnOutput[4] = GetOutput((XN_TAKE_BITS(pcInput[5],4,0) << 7) | XN_TAKE_BITS(pcInput[6],7,1));
		pnOutput[5] = GetOutput((XN_TAKE_BITS(pcInput[6],1,0) << 10) | (XN_TAKE_BITS(pcInput[7],8,0) << 2) | XN_TAKE_BITS(pcInput[8],2,6));
		pnOutput[6] = GetOutput((XN_TAKE_BITS(pcInput[8],6,0) << 5) | XN_TAKE_BITS(pcInput[9],5,3));
		pnOutput[7] = GetOutput((XN_TAKE_BITS(pcInput[9],3,0) << 8) | XN_TAKE_BITS(pcInput[10],8,0));

		pcInput += XN_INPUT_ELEMENT_SIZE;
		pnOutput += 8;
	}

	*pnActualRead = pcInput - pOrigInput;
	pWriteBuffer->UnsafeUpdateSize(nNeededOutput);

	return XN_STATUS_OK;
}

void XnPacked11DepthProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	XN_PROFILING_START_SECTION("XnPacked11DepthProcessor::ProcessFramePacketChunk")

	XnStatus nRetVal = XN_STATUS_OK;

	XnBuffer* pWriteBuffer = GetWriteBuffer();

	// check if we have data from previous packet
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
			Unpack11to16(m_ContinuousBuffer.GetData(), XN_INPUT_ELEMENT_SIZE, &nActualRead);
			m_ContinuousBuffer.Reset();
		}
	}

	// find out the number of input elements we have
	XnUInt32 nActualRead = 0;
	nRetVal = Unpack11to16(pData, nDataSize, &nActualRead);
	if (nRetVal == XN_STATUS_OK)
	{
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

void XnPacked11DepthProcessor::OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XnDepthProcessor::OnStartOfFrame(pHeader);
	m_ContinuousBuffer.Reset();
}

void XnPacked11DepthProcessor::OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XnDepthProcessor::OnEndOfFrame(pHeader);
	m_ContinuousBuffer.Reset();
}
