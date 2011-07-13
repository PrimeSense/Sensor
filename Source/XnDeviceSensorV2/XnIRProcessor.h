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





#ifndef __XN_IR_PROCESSOR_H__
#define __XN_IR_PROCESSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFrameStreamProcessor.h"
#include "XnSensorIRStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

class XnIRProcessor : public XnFrameStreamProcessor
{
public:
	XnIRProcessor(XnSensorIRStream* pStream, XnSensorStreamHelper* pHelper);
	virtual ~XnIRProcessor();

	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize);
	virtual void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader);
	virtual XnUInt64 GetTimeStamp(XnUInt32 nDeviceTimeStamp);

	//---------------------------------------------------------------------------
	// Internal Functions
	//---------------------------------------------------------------------------
private:
	XnUInt32 CalculateExpectedSize();
	XnStatus Unpack10to16(const XnUInt8* pcInput, const XnUInt32 nInputSize, XnUInt16* pnOutput, XnUInt32* pnActualRead, XnUInt32* pnOutputSize);
	inline XnSensorIRStream* GetStream()
	{
		return (XnSensorIRStream*)XnFrameStreamProcessor::GetStream();
	}

	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
private:
	/* A buffer to store bytes till we have enough to unpack. */
	XnBuffer m_ContinuousBuffer;
	XnBuffer m_UnpackedBuffer;
	XnUInt64 m_nRefTimestamp; // needed for firmware bug workaround 
};

#endif //__XN_IR_PROCESSOR_H__
