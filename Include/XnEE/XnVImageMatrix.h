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






#ifndef _XNV_IMAGE_MATRIX_H_
#define _XNV_IMAGE_MATRIX_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVMatrix.h"
#include <XnStreamFormats.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This object is a specific XnVMatrix, which holds XnUInt8, and should be used
 * for holding images. It holds 3 entries for each pixel - for RGB.
 * It is used by XnVImageMap.
 */

class XN_EE_CORE_API XnVImageMatrix : public XnVMatrix
{
public:
	XnVImageMatrix(XnUInt8* pBuffer, XnUInt16 nXRes, XnUInt16 nYRes, XnUInt16 nBPP) :
	  XnVMatrix(pBuffer, nBPP * nXRes, nYRes, sizeof(XnUInt8))
	 {}
	XnVImageMatrix(XnUInt16 nXRes, XnUInt16 nYRes, XnUInt16 nBPP) :
	  XnVMatrix(nBPP * nXRes, nYRes, sizeof(XnUInt8))
	{}

	operator XnUInt8*()
	{
		return (XnUInt8*)Data();
	}

	operator const XnUInt8*() const
	{
		return (const XnUInt8*)Data();
	}

	inline const XnUInt8& operator[](XnInt32 nIndex) const
	{
		return ((const XnUInt8*)Data())[nIndex];
	}

	inline XnUInt8& operator[](XnInt32 nIndex)
	{
		return ((XnUInt8*)Data())[nIndex];
	}

	inline void Clear()
	{
		xnOSMemSet(Data(), 0, m_nXRes*m_nYRes * sizeof(XnDepthPixel));
	}
};

#endif //_XNV_IMAGE_MATRIX_H_

