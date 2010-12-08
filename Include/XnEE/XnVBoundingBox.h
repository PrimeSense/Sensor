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






#ifndef _XNV_BOUNDING_BOX_H_
#define _XNV_BOUNDING_BOX_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVPoint.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a 3D bounding box. It doesn't hold the specific points that define it,
 * only the number of these points and the current maximum and minimum values for each
 * axis, which defines the bounding box.
 */

class XN_EE_CORE_API XnVBoundingBox
{
public:
	XnVBoundingBox();
	XnVBoundingBox(const XnVPoint& pt1, const XnVPoint& pt2);

	/**
	* Add a point to the set, and adjust the bounding box accordingly.
	*
	* @param	[in]	ptNewPoint	The point to add
	*/
	void AddPoint(const XnVPoint& ptNewPoint);
	void AddPoint(XnFloat fX, XnFloat fY, XnFloat fZ);

	/**
	* Get the minimum point of the Real World bounding box.
	*/
	inline const XnVPoint& GetMins() const
	{
		return m_ptMins;
	}
	/**
	* Get the maximum point of the Real World bounding box.
	*/
	inline const XnVPoint& GetMaxs() const
	{
		return m_ptMaxs;
	}
	inline XnUInt32 GetSize() const
	{
		return m_nSize;
	}

	XnBool IsPointWithin(const XnVPoint& pt) const;
	/**
	* If maximum of any axis is more than the resolution for it, it's changed to the resolution.
	* As a side effect, if the minimum is negative, it's changed to 0.
	*
	* @param	[in]	nXRes	X-axis maximum value
	* @param	[in]	nYRes	Y-axis maximum value
	* @param	[in]	nZRes	Z-axis maximum value
	*/
	void CutToResolution(XnUInt16 nXRes, XnUInt16 nYRes, XnUInt16 nZRes);
	void Clear();
	inline XnBool IsValid() const
	{
		return (m_nSize > 0);
	}
protected:
	XnUInt32 m_nSize;
	XnVPoint m_ptMins;
	XnVPoint m_ptMaxs;
};


#endif //_XNV_BOUNDING_BOX_H_

