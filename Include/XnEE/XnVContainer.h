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

