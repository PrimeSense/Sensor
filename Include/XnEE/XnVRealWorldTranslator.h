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
#ifndef _XNV_REAL_WORLD_TRANSLATOR_H_
#define _XNV_REAL_WORLD_TRANSLATOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVPoint.h"
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/** 
 * This algorithm translates points from projective to real world representation and vice versa.
 */
class XN_EE_CORE_API XnVRealWorldTranslator
{
public:
	// Constructors
	XnVRealWorldTranslator(XnDouble fZeroPlaneDistance, XnDouble fPixelSizeAtZeroPlane,
							XnUInt16 nXRes, XnUInt16 nYRes,
							XnUInt16 nXFullRes, XnUInt16 nYFullRes, XnUInt16 nXOffset, XnUInt16 nYOffset);
	XnVRealWorldTranslator(XnDouble fZeroPlaneDistance, XnDouble fPixelSizeAtZeroPlane,
							XnUInt16 nXRes, XnUInt16 nYRes);

	// Initializers (or re-initializers)
	/**
	* Update internal values, relevant for translation purposes.
	*
	* @param	[in]	fZeroPlaneDistance	The distance (in cm) where the reference was taken.
	* @param	[in]	fPixelSizeAtZeroPlane	The size of a pixel at the distance the reference was taken.
	* @param	[in]	nXRes			The maximum projective x value of the depth map.
	* @param	[in]	nYRes			The maximum projective y value of the depth map.
	* @param	[in]	nXFullRes		Future Use - the x-resolution of the depth map that created this one as a submap.
	* @param	[in]	nYFullRes		Future Use - the y-resolution of the depth map that created this one as a submap.
	* @param	[in]	nXOffset		Future Use - the x-offset of this depth map in the one that created it as a submap.
	* @param	[in]	nYOffset		Future Use - the y-offset of this depth map in the one that created it as a submap.
	*/
	XnStatus Update(XnDouble fZeroPlaneDistance, XnDouble fPixelSizeAtZeroPlane,
						XnUInt16 nXRes, XnUInt16 nYRes,
						XnUInt16 nXFullRes, XnUInt16 nYFullRes, XnUInt16 nXOffset, XnUInt16 nYOffset);
	/**
	* Update internal values, relevant for translation purposes.
	*
	* @param	[in]	fZeroPlaneDistance	The distance (in cm) where the reference was taken.
	* @param	[in]	fPixelSizeAtZeroPlane	The size of a pixel at the distance the reference was taken.
	* @param	[in]	nXRes			The maximum projective x value of the depth map.
	* @param	[in]	nYRes			The maximum projective y value of the depth map.
	*/
	XnStatus Update(XnDouble fZeroPlaneDistance, XnDouble fPixelSizeAtZeroPlane,
						XnUInt16 nXRes, XnUInt16 nYRes);

	/**
	* Translate point from Real World coordinates to Projective coordinates, rounding the result.
	*
	* @param	[in,out]	ptPoint	A Real World point on input, Projective on output.
	*/
	void RW2Projective(XnVPoint& ptPoint) const;

	/**
	* Translate point from Real World coordinates to Projective coordinates.
	*
	* @param	[in,out]	ptPoint	A Real World point on input, Projective on output.
	*/
	void RW2ProjectiveFloat(XnVPoint& ptPoint) const;

	/**
	* Translate point from Projective coordinates to Real World coordinates.
	*
	* @param	[in,out]	ptPoint	A Projective point on input, Real World on output.
	*/
	void Projective2RW(XnVPoint& ptPoint) const;

protected:
	XnStatus Init(XnDouble fZeroPlaneDistance, XnDouble fPixelSizeAtZeroPlane,
				XnUInt16 nXRes, XnUInt16 nYRes,
				XnUInt16 nXFullRes, XnUInt16 nYFullRes, XnUInt16 nXOffset, XnUInt16 nYOffset);

	XnDouble m_fZeroPlaneDistance;
	XnDouble m_fPixelSizeAtZeroPlane;

	XnDouble m_fSourceToDepthPixelRatio;

	XnUInt16 m_nXRes;
	XnUInt16 m_nYRes;

	XnUInt16 m_nXFullRes, m_nYFullRes;

	XnUInt16 m_nXOffset, m_nYOffset;
};


#endif //_XNV_REAL_WORLD_TRANSLATOR_H_

