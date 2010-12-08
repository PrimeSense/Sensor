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
#include "XnStreamReaderStreamHolder.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamReaderStreamHolder::XnStreamReaderStreamHolder(XnDeviceStream* pStream) :
	XnStreamDeviceStreamHolder(pStream, TRUE),
	m_pS2DHelper(NULL)
{}

XnStreamReaderStreamHolder::~XnStreamReaderStreamHolder()
{
	XnStreamReaderStreamHolder::Free();
}

XnStatus XnStreamReaderStreamHolder::Init(const XnActualPropertiesHash* pProps)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnStreamDeviceStreamHolder::Init(pProps);
	XN_IS_STATUS_OK(nRetVal);

	if (strcmp(GetStream()->GetType(), XN_STREAM_TYPE_DEPTH) == 0)
	{
		XN_VALIDATE_NEW(m_pS2DHelper, XnShiftToDepthStreamHelper, GetStream());
		nRetVal = m_pS2DHelper->Init();
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderStreamHolder::Free()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_pS2DHelper != NULL)
	{
		m_pS2DHelper->Free();
		XN_DELETE(m_pS2DHelper);
		m_pS2DHelper = NULL;
	}

	XnStreamDeviceStreamHolder::Free();

	return (XN_STATUS_OK);
}

