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
#include "XnExternalBufferPool.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnExternalBufferPool::XnExternalBufferPool()
{
}

XnExternalBufferPool::~XnExternalBufferPool()
{
	FreeAll(TRUE);
}

XnStatus XnExternalBufferPool::SetBuffers(XnUInt32 nCount, const XnGeneralBuffer* aBuffers)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(aBuffers);

	// make sure we have at least 3 buffers (user, stable and working)
	if (nCount < 3)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_DDK, "ExternalBufferPool: at least 3 buffers are required!");
	}

	m_buffers.Clear();

	// add buffers to the list
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		nRetVal = m_buffers.AddLast(aBuffers[i]);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnExternalBufferPool::AllocateBuffers(XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// make sure all buffers are in the correct size
	for (XnUInt32 i = 0; i < m_buffers.GetSize(); ++i)
	{
		if (m_buffers[i].nDataSize < nSize)
		{
			return XN_STATUS_ALLOC_FAILED;
		}
	}

	// now "allocate" them
	for (XnUInt32 i = 0; i < m_buffers.GetSize(); ++i)
	{
		nRetVal = AddNewBuffer(m_buffers[i].pData, m_buffers[i].nDataSize);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

void XnExternalBufferPool::DestroyBuffer(void* /*pBuffer*/)
{
}
