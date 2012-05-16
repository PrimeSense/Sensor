/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFrameStream.h"
#include "XnStreamDataInternal.h"
#include "XnSimpleBufferPool.h"
#include "XnExternalBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnFrameStream::XnFrameStream(const XnChar* csType, const XnChar* csName) :
	XnDeviceStream(csType, csName),
	m_IsFrameStream(XN_STREAM_PROPERTY_IS_FRAME_BASED, TRUE),
	m_FPS(XN_STREAM_PROPERTY_FPS, 0),
	m_LastRawFrame(XN_STREAM_PROPERTY_LAST_RAW_FRAME),
	m_externalBufferPool(XN_STREAM_PROPERTY_EXTERNAL_BUFFER_POOL),
	m_nLastReadFrame(0),
	m_bTripleBufferReallocated(FALSE),
	m_pBufferManager(NULL),
	m_pBufferPool(NULL),
	m_bPoolAllocated(FALSE)
{
	m_FPS.UpdateSetCallback(SetFPSCallback, this);
	m_LastRawFrame.UpdateGetCallback(GetLastRawFrameCallback, this);
	m_externalBufferPool.UpdateSetCallback(SetExternalBufferPoolCallback, this);
}

XnStatus XnFrameStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnDeviceStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_ADD_PROPERTIES(this, &m_IsFrameStream, &m_FPS, &m_LastRawFrame, &m_externalBufferPool );

	XnCallbackHandle hDummy;

	// be notified when required size changes
	nRetVal = RequiredSizeProperty().OnChangeEvent().Register(RequiredSizeChangedCallback, this, hDummy);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::GetTripleBuffer(XnFrameBufferManager** pBufferManager)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// lazy initialization (this allows us to set buffer pool after initialization of the stream
	// and before data actually arrives (or stream data is allocated)
	if (m_pBufferManager == NULL)
	{
		if (m_pBufferPool == NULL)
		{
			XN_VALIDATE_NEW(m_pBufferPool, XnSimpleBufferPool, 3);
			m_bPoolAllocated = TRUE;

			nRetVal = m_pBufferPool->Init(GetRequiredDataSize());
			XN_IS_STATUS_OK(nRetVal);
		}

		// allocate buffer manager
		XN_VALIDATE_NEW(m_pBufferManager, XnFrameBufferManager, m_pBufferPool);

		nRetVal = m_pBufferManager->Init(GetRequiredDataSize());
		XN_IS_STATUS_OK(nRetVal);

		// register for new data events
		XnCallbackHandle hDummy;
		nRetVal = m_pBufferManager->OnNewFrameEvent().Register(OnTripleBufferNewData, this, hDummy);
		XN_IS_STATUS_OK(nRetVal);
	}

	*pBufferManager = m_pBufferManager;

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

	// NOTE: in any case, we must make sure data is not null, because some old applications
	// counts on it (they might read the data before the first frame).

	// check if the buffer pool has been set yet
	if (m_pBufferPool == NULL)
	{
		// Create it with a buffer. This buffer will be later on freed when buffers from 
		// the buffer pool will be used.
		nRetVal = XnStreamDataCreate(&pStreamData, GetName(), GetRequiredDataSize());
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// we create a StreamData object with no buffer allocated. The buffer will just be 
		// a pointer from the buffer pool
		nRetVal = XnStreamDataCreateNoBuffer(&pStreamData, GetName());
		XN_IS_STATUS_OK(nRetVal);

		// take a buffer from the pool
		nRetVal = m_pBufferPool->GetBuffer(&pStreamData->pInternal->pLockedBuffer);
		if (nRetVal != XN_STATUS_OK)
		{
			XnStreamDataDestroy(&pStreamData);
			return (nRetVal);
		}

		pStreamData->pData = (void*)pStreamData->pInternal->pLockedBuffer->GetData();
	}

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

	// if the stream already notified it has new data, this must be undone, as we don't want
	// the application getting frames of unexpected size
	ResetNewDataAvailableFlag();

	nRetVal = ReallocTripleFrameBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);	
}

XnStatus XnFrameStream::ReallocTripleFrameBuffer()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_pBufferManager != NULL)
	{
		nRetVal = m_pBufferManager->Reallocate(GetRequiredDataSize());
		XN_IS_STATUS_OK(nRetVal);

		m_bTripleBufferReallocated = TRUE;
	}

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
	if (pStreamOutput->pInternal->pLockedBuffer != NULL)
	{
		m_pBufferPool->DecRef(pStreamOutput->pInternal->pLockedBuffer);
	}
	else if (pStreamOutput->pInternal->nAllocSize > 0)
	{
		// no need for this buffer, we're replacing it with a pointer to the 
		// buffer pool
		nRetVal = XnStreamDataUpdateSize(pStreamOutput, 0);
		XN_IS_STATUS_OK(nRetVal);
	}

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

XnStatus XnFrameStream::GetLastRawFrame(XnDynamicSizeBuffer* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_pBufferManager->CopyLastStableBuffer(pBuffer->pData, pBuffer->nMaxSize, &pBuffer->nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFrameStream::SetExternalBufferPool(XnUInt32 nCount, XnGeneralBuffer* aBuffers)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_pBufferPool != NULL)
	{
		xnLogError(XN_MASK_DDK, "Cannot change buffer pool.");
		return XN_STATUS_DEVICE_PROPERTY_READ_ONLY;
	}

	XnExternalBufferPool* pExternalBufferPool;
	XN_VALIDATE_NEW(pExternalBufferPool, XnExternalBufferPool);

	nRetVal = pExternalBufferPool->SetBuffers(nCount, aBuffers);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pExternalBufferPool);
		return (nRetVal);
	}

	nRetVal = pExternalBufferPool->Init(GetRequiredDataSize());
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pExternalBufferPool);
		return (nRetVal);
	}

	m_pBufferPool = pExternalBufferPool;

	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnFrameStream::SetFPSCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	return pThis->SetFPS((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnFrameStream::RequiredSizeChangedCallback(const XnProperty* /*pSender*/, void* pCookie)
{
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	return pThis->OnRequiredSizeChanging();
}

void XN_CALLBACK_TYPE XnFrameStream::OnTripleBufferNewData(const XnFrameBufferManager::NewFrameEventArgs& args, void* pCookie)
{
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	pThis->NewDataAvailable(args.nTimestamp, pThis->m_nLastReadFrame + 1);
}

XnStatus XN_CALLBACK_TYPE XnFrameStream::GetLastRawFrameCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnDynamicSizeBuffer);
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	XnDynamicSizeBuffer* pBuffer = (XnDynamicSizeBuffer*)gbValue.pData;
	return pThis->GetLastRawFrame(pBuffer);
}

XnStatus XN_CALLBACK_TYPE XnFrameStream::SetExternalBufferPoolCallback(XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XnFrameStream* pThis = (XnFrameStream*)pCookie;
	return pThis->SetExternalBufferPool(gbValue.nDataSize / sizeof(XnGeneralBuffer), (XnGeneralBuffer*)gbValue.pData);
}