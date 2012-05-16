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
#include "XnSimpleBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSimpleBufferPool::XnSimpleBufferPool(XnUInt32 nBufferCount) : 
	m_nBufferCount(nBufferCount)
{}

XnSimpleBufferPool::~XnSimpleBufferPool()
{
	FreeAll(TRUE);
}

XnStatus XnSimpleBufferPool::AllocateBuffers(XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// now allocate new
	for (XnUInt32 i = 0; i < m_nBufferCount; ++i)
	{
		void* pBuffer;
		XN_VALIDATE_ALIGNED_CALLOC(pBuffer, XnUChar, nSize, XN_DEFAULT_MEM_ALIGN);
		nRetVal = AddNewBuffer(pBuffer, nSize);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

void XnSimpleBufferPool::DestroyBuffer(void* pBuffer)
{
	xnOSFreeAligned(pBuffer);
}

