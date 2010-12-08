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
#include "XnStreamingStream.h"
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamingStream::XnStreamingStream(const XnChar* csType, const XnChar* csName) :
	XnDeviceStream(csType, csName),
	m_IsStreamingStream(XN_STREAM_PROPERTY_IS_STREAMING, TRUE),
	m_ReadChunkSize(XN_STREAM_PROPERTY_READ_CHUNK_SIZE)
{
}

XnStatus XnStreamingStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnDeviceStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	m_ReadChunkSize.UpdateSetCallback(SetReadChunkSizeCallback, this);

	XN_VALIDATE_ADD_PROPERTIES(this, &m_IsStreamingStream, &m_ReadChunkSize);

	return (XN_STATUS_OK);
}

XnStatus XnStreamingStream::SetReadChunkSize(XnUInt32 nChunkSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_ReadChunkSize.UnsafeUpdateValue(nChunkSize);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnStreamingStream::SetReadChunkSizeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnStreamingStream* pStream = (XnStreamingStream*)pCookie;
	return pStream->SetReadChunkSize((XnUInt32)nValue);
}
