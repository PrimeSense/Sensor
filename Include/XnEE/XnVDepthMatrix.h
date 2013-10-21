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
#ifndef _XNV_DEPTH_MATRIX_H_
#define _XNV_DEPTH_MATRIX_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVMatrix.h"

/**
 * This object is a specific XnVMatrix, which holds XnDepthPixel, and should be used
 * for holding depths.
 * It is used by XnVDepthMap.
 */

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XN_EE_CORE_API XnVDepthMatrix : public XnVMatrix
{
public:
	XnVDepthMatrix(XnDepthPixel* pBuffer, XnUInt16 nXRes, XnUInt16 nYRes) :
	  XnVMatrix(pBuffer, nXRes, nYRes, sizeof(XnDepthPixel))
	 {}
	XnVDepthMatrix(XnUInt16 nXRes, XnUInt16 nYRes) :
	  XnVMatrix(nXRes, nYRes, sizeof(XnDepthPixel))
	{}

	operator XnDepthPixel*()
	{
		return (XnDepthPixel*)Data();
	}

	operator const XnDepthPixel*() const
	{
		return (const XnDepthPixel*)Data();
	}

	inline const XnDepthPixel& operator[](XnInt32 nIndex) const
	{
		return ((XnDepthPixel*)Data())[nIndex];
	}

	inline XnDepthPixel& operator[](XnInt32 nIndex)
	{
		return ((XnDepthPixel*)Data())[nIndex];
	}

	inline const XnDepthPixel& operator()(XnInt32 nX, XnInt32 nY) const
	{
		return (*this)[nY*m_nXRes+nX];
	}

	inline XnDepthPixel& operator()(XnInt32 nX, XnInt32 nY)
	{
		return (*this)[nY*m_nXRes+nX];
	}
};

#endif //_XNV_DEPTH_MATRIX_H_

