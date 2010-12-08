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
#include "XnFrameStream.h"
#include "XnStreamDataInternal.h"
#include "XnSimpleBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnFrameStream::XnFrameStream(const XnChar* csType, const XnChar* csName) :
	XnDeviceStream(csType, csName),
	m_IsFrameStream(XN_STREAM_PROPERTY_IS_FRAME_BASED, TRUE),
	m_FPS(XN_STREAM_PROPERTY_FPS, 0),
	m_LastRawFrame(XN_STREAM_PROPERTY_LAST_RAW_FRAME),
	m_nLastReadFrame(0),
	m_bTripleBufferReallocated(FALSE),
	m_pBufferManager(NULL),
	m_pBufferPool(NULL),
	m_bPoolAllocated(FALSE)
{
	m_FPS.UpdateSetCallback(SetFPSCallback, this);
	m_LastRawFrame.UpdateGetCallback(GetLastRawFrameCallback, this);
}

XnStatus XnFrameStream::SetBufferPool(XnBufferPool* pBufferPool)
{
	// we only allow this if no buffer pool exists
	if (m_pBufferPool != NULL)
	{
		return XN_STATUS_ERROR;
	}

	m_pBufferPool = pBufferPool;
	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnDeviceStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_ADD_PROPERTIES(this, &m_IsFrameStream, &m_FPS, &m_LastRawFrame);

	XnCallbackHandle hDummy;

	// be notified when required size changes
	nRetVal = RequiredSizeProperty().OnChangeEvent().Register(RequiredSizeChangedCallback, this, &hDummy);
	XN_IS_STATUS_OK(nRetVal);

	if (m_pBufferPool == NULL)
	{
		XN_VALIDATE_NEW(m_pBufferPool, XnSimpleBufferPool, 3);
		m_bPoolAllocated = TRUE;
	}

	// allocate buffer manager
	XN_VALIDATE_NEW(m_pBufferManager, XnFrameBufferManager, m_pBufferPool);

	nRetVal = m_pBufferManager->Init(GetRequiredDataSize());
	XN_IS_STATUS_OK(nRetVal);

	// register for new data events
	nRetVal = m_pBufferManager->OnNewFrameEvent().Register(OnTripleBufferNewData, this, &hDummy);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::Free()
{
	if (m_pBufferManager != NULL)
	{
		XN_DELETE(m_pBufferManager);
		m_pBufferManager = NULL;
	}

	if (m_bPoolAllocated && m_pBufferPool != NULL)
	{
		XN_DELETE(m_pBufferPool);
		m_pBufferPool = NULL;
		m_bPoolAllocated = FALSE;
	}

	XnDeviceStream::Free();
	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::CreateStreamData(XnStreamData** ppStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnStreamData* pStreamData;

	// we create a StreamData object with no buffer allocated. The buffer will just be 
	// a pointer to the triple buffer
	nRetVal = XnStreamDataCreateNoBuffer(&pStreamData, GetName());
	XN_IS_STATUS_OK(nRetVal);

	// However, we don't want the user to get a null pointer, even if no new frame yet,
	// so we'll initialize the data with one of the buffers
	nRetVal = m_pBufferPool->GetBuffer(&pStreamData->pInternal->pLockedBuffer);
	if (nRetVal != XN_STATUS_OK)
	{
		XnStreamDataDestroy(&pStreamData);
		return (nRetVal);
	}

	pStreamData->pData = (void*)pStreamData->pInternal->pLockedBuffer->GetData();

	*ppStreamData = pStreamData;
	
	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::SetFPS(XnUInt32 nFPS)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_FPS.UnsafeUpdateValue(nFPS);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::OnRequiredSizeChanging()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ReallocTripleFrameBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);	
}

XnStatus XnFrameStream::ReallocTripleFrameBuffer()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_pBufferManager->Reallocate(GetRequiredDataSize());
	XN_IS_STATUS_OK(nRetVal);

	m_bTripleBufferReallocated = TRUE;

	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::Read(XnStreamData* pStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnDeviceStream::Read(pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	m_bTripleBufferReallocated = FALSE;
	
	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::ReadImpl(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// release previous buffer
	m_pBufferPool->DecRef(pStreamOutput->pInternal->pLockedBuffer);

	m_pBufferManager->ReadLastStableBuffer(
		&pStreamOutput->pInternal->pLockedBuffer, 
		&pStreamOutput->nTimestamp,
		&pStreamOutput->nFrameID);

	pStreamOutput->pData = (void*)pStreamOutput->pInternal->pLockedBuffer->GetData();
	pStreamOutput->nDataSize = pStreamOutput->pInternal->pLockedBuffer->GetSize();
	pStreamOutput->nFrameID = ++m_nLastReadFrame;

	nRetVal = PostProcessFrame(pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnFrameStream::HandleNoNewData(XnStreamData* pStreamOutput)
{
/*	// No new data, but if triple buffer was reallocated, we want the user buffer to be replaced
	// (so that it has a buffer with a size that matches what he expects, even if no new data in it)
	if (m_bTripleBufferReallocated && !pStreamOutput->pInternal->bAllocated)
	{
		XnFrameBuffer* pStableBuffer = m_pBufferManager->ReadLastStableBuffer();
		pStreamOutput->pData = (void*)pStableBuffer->DataBuf.GetData();
	}
*/
	return XN_STATUS_OK;
}

XnStatus XnFrameStream::GetLastRawFrame(XnDynamicSizeBuffer* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_pBufferManager->CopyLastStableBuffer(pBuffer->pData, pBuffer->nMaxSize, &pBuffer->nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnFrameStream::SetFPSCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie)
{
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	return pThis->SetFPS((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnFrameStream::RequiredSizeChangedCallback(const XnProperty* pSenser, void* pCookie)
{
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	return pThis->OnRequiredSizeChanging();
}

void XN_CALLBACK_TYPE XnFrameStream::OnTripleBufferNewData(XnFrameBufferManager* pTripleBuffer, XnUInt64 nTimestamp, void* pCookie)
{
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	pThis->NewDataAvailable(nTimestamp, pThis->m_nLastReadFrame + 1);
}

XnStatus XN_CALLBACK_TYPE XnFrameStream::GetLastRawFrameCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnDynamicSizeBuffer);
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	XnDynamicSizeBuffer* pBuffer = (XnDynamicSizeBuffer*)gbValue.pData;
	return pThis->GetLastRawFrame(pBuffer);
}
