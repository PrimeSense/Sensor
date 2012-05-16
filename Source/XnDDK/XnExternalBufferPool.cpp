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
