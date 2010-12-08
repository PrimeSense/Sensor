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





#ifndef __XN_BUFFER_H__
#define __XN_BUFFER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <XnCore.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
* Holds a buffer of data, and provides some common methods for it.
*/
class XN_CORE_CPP_API XnBuffer
{
public:
	XnBuffer() : m_pData(NULL), m_nMaxSize(0), m_nSize(0), m_bAllocated(FALSE) {}
	~XnBuffer() { Free(); }

	/*
	* Allocates a buffer (aligned to default).
	*
	* @param	size		[in]	The number of bytes to allocate.
	*/
	XnStatus Allocate(XnUInt32 nAllocSize);

	/**
	 * Sets an external buffer (instead of allocating)
	 *
	 * @param	pBuffer		[in]	the buffer to use.
	 * @param	nSize		[in]	Buffer size.
	 */
	void SetExternalBuffer(XnUChar* pBuffer, XnUInt32 nSize);

	/*
	* Writes data to the buffer.
	*
	* @param	data		[in]	a pointer to the data to copy from.
	* @param	size		[in]	The number of bytes to copy.
	*/
	inline void UnsafeWrite(const XnUChar* pData, XnUInt32 nDataSize)
	{
		xnOSMemCopy(m_pData + m_nSize, pData, nDataSize);
		m_nSize += nDataSize;
	}

	/*
	* Writes data to the buffer, checking for free space
	*
	* @param	data		[in]	a pointer to the data to copy from.
	* @param	size		[in]	The number of bytes to copy.
	*/
	XnStatus Write(const XnUChar* pData, XnUInt32 nDataSize);

	/*
	* Copies buffer data to another location. 
	*
	* @param	dest	[in]	Location to write to.
	*/
	inline void UnsafeCopy(void* pDest)
	{
		xnOSMemCopy(pDest, m_pData, m_nSize);
	}

	/*
	* Empties the buffer.
	*/
	inline void Reset()
	{
		m_nSize = 0;
	}

	inline const XnUChar* GetData()
	{
		return m_pData;
	}

	inline XnUInt32 GetSize()
	{
		return m_nSize;
	}

	inline XnUInt32 GetMaxSize()
	{
		return m_nMaxSize;
	}

	/*
	* Frees an allocated buffer.
	*/
	inline void Free()
	{
		if (m_bAllocated)
		{
			XN_ALIGNED_FREE_AND_NULL(m_pData);
			m_bAllocated = FALSE;
		}
	}

	/*
	* Gets the free space in the buffer.
	*/
	inline XnUInt32 GetFreeSpaceInBuffer()
	{
		return XN_MAX(0, (XnInt32)(m_nMaxSize - m_nSize));
	}

	/*
	* Gets the free space in the buffer.
	*/
	inline XnUChar* GetUnsafeWritePointer()
	{
		return m_pData + m_nSize;
	}

	/*
	* Updates the size of the buffer
	*/
	inline void UnsafeUpdateSize(XnUInt32 nWrittenBytes)
	{
		m_nSize += nWrittenBytes;
	}

private:
	XnUChar* m_pData;
	XnUInt32 m_nSize;
	XnUInt32 m_nMaxSize;
	XnBool m_bAllocated;
};

/*
* Allocates a buffer (aligned to default), and validates that allocation succeeded.
*
* @param	buf			[in]	The buffer to allocate.
* @param	size		[in]	The number of bytes to allocate.
*/
#define XN_VALIDATE_BUFFER_ALLOCATE(buf, size)				\
	{														\
		XnStatus rc = buf.Allocate(size);					\
		XN_IS_STATUS_OK(rc);								\
	}

#endif //__XN_BUFFER_H__
