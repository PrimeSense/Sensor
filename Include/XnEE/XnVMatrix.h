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

