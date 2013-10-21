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
