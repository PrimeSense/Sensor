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






#ifndef _XNV_MATRIX_H_
#define _XNV_MATRIX_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVContainer.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a general 2-Dimension Container, and is implemented over XnVContainer.
 * It can either be initialized by the size it should be in, or by a buffer already
 * allocated outside.
 * All functionalities that change the size of the matrix are disabled if
 * the buffer was supplied by the user.
 */

class XN_EE_CORE_API XnVMatrix
{
public:
	/**
	* Create a new matrix, initializing the buffer it will use.
	*
	* @param	[in]	pBuffer		The buffer in which to implement the container
	* @param	[in]	nXRes		The X-axis resolution of the matrix
	* @param	[in]	nYRes		The Y-axis resolution of the matrix
	* @param	[in]	nElSize		The size of each element
	*/
	XnVMatrix(void* pBuffer, XnUInt16 nXRes, XnUInt16 nYRes, XnUInt16 nElSize);
	/**
	* Create a new matrix, using a user-supplied buffer.
	*
	* @param	[in]	nXRes		The X-axis resolution of the matrix
	* @param	[in]	nYRes		The Y-axis resolution of the matrix
	* @param	[in]	nElSize		The size of each element
	*/
	XnVMatrix(XnUInt16 nXRes, XnUInt16 nYRes, XnUInt16 nElSize);
	virtual ~XnVMatrix()
	{}

	inline XnStatus ReAdjust(XnUInt16 nXRes, XnUInt16 nYRes)
	{
		XnStatus rc = m_vData.ReAdjust(nXRes * nYRes);
		if (rc != XN_STATUS_OK)
			return rc;
		m_nXRes = nXRes;
		m_nYRes = nYRes;
		return rc;
	}
	inline XnStatus ReAdjust(void* pData, XnUInt16 nXRes, XnUInt16 nYRes)
	{
		m_nXRes = nXRes;
		m_nYRes = nYRes;
		return m_vData.ReAdjust(pData, nXRes*nYRes);
	}

	inline void* GetAt(XnUInt16 nX, XnUInt16 nY)
	{
		return m_vData.GetAt(nY * m_nXRes + nX);
	}

	inline void* GetAt(XnUInt16 nX, XnUInt16 nY) const
	{
		return m_vData.GetAt(nY * m_nXRes + nX);
	}

	inline void* GetAt(XnUInt32 nIndex)
	{
		return m_vData.GetAt(nIndex);
	}

	inline void* GetAt(XnUInt32 nIndex) const
	{
		return m_vData.GetAt(nIndex);
	}

	inline void* GetNext(void* pPrevious) const
	{
		return m_vData.GetNext(pPrevious);
	}

	inline XnUInt16 GetXRes() const
	{
		return m_nXRes;
	}

	inline XnUInt16 GetYRes() const
	{
		return m_nYRes;
	}

	inline void* Data()
	{
		return m_vData.Data();
	}

	inline const void* Data() const
	{
		return m_vData.Data();
	}

	virtual void Clear();

	virtual XnStatus Clone(XnVMatrix& other) const;

//protected:
	XnUInt16 m_nXRes, m_nYRes;
	XnVContainer m_vData;

private:
	XnVMatrix& operator=(const XnVMatrix& other);
};

#endif //_XNV_MATRIX_H_

