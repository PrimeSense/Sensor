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
#include "XnSharedMemoryBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnSharedMemoryBufferPool::XnSharedMemoryBufferPool(XnUInt32 nBufferCount, const XnChar* strDeviceName, const XnChar* strStreamName, XnUInt32 nMaxBufferSize) :
	XnBufferPool(nBufferCount),
	m_nMaxBufferSize(nMaxBufferSize),
	m_hSharedMemory(NULL),
	m_pSharedMemoryAddress(NULL)
{
	// to make the name unique, we'll add process ID
	XN_PROCESS_ID procID;
	xnOSGetCurrentProcessID(&procID);
	sprintf(m_strName, "%u_%s_%s", procID, strDeviceName, strStreamName);
}

XnSharedMemoryBufferPool::~XnSharedMemoryBufferPool()
{
	XnSharedMemoryBufferPool::Free();
}

XnStatus XnSharedMemoryBufferPool::AllocateBuffers()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_nBufferSize > m_nMaxBufferSize)
	{
		return XN_STATUS_ALLOC_FAILED;
	}

	if (m_pSharedMemoryAddress != NULL)
	{
		// already allocated. nothing to do here
		return (XN_STATUS_OK);
	}

	// first time. allocate shared memory
	XnUInt32 nTotalSize = m_nMaxBufferSize * m_nBufferCount;
	nRetVal = xnOSCreateSharedMemory(m_strName, nTotalSize, XN_OS_FILE_READ | XN_OS_FILE_WRITE, &m_hSharedMemory);
	XN_IS_STATUS_OK(nRetVal);

	void* pAddress;
	nRetVal = xnOSSharedMemoryGetAddress(m_hSharedMemory, &pAddress);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSCloseSharedMemory(m_hSharedMemory);
		m_hSharedMemory = NULL;
		return (nRetVal);
	}

	m_pSharedMemoryAddress = (XnUChar*)pAddress;

	// now allocate buffers
	for (XnUInt32 i = 0; i < m_nBufferCount; ++i)
	{
		XnBufferInPool* pBuffer = XN_NEW(XnBufferInPool);
		if (pBuffer == NULL)
		{
			Free();
			return (XN_STATUS_ALLOC_FAILED);
		}

		pBuffer->m_nID = i;

		pBuffer->SetExternalBuffer(m_pSharedMemoryAddress + i*m_nMaxBufferSize, m_nMaxBufferSize);

		xnDumpWriteString(Dump(), "Allocated buffer %u with size %u\n", i, m_nMaxBufferSize);

		// add it to free list
		m_AllBuffers.AddLast(pBuffer);
		m_FreeBuffers.AddLast(pBuffer);
	}

	return (XN_STATUS_OK);
}

void XnSharedMemoryBufferPool::DestroyBuffer(XnBufferInPool* pBuffer)
{
	// simply add it back to free list
	m_FreeBuffers.AddLast(pBuffer);
}

void XnSharedMemoryBufferPool::Free()
{
	if (m_hSharedMemory != NULL)
	{
		xnOSCloseSharedMemory(m_hSharedMemory);
		m_hSharedMemory = NULL;
	}

	for (XnBuffersList::Iterator it = m_AllBuffers.begin(); it != m_AllBuffers.end(); ++it)
	{
		XnBufferInPool* pBuffer = *it;
		XN_DELETE(pBuffer);
	}

	m_AllBuffers.Clear();
	m_FreeBuffers.Clear();
	XnBufferPool::Free();
}
