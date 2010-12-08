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

