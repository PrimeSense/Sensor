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

