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
#include "XnStreamReaderStream.h"
#include "XnStreamDataInternal.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamReaderStream::XnStreamReaderStream(const XnChar* strType, const XnChar* strName) :
	XnDeviceStream(strType, strName),
	m_pLastData(NULL),
	m_nLastFrameIDFromStream(0)
{}

XnStreamReaderStream::~XnStreamReaderStream()
{
	XnStreamReaderStream::Free();
}

XnStatus XnStreamReaderStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnDeviceStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	// register for size change (so we can realloc stream data)
	XnCallbackHandle hCallback;
	nRetVal = RequiredSizeProperty().OnChangeEvent().Register(RequiredSizeChangedCallback, this, hCallback);
	XN_IS_STATUS_OK(nRetVal);

	// and create stream data
	nRetVal = XnStreamDataCreate(&m_pLastData, GetName(), GetRequiredDataSize());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderStream::Free()
{
	if (m_pLastData != NULL)
	{
		XnStreamDataDestroy(&m_pLastData);
		m_pLastData = NULL;
	}

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderStream::ReadImpl(XnStreamData* pStreamData)
{
	pStreamData->nFrameID = m_pLastData->nFrameID;
	pStreamData->nTimestamp = m_pLastData->nTimestamp;

	if (pStreamData->pInternal->bAllocated)
	{
		// don't take more than required size
		pStreamData->nDataSize = XN_MIN(m_pLastData->nDataSize, GetRequiredDataSize());
		xnOSMemCopy(pStreamData->pData, m_pLastData->pData, pStreamData->nDataSize);
	}
	else
	{
		pStreamData->nDataSize = m_pLastData->nDataSize;
		pStreamData->pData = m_pLastData->pData;
	}

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderStream::CalcRequiredSize(XnUInt32* pnRequiredSize) const
{
	// we use the same size we have now
	*pnRequiredSize = GetRequiredDataSize();
	return XN_STATUS_OK;
}

void XnStreamReaderStream::NewDataAvailable(XnUInt64 nTimestamp, XnUInt32 /*nFrameID*/)
{
	m_pLastData->nTimestamp = nTimestamp;
	m_pLastData->nFrameID = ++m_nLastFrameIDFromStream;
	XnDeviceStream::NewDataAvailable(m_pLastData->nTimestamp, m_pLastData->nFrameID);
}

void XnStreamReaderStream::ReMarkDataAsNew()
{
	XnDeviceStream::NewDataAvailable(m_pLastData->nTimestamp, m_pLastData->nFrameID);
}

void XnStreamReaderStream::Reset()
{
	m_nLastFrameIDFromStream = 0;
	xnOSMemSet(m_pLastData->pData, 0, m_pLastData->pInternal->nAllocSize);
	m_pLastData->nDataSize = 0;
	m_pLastData->nFrameID = 0;
	m_pLastData->nTimestamp = 0;
	m_pLastData->bIsNew = FALSE;
	XnDeviceStream::ResetLastTimestampAndFrameID();
}

XnStatus XnStreamReaderStream::OnRequiredSizeChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnStreamDataUpdateSize(m_pLastData, GetRequiredDataSize());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderStream::RequiredSizeChangedCallback(const XnProperty* /*pSender*/, void* pCookie)
{
	XnStreamReaderStream* pThis = (XnStreamReaderStream*)pCookie;
	return pThis->OnRequiredSizeChanged();
}
