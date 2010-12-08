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

