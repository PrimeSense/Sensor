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
