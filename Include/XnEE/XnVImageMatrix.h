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

