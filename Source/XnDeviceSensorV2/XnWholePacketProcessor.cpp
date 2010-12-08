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
#include "XnWholePacketProcessor.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnWholePacketProcessor::XnWholePacketProcessor(XnDevicePrivateData* pDevicePrivateData, const XnChar* csName, XnUInt32 nMaxPacketSize) :
	XnDataProcessor(pDevicePrivateData, csName),
	m_nMaxPacketSize(nMaxPacketSize)
{}

XnWholePacketProcessor::~XnWholePacketProcessor()
{}

XnStatus XnWholePacketProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnDataProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_WholePacket.Allocate(m_nMaxPacketSize);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

void XnWholePacketProcessor::ProcessPacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	if (nDataOffset == 0 && m_WholePacket.GetSize() != 0)
	{
		// previous packet was not received to its end
		xnLogWarning(XN_MASK_SENSOR_PROTOCOL, "%s: Expected %d additional bytes in packet (got %d out of %d bytes)!", m_csName, pHeader->nBufSize - m_WholePacket.GetSize(), m_WholePacket.GetSize(), pHeader->nBufSize);
		m_WholePacket.Reset();
	}

	// sanity check
	if (pHeader->nBufSize > m_WholePacket.GetMaxSize())
	{
		xnLogWarning(XN_MASK_SENSOR_PROTOCOL, "Got a packet which is bigger than max size! (%d > %d)", pHeader->nBufSize, m_WholePacket.GetMaxSize());
	}
	else
	{
		// append data
		m_WholePacket.UnsafeWrite(pData, nDataSize);

		// check if we have entire packet
		if (m_WholePacket.GetSize() == pHeader->nBufSize)
		{
			// process it
			ProcessWholePacket(pHeader, m_WholePacket.GetData());
			m_WholePacket.Reset();
		}
	}
}
