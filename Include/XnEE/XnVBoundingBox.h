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

