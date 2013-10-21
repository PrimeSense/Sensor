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
#ifndef _XNV_LABEL_MATRIX_H_
#define _XNV_LABEL_MATRIX_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVObject.h"
#include "XnVMatrix.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * XnVLabelMatrix is a matrix of XnVObject::XnVLabel.
 * It is usually coupled with a XnVDepthMap object of the same resolution,
 * and is filled by XnVConnectedComponentDetector.
 * A label at a certain (x,y) position (or at [index] position) 
 * is the label of the pixel at the same position in the matching XnVDepthMap.
 */
class XN_EE_FW_API XnVLabelMatrix : public XnVMatrix
{
public:
	XnVLabelMatrix(XnDepthPixel* pBuffer, XnUInt16 nXRes, XnUInt16 nYRes) :
	  XnVMatrix(pBuffer, nXRes, nYRes, sizeof(XnVObject::XnVLabel))
	 {}
	XnVLabelMatrix(XnUInt16 nXRes, XnUInt16 nYRes) :
	  XnVMatrix(nXRes, nYRes, sizeof(XnVObject::XnVLabel))
	 {}
	XnVLabelMatrix() :
	  XnVMatrix(0, 0, sizeof(XnVObject::XnVLabel))
	 {}

	inline XnVObject::XnVLabel& operator[](XnInt32 nIndex)
	{
		return ((XnVObject::XnVLabel*)Data())[nIndex];
	}

	inline const XnVObject::XnVLabel& operator[](XnInt32 nIndex) const
	{
		return ((XnVObject::XnVLabel*)Data())[nIndex];
	}

	inline const XnVObject::XnVLabel& operator()(XnInt32 nX, XnInt32 nY) const
	{
		return (*this)[nY*m_nXRes+nX];
	}

	inline XnVObject::XnVLabel& operator()(XnInt32 nX, XnInt32 nY)
	{
		return (*this)[nY*m_nXRes+nX];
	}

	inline void Clear()
	{
		xnOSMemSet(Data(), 0, m_nXRes*m_nYRes*sizeof(XnVObject::XnVLabel));
	}
};


#endif //_XNV_LABEL_MATRIX_H_

