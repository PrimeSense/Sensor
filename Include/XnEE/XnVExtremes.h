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
#ifndef _XNV_EXTREMES_H_
#define _XNV_EXTREMES_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVPoint.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This object is generally filled by XnVHumanStretch.
 * This object holds 16 XnVPoint instances: 
 * For each quadrant (x,y quadrant, defined by a pivot XnVPoint to XnVHumanStretch)
 * it holds four points - the extreme projective point in each of x and y
 * (extreme being either minimum or maximum values, depending on the quadrant)
 * and 2 points for z axis extremes - both the minimum z and maximum z.
 */

class XN_EE_FW_API XnVExtremes
{
public:
	XnVExtremes()
	{}

	class XN_EE_FW_API XnVExtremeCandidate 
	{
	public:
		const XnVPoint& GetX() const;
		XnVPoint& GetX();
		const XnVPoint& GetY() const;
		XnVPoint& GetY();
		const XnVPoint& GetZMin() const;
		XnVPoint& GetZMin();
		const XnVPoint& GetZMax() const;
		XnVPoint& GetZMax();

		void SetX(XnVPoint& ptX);
		void SetY(XnVPoint& ptY);
		void SetZMin(XnVPoint& ptZMin);
		void SetZMax(XnVPoint& ptZMax);
	protected:
		XnVPoint m_ptX;
		XnVPoint m_ptY;
		XnVPoint m_ptZMax;
		XnVPoint m_ptZMin;
	};

	const XnVExtremeCandidate& GetTopRight() const;
	XnVExtremeCandidate& GetTopRight();
	const XnVExtremeCandidate& GetTopLeft() const;
	XnVExtremeCandidate& GetTopLeft();
	const XnVExtremeCandidate& GetBottomRight() const;
	XnVExtremeCandidate& GetBottomRight();
	const XnVExtremeCandidate& GetBottomLeft() const;
	XnVExtremeCandidate& GetBottomLeft();

protected:
	XnVExtremeCandidate m_TopRight, m_TopLeft, m_BottomRight, m_BottomLeft;
};


#endif //_XNV_EXTREMES_H_

