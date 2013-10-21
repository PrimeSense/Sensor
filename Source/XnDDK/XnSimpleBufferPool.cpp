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

