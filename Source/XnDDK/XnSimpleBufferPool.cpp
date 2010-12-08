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
#include "XnSimpleBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSimpleBufferPool::XnSimpleBufferPool(XnUInt32 nBufferCount) : XnBufferPool(nBufferCount)
{}

XnSimpleBufferPool::~XnSimpleBufferPool()
{
	FreeAll(TRUE);
}

XnStatus XnSimpleBufferPool::AllocateBuffers()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// first free old ones
	FreeAll(FALSE);

	// now allocate new
	for (XnUInt32 i = 0; i < m_nBufferCount; ++i)
	{
		XnBufferInPool* pBufferInPool;
		XN_VALIDATE_NEW(pBufferInPool, XnBufferInPool);
		nRetVal = pBufferInPool->Allocate(m_nBufferSize);
		XN_IS_STATUS_OK(nRetVal);
		pBufferInPool->m_nID = i;

		// add it to all list
		m_AllBuffers.AddLast(pBufferInPool);
		// and to free list
		m_FreeBuffers.AddLast(pBufferInPool);
	}
	
	return (XN_STATUS_OK);
}

void XnSimpleBufferPool::DestroyBuffer(XnBufferInPool* pBuffer)
{
	XN_DELETE(pBuffer);
}

