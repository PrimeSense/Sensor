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
#include "XnFrameBufferManager.h"
#include <XnLog.h>
#include <XnDDK.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnFrameBufferManager::XnFrameBufferManager(XnBufferPool* pBufferPool) :
	m_pBufferPool(pBufferPool),
	m_pWorkingBuffer(NULL),
	m_pStableBuffer(NULL),
	m_nStableFrameID(0),
	m_nStableTimestamp(0),
	m_hLock(NULL)
{
}

XnFrameBufferManager::~XnFrameBufferManager()
{
	Free();
}

XnStatus XnFrameBufferManager::Init(XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnOSCreateCriticalSection(&m_hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pBufferPool->Init(nBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = Reallocate(nBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnFrameBufferManager::Free()
{
	if (m_hLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hLock);
		m_hLock = NULL;
	}
}

XnStatus XnFrameBufferManager::Reallocate(XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnOSEnterCriticalSection(&m_hLock);

	nRetVal = m_pBufferPool->ChangeBufferSize(nBufferSize);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSLeaveCriticalSection(&m_hLock);
		return (nRetVal);
	}

	// release current ones
	if (m_pWorkingBuffer != NULL)
	{
		m_pBufferPool->DecRef(m_pWorkingBuffer);
	}

	if (m_pStableBuffer != NULL)
	{
		m_pBufferPool->DecRef(m_pStableBuffer);
	}

	// and take one
	if (nBufferSize == 0)
	{
		m_pWorkingBuffer = NULL;
		m_pStableBuffer = NULL;
	}
	else
	{
		// take working buffer
		nRetVal = m_pBufferPool->GetBuffer(&m_pWorkingBuffer);
		XN_IS_STATUS_OK(nRetVal);
	}

	xnOSLeaveCriticalSection(&m_hLock);

	return (XN_STATUS_OK);
}

void XnFrameBufferManager::MarkWriteBufferAsStable(XnUInt64 nTimestamp, XnUInt32* pnFrameID)
{
	xnOSEnterCriticalSection(&m_hLock);

	// lock buffer pool (for rollback option)
	m_pBufferPool->Lock();

	XnBuffer* pPrevStable = m_pStableBuffer;

	// release previous stable
	if (m_pStableBuffer != NULL)
	{
		m_pBufferPool->DecRef(m_pStableBuffer);
	}

	// mark working as stable
	m_nStableFrameID++;
	m_nStableTimestamp = nTimestamp;
	*pnFrameID = m_nStableFrameID;

	m_pStableBuffer = m_pWorkingBuffer; // no need to add ref, working buffer will be replaced in a moment

	// take a new working buffer
	XnStatus nRetVal = m_pBufferPool->GetBuffer(&m_pWorkingBuffer);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_DDK, "Failed to get new working buffer!");
		// we'll return back to our old working one
		m_pWorkingBuffer->Reset();

		m_pStableBuffer = pPrevStable;
		m_pBufferPool->AddRef(m_pStableBuffer);
		m_pBufferPool->Unlock();
		return;
	}

	m_pBufferPool->Unlock();
	xnOSLeaveCriticalSection(&m_hLock);

	// reset new working
	m_pWorkingBuffer->Reset();

	// notify stream that new data is available
	m_NewFrameEvent.Raise(this, m_nStableTimestamp);
}

void XnFrameBufferManager::ReadLastStableBuffer(XnBuffer** ppBuffer, XnUInt64* pnTimestamp, XnUInt32* pnFrameID)
{
	xnOSEnterCriticalSection(&m_hLock);

	// take a pointer to stable one
	*ppBuffer = m_pStableBuffer;
	// and add ref to it (so it wouldn't be deleted)
	if (m_pStableBuffer != NULL)
	{
		m_pBufferPool->AddRef(m_pStableBuffer);
	}

	// take props
	*pnTimestamp = m_nStableTimestamp;
	*pnFrameID = m_nStableFrameID;

	xnOSLeaveCriticalSection(&m_hLock);
}

XnStatus XnFrameBufferManager::CopyLastStableBuffer(void* pDest, XnUInt32 nDestSize, XnUInt32* pnWritten)
{
	// do this inside a lock (so we won't erase memory during copy)
	xnOSEnterCriticalSection(&m_hLock);

	// check size
	if (nDestSize < m_pStableBuffer->GetSize())
	{
		xnOSLeaveCriticalSection(&m_hLock);
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
	}

	// copy
	m_pStableBuffer->UnsafeCopy(pDest);
	*pnWritten = m_pStableBuffer->GetSize();

	xnOSLeaveCriticalSection(&m_hLock);

	return XN_STATUS_OK;
}


