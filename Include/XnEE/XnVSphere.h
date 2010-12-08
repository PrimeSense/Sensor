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






#ifndef _XNV_SPHERE_H_
#define _XNV_SPHERE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnV3DGeometry.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a specific XnVVirtualObject class, which represents a Real World 3D sphere.
 */

class XN_EE_FW_API XnVSphere : public XnV3DGeometry
{
public:
	XnVSphere();
	XnVSphere(const XnVPoint& ptCenter, XnUInt32 nRadius);

	void SetAttributes(const XnVPoint& ptCenter, XnUInt32 nRadius);

	/**
	* Intersect the box with a depth map, resulting in the intersection depth map.
	*
	* @param	[in]	dmOther		The depth map to check against
	* @param	[out]	dmOutput	The result of the intersection
	*/
	XnUInt32 Intersect(const XnVDepthMap& dmOther, XnVDepthMap& dmOutput);
	/**
	* Check if there is an intersection between the box and the depth map,
	* With a minimum size.
	*
	* @param	[in]	dmOther		The depth map to check against
	* @param	[in]	nThreshold	The minimum number of points in order to conclude an intersection 
	*
	* @return	true if the intersection had at least the minimum number of point.
	*/
	XnBool IsIntersect(const XnVDepthMap& dmOther, XnUInt32 nThreshold);

	XnBool IsIntersect(const XnV3DVector& pt) const;
	/**
	* Get the intersection size
	*
	* @param	[in]	dmOther	The depth map to check against.
	*
	* @return	Number of points in the intersection.
	*/
	XnUInt32 IntersectionSize(const XnVDepthMap& dmOther);

	inline XnBool IsValid()
	{
		return m_bValid;
	}

protected:
	XnBool m_bValid;
	XnUInt32 m_nRadius;
protected:
	XnUInt32 Intersection(const XnVDepthMap& dmOther, XnUInt32 nThreshold, XnVDepthMap* pdmOutput);

};

#endif
