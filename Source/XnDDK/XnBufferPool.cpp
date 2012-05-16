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
#include "XnBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnBufferPool::XnBufferPool() :
	m_nBufferSize(0),
	m_nNextBufferID(0),
	m_hLock(NULL),
	m_dump(NULL)
{}

XnBufferPool::~XnBufferPool()
{
	XnBufferPool::Free();
}

XnStatus XnBufferPool::Init(XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	m_dump = xnDumpFileOpen("BufferPool", "bufferpool_%x.txt", this);

	nRetVal = xnOSCreateCriticalSection(&m_hLock);
	XN_IS_STATUS_OK(nRetVal);

	// allocate buffers
	xnDumpFileWriteString(m_dump, "Initializing with size %u\n", nBufferSize);
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

	xnDumpFileWriteString(m_dump, "Changing buffer size to %d\n", nBufferSize);

	xnOSEnterCriticalSection(&m_hLock);

	m_nBufferSize = nBufferSize;

	// first free old ones
	FreeAll(FALSE);

	nRetVal = AllocateBuffers(nBufferSize);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSLeaveCriticalSection(&m_hLock);
		return (nRetVal);
	}

	xnDumpFileWriteString(m_dump, "Buffers were allocated\n");

	xnOSLeaveCriticalSection(&m_hLock);
	
	return (XN_STATUS_OK);
}

void XnBufferPool::FreeAll(XnBool bForceDestroyOfLockedBuffers)
{
	// free existing buffers
	xnDumpFileWriteString(m_dump, "freeing existing buffers...\n");

	XnBuffersList::Iterator it = m_AllBuffers.Begin();
	while (it != m_AllBuffers.End())
	{
		XnBuffersList::Iterator currIt = it;

		// first advance (we might remove this item)
		++it;

		// now check current
		XnBufferInPool* pBuffer = *currIt;

		// check if item is in free list (or we're forcing deletion)
		if (bForceDestroyOfLockedBuffers || pBuffer->m_nRefCount == 0)
		{
			xnDumpFileWriteString(m_dump, "\tdestroying buffer %u\n", pBuffer->m_nID);
			DestroyBuffer((void*)pBuffer->GetData());
			XN_DELETE(pBuffer);
			m_AllBuffers.Remove(currIt);
		}
		else
		{
			// we can't free it, cause it's still locked. instead, mark it for deletion
			xnDumpFileWriteString(m_dump, "\tBuffer %u can't be destroyed right now (locked). Just mark it for destruction.\n", pBuffer->m_nID);
			pBuffer->m_bDestroy = TRUE;
		}
	}

	m_FreeBuffers.Clear();

	xnDumpFileWriteString(m_dump, "Buffers were freed\n");
}

XnStatus XnBufferPool::AddNewBuffer(void* pBuffer, XnUInt32 nSize)
{
	XnBufferInPool* pBufferInPool;
	XN_VALIDATE_NEW(pBufferInPool, XnBufferInPool);

	xnOSEnterCriticalSection(&m_hLock);

	pBufferInPool->m_nID = m_nNextBufferID++;
	pBufferInPool->SetExternalBuffer((XnUChar*)pBuffer, nSize);

	xnDumpFileWriteString(Dump(), "\tAdd new buffer %u with size %u at 0x%p\n", pBufferInPool->m_nID, nSize, pBuffer);

	// add it to free list
	m_AllBuffers.AddLast(pBufferInPool);
	m_FreeBuffers.AddLast(pBufferInPool);

	xnOSLeaveCriticalSection(&m_hLock);

	return XN_STATUS_OK;
}

XnStatus XnBufferPool::GetBuffer(XnBuffer** ppBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnOSEnterCriticalSection(&m_hLock);

	XnBuffersList::Iterator it = m_FreeBuffers.Begin();
	if (it == m_FreeBuffers.End())
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
	xnDumpFileWriteString(m_dump, "%u taken from pool\n", pBuffer->m_nID);

	xnOSLeaveCriticalSection(&m_hLock);

	*ppBuffer = pBuffer;
	
	return (XN_STATUS_OK);
}

void XnBufferPool::AddRef(XnBuffer* pBuffer)
{
	if (pBuffer == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}

	xnOSEnterCriticalSection(&m_hLock);
	XnBufferInPool* pBufferInPool = (XnBufferInPool*)pBuffer;
	++pBufferInPool->m_nRefCount;

	xnDumpFileWriteString(m_dump, "%u add ref (%d)\n", pBufferInPool->m_nID, pBufferInPool->m_nRefCount);

	xnOSLeaveCriticalSection(&m_hLock);
}

void XnBufferPool::DecRef(XnBuffer* pBuffer)
{
	if (pBuffer == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}

	XnBufferInPool* pBufInPool = (XnBufferInPool*)pBuffer;

	xnOSEnterCriticalSection(&m_hLock);

	xnDumpFileWriteString(m_dump, "%u dec ref (%d)", pBufInPool->m_nID, pBufInPool->m_nRefCount-1);

	if (--pBufInPool->m_nRefCount == 0)
	{
		if (pBufInPool->m_bDestroy)
		{
			// remove it from all buffers pool
			XnBuffersList::ConstIterator it = m_AllBuffers.Find(pBufInPool);
			XN_ASSERT(it != m_AllBuffers.End());
			m_AllBuffers.Remove(it);
			// and free it
			DestroyBuffer((void*)pBufInPool->GetData());
			xnDumpFileWriteString(m_dump, "destroy!\n");
		}
		else
		{
			// return it to free buffers list
			m_FreeBuffers.AddLast(pBufInPool);
			xnDumpFileWriteString(m_dump, "return to pool!\n");
		}
	}
	else
	{
		xnDumpFileWriteString(m_dump, "\n");
	}

	xnOSLeaveCriticalSection(&m_hLock);
}
