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





#ifndef __XN_WHOLE_PACKET_PROCESSOR_H__
#define __XN_WHOLE_PACKET_PROCESSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDataProcessor.h"

//---------------------------------------------------------------------------
// XnWholePacketProcessor Class
//---------------------------------------------------------------------------
class XnWholePacketProcessor : public XnDataProcessor
{
public:
	XnWholePacketProcessor(XnDevicePrivateData* pDevicePrivateData, const XnChar* csName, XnUInt32 nMaxPacketSize);
	~XnWholePacketProcessor();

	XnStatus Init();

//---------------------------------------------------------------------------
// Overridden Functions
//---------------------------------------------------------------------------
protected:
	void ProcessPacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize);

//---------------------------------------------------------------------------
// Virtual Functions
//---------------------------------------------------------------------------
protected:
	virtual void ProcessWholePacket(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData) = 0;

private:
	/* The maximum size of the packet. */
	XnUInt32 m_nMaxPacketSize;
	/* A buffer to store whole packet */
	XnBuffer m_WholePacket;
};

#endif //__XN_WHOLE_PACKET_PROCESSOR_H__
