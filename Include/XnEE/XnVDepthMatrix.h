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

