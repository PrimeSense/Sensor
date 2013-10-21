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
#ifndef _XNV_CONTAINER_H_
#define _XNV_CONTAINER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCore.h>
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a general container.
 * It can either be initialized by the size it should be in, or by a buffer already
 * allocated outside.
 * All functionalities that change the size of the container are disabled if
 * the buffer was supplied by the user.
 */

class XN_EE_CORE_API XnVContainer
{
public:
	/**
	* Create a new container, initializing the buffer it will use.
	*
	* @param	[in]	pBuffer		The buffer in which to implement the container
	* @param	[in]	nCapacity	Maximum elements in the container
	* @param	[in]	nElSize		The size of each element
	*/
	XnVContainer(void* pBuffer, XnUInt32 nCapacity, XnUInt16 nElSize);
	/**
	* Create a new container, using a user-supplied buffer.
	*
	* @param	[in]	nCapacity	Maximum elements in the container
	* @param	[in]	nElSize		The size of each element
	*/
	XnVContainer(XnUInt32 nCapacity, XnUInt16 nElSize);
	virtual ~XnVContainer();

	/**
	* Change the buffer on which the container is implemented.
	*
	* @param	[in]	pBuffer		The new buffer
	* @param	[in]	nCapacity	The buffer's capacity
	*/
	XnStatus ReAdjust(void* pBuffer, XnUInt32 nCapacity);
	/**
	* Change the buffer on which the container is implemented.
	* This function works only on containers which allocated their own internal buffer.
	*
	* @param	[in]	nCapacity	The new buffer's capacity
	*/
	XnStatus ReAdjust(XnUInt32 nCapacity);

	XnStatus Add(void* pData); // copies size of nSize!
	XnStatus SetAt(XnUInt32 nIndex, void* pData);
	inline void* GetAt(XnUInt32 nPosition) const
	{
		return (XnChar*)m_pBuffer + nPosition * m_nElSize;
	}
	inline void* GetNext(void* pPrevious) const
	{
		return (XnChar*)pPrevious + m_nElSize;
	}
	inline void* GetPrevious(void* pOriginal) const
	{
		return (XnChar*)pOriginal - m_nElSize;
	}

	inline XnUInt32 GetSize() const
	{
		return m_nElCount;
	}
	inline XnUInt32 GetCapacity() const
	{
		return m_nCapacity;
	}

	inline void* Data()
	{
		return m_pBuffer;
	}

	inline const void* Data() const
	{
		return m_pBuffer;
	}

	inline XnBool IsExternal() const
	{
		return !m_bAllocated;
	}
	inline void Clear()
	{
		m_nElCount = 0;
	}

	inline XnUInt16 GetElSize() const
	{
		return m_nElSize;
	}
protected:
	XnStatus Resize(XnUInt32 nNewCapacity);

	XnUInt16 m_nElSize;
	XnUInt32 m_nCapacity;
	XnUInt32 m_nElCount;
	void* m_pBuffer;

	XnBool m_bAllocated;
};


#endif //_XNV_CONTAINER_H_

