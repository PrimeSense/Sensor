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
#ifndef _XNV_VIRTUAL_OBJECT_H_
#define _XNV_VIRTUAL_OBJECT_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVObject.h"
#include "XnVDepthMap.h"
#include "XnVBoundingBox.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * XnVVirtualObject is a base class for all user-created objects that are inserted
 * into the scene.
 * Each of these objects must be able to intersect with a XnVDepthMap.
 * The intersection is in Real World coordinations.
 * Currently implemented virtual objects include XnV3DBox and XnVSphere.
 */

class XN_EE_FW_API XnVVirtualObject : public XnVObject
{
public:
	XnVVirtualObject();

	virtual ~XnVVirtualObject() {}

	/**
	* Intersect the virtual object with the depth map.
	* Intersection is done in Real World coordinations.
	* Adds the pixels from the input depth map that are inside or on the virtual object
	* to the output depth map.
	* No pixels are deleted from the output depth map if they already exist.
	*
	* @param	[in]		dmOther		The input depth map on which to run.
	* @param	[in,out]	dmOutput	The output depth map, with only the pixels inside or on the virtual object.
	*/
	virtual XnUInt32 Intersect(const XnVDepthMap& dmOther, XnVDepthMap& dmOutput) = 0;
	/**
	* Is there at least nThreshold pixels in the intersection of the depth map and
	* this virtual object?
	*
	* @param	[in]	dmOther		The input depth map.
	* @param	[in]	nThreshold	The minimal number of pixels to consider intersection.
	*/
	virtual XnBool IsIntersect(const XnVDepthMap& dmOther, XnUInt32 nThreshold) = 0;
	/**
	* What is the size of the intersection between the input depth map and this virtual object?
	*
	* @param	[in]	dmOther	The input depth map.
	*/
	virtual XnUInt32 IntersectionSize(const XnVDepthMap& dmOther) = 0;

protected:
	/**
	* Sets a Projective bounding box.
	* This Projective bounding box will assist in intersections.
	*
	* @param [in]	ptMins		Minimum point
	* @param [in]	ptMaxs		Maximum point
	* @param [in]	drDepth		Depth representation, mainly for the Real World Translator inside.
	* @param [in]	bProjective	If the input isn't Projective (false, which means Real World), the points should be translated.
	*/
	XnStatus SetBoundingBox(const XnVPoint& ptMins, const XnVPoint& ptMaxs, const XnVDepthRepresentation& drDepth, XnBool bProjective);

	XnUInt16 m_nXResWhenBounding, m_nYResWhenBounding;
	XnVBoundingBox m_BoundingBox;
};


#endif //_XNV_VIRTUAL_OBJECT_H_

