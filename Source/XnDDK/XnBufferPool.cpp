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
#include "XnBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnBufferPool::XnBufferPool(XnUInt32 nBufferCount) :
	m_nBufferCount(nBufferCount),
	m_nBufferSize(0),
	m_hLock(NULL),
	m_dump(XN_DUMP_CLOSED)
{}

XnBufferPool::~XnBufferPool()
{
	XnBufferPool::Free();
}

XnStatus XnBufferPool::Init(XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnDumpInit(&m_dump, "BufferPool", "", "bufferpool_%x.txt", this);

	nRetVal = xnOSCreateCriticalSection(&m_hLock);
	XN_IS_STATUS_OK(nRetVal);

	// allocate buffers
	nRetVal = ChangeBufferSize(nBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnBufferPool::Free()
{
	if (m_hLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hLock);
		m_hLock = NULL;
	}
}

XnStatus XnBufferPool::ChangeBufferSize(XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnDumpWriteString(m_dump, "changing buffer size to %d\n", nBufferSize);

	xnOSEnterCriticalSection(&m_hLock);

	m_nBufferSize = nBufferSize;

	nRetVal = AllocateBuffers();
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSLeaveCriticalSection(&m_hLock);
		return (nRetVal);
	}

	xnOSLeaveCriticalSection(&m_hLock);
	
	return (XN_STATUS_OK);
}

void XnBufferPool::FreeAll(XnBool bForceDestroyOfLockedBuffers)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// free existing buffers
	XnBuffersList::Iterator it = m_AllBuffers.begin();
	while (it != m_AllBuffers.end())
	{
		XnBuffersList::Iterator currIt = it;

		// first advance (we might remove this item)
		++it;

		// now check current
		XnBufferInPool* pBuffer = *currIt;

		// check if item is in free list (or we're forcing deletion)
		if (bForceDestroyOfLockedBuffers || pBuffer->m_nRefCount == 0)
		{
			DestroyBuffer(pBuffer);
			m_AllBuffers.Remove(currIt);
		}
		else
		{
			// we can't free it, cause it's still locked. instead, mark it for deletion
			pBuffer->m_bDestroy = TRUE;
		}
	}

	m_FreeBuffers.Clear();
}

XnStatus XnBufferPool::GetBuffer(XnBuffer** ppBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnOSEnterCriticalSection(&m_hLock);

	XnBuffersList::Iterator it = m_FreeBuffers.begin();
	if (it == m_FreeBuffers.end())
	{
		xnOSLeaveCriticalSection(&m_hLock);
		return XN_STATUS_ALLOC_FAILED;
	}

	XnBufferInPool* pBuffer = *it;

	// remove from list
	nRetVal = m_FreeBuffers.Remove(it);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSLeaveCriticalSection(&m_hLock);
		return XN_STATUS_ALLOC_FAILED;
	}

	pBuffer->m_nRefCount = 1;
	xnDumpWriteString(m_dump, "%u taken from pool\n", pBuffer->m_nID);

	xnOSLeaveCriticalSection(&m_hLock);

	*ppBuffer = pBuffer;
	
	return (XN_STATUS_OK);
}

void XnBufferPool::AddRef(XnBuffer* pBuffer)
{
	if (pBuffer == NULL)
	{
		return;
	}

	xnOSEnterCriticalSection(&m_hLock);
	XnBufferInPool* pBufferInPool = (XnBufferInPool*)pBuffer;
	++pBufferInPool->m_nRefCount;

	xnDumpWriteString(m_dump, "%u add ref (%d)\n", pBufferInPool->m_nID, pBufferInPool->m_nRefCount);

	xnOSLeaveCriticalSection(&m_hLock);
}

void XnBufferPool::DecRef(XnBuffer* pBuffer)
{
	if (pBuffer == NULL)
	{
		return;
	}

	XnBufferInPool* pBufInPool = (XnBufferInPool*)pBuffer;

	xnOSEnterCriticalSection(&m_hLock);

	xnDumpWriteString(m_dump, "%u dec ref (%d)", pBufInPool->m_nID, pBufInPool->m_nRefCount-1);

	if (--pBufInPool->m_nRefCount == 0)
	{
		if (pBufInPool->m_bDestroy)
		{
			// remove it from all buffers pool
			XnBuffersList::ConstIterator it = m_AllBuffers.Find(pBufInPool);
			XN_ASSERT(it != m_AllBuffers.end());
			m_AllBuffers.Remove(it);
			// and free it
			DestroyBuffer(pBufInPool);
			xnDumpWriteString(m_dump, "destroy!\n");
		}
		else
		{
			// return it to free buffers list
			m_FreeBuffers.AddLast(pBufInPool);
			xnDumpWriteString(m_dump, "return to pool!\n");
		}
	}
	else
	{
		xnDumpWriteString(m_dump, "\n");
	}

	xnOSLeaveCriticalSection(&m_hLock);
}
