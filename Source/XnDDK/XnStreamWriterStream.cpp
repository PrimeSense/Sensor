/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.x Alpha                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Sensor                                    *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
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