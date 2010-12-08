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
#include "XnStreamWriterStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamWriterStream::XnStreamWriterStream(const XnChar* strType, const XnChar* strName, XnDataPacker* pDataPacker) :
	XnDeviceStream(strType, strName),
	m_pDataPacker(pDataPacker),
	m_nFrameID(0)
{}

XnStreamWriterStream::~XnStreamWriterStream()
{}

XnStatus XnStreamWriterStream::WriteImpl(XnStreamData* pStreamData)
{
	m_nFrameID++;
	pStreamData->nFrameID = m_nFrameID;
	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterStream::CalcRequiredSize(XnUInt32* pnRequiredSize) const
{
	// we use the same size we have now
	*pnRequiredSize = GetRequiredDataSize();
	return XN_STATUS_OK;
}