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
		XN_VALIDATE_NEW(m_pS2DHelper, XnShiftToDepthStreamHelper);
		nRetVal = m_pS2DHelper->Init(GetStream());
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderStreamHolder::Free()
{
	if (m_pS2DHelper != NULL)
	{
		m_pS2DHelper->Free();
		XN_DELETE(m_pS2DHelper);
		m_pS2DHelper = NULL;
	}

	XnStreamDeviceStreamHolder::Free();

	return (XN_STATUS_OK);
}

