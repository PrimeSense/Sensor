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
#include "XnBuffer.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnBuffer::Allocate(XnUInt32 nAllocSize)
{
	Free();
	XN_VALIDATE_ALIGNED_CALLOC(m_pData, XnUChar, nAllocSize, XN_DEFAULT_MEM_ALIGN);
	m_nMaxSize = nAllocSize;
	m_nSize = 0;
	m_bAllocated = TRUE;
	return (XN_STATUS_OK);
}

void XnBuffer::SetExternalBuffer(XnUChar* pBuffer, XnUInt32 nSize)
{
	Free();
	m_pData = pBuffer;
	m_nMaxSize = nSize;
	m_nSize = 0;
	m_bAllocated = FALSE;
}

XnStatus XnBuffer::Write(const XnUChar* pData, XnUInt32 nDataSize)
{
	if (GetFreeSpaceInBuffer() < nDataSize)
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;

	UnsafeWrite(pData, nDataSize);

	return (XN_STATUS_OK);
}
