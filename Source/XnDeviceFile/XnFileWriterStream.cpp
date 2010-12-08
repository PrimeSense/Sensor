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
#include "XnFileWriterStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnFileWriterStream::XnFileWriterStream(const XnChar* strType, const XnChar* strName, XnDataPacker* pDataPacker) :
	XnStreamWriterStream(strType, strName, pDataPacker),
	m_nNumFramesPos(0),
	m_NumberOfFrames(XN_STREAM_PROPERTY_NUMBER_OF_FRAMES)
{
}

XnFileWriterStream::~XnFileWriterStream()
{
}

XnStatus XnFileWriterStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_ADD_PROPERTIES(this, &m_NumberOfFrames);

	nRetVal = XnStreamWriterStream::Init();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnFileWriterStream::WriteImpl(XnStreamData* pStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_NumberOfFrames.UnsafeUpdateValue(m_NumberOfFrames.GetValue() + 1);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnStreamWriterStream::WriteImpl(pStreamData);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}
