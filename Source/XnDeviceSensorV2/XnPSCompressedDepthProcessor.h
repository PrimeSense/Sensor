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





#ifndef __XN_PS_COMPRESSED_DEPTH_PROCESSOR_H__
#define __XN_PS_COMPRESSED_DEPTH_PROCESSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDepthProcessor.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

class XnPSCompressedDepthProcessor : public XnDepthProcessor
{
public:
	XnPSCompressedDepthProcessor(XnSensorDepthStream* pStream, XnSensorStreamHelper* pHelper);
	virtual ~XnPSCompressedDepthProcessor();

	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize);
	virtual void OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader);
	virtual void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader);

	//---------------------------------------------------------------------------
	// Internal Functions
	//---------------------------------------------------------------------------
	XnStatus UncompressDepthPS(const XnUInt8* pInput, const XnUInt32 nInputSize,
		XnUInt16* pOutput, XnUInt32* pnOutputSize,
		XnUInt32* pnActualRead, XnBool bLastPart);

private:
	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
	/* Keeps raw data in case not all bytes can be processed. */
	XnBuffer m_RawData;

	static void XN_CALLBACK_TYPE OnRequiredSizeChanged();
};

#endif //__XN_PS_COMPRESSED_DEPTH_PROCESSOR_H__
