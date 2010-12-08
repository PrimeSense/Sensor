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






#ifndef _XNV_POINT_H_
#define _XNV_POINT_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <XnPlatform.h>
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * XnVPoint represents a point in either Projective or Real World coordinates.
 * The XnVPoint doens't know which coordinates are used - this is the user's responsibility.
 * The X and Y axis values are floating points, because Real World coordinates require it.
 * The Z axis value (or depth) is XN_DEPTH_TYPE.
 */

class XN_EE_CORE_API XnVPoint
{
public:
	XnVPoint()
	{}

	XnVPoint(const XnVPoint& other)
	{
		SetPoint(other.X(), other.Y(), other.Z());
	}
	XnVPoint(XnFloat fX, XnFloat fY, XnFloat fZ)
	{
		SetPoint(fX, fY, fZ);
	}

	inline void SetPoint(XnFloat fX, XnFloat fY, XnFloat fZ)
	{
		X() = fX; Y() = fY; Z() = fZ;
	}

	inline const XnVPoint& operator=(const XnVPoint& ptOther)
	{
		SetPoint(ptOther.X(), ptOther.Y(), ptOther.Z());
		return *this;
	}

	inline XnFloat X() const
	{
		return elements[0];
	}
	inline XnFloat& X()
	{
		return elements[0];
	}

	inline XnFloat Y() const
	{
		return elements[1];
	}
	inline XnFloat& Y()
	{
		return elements[1];
	}

	inline XnFloat Z() const
	{
		return elements[2];
	}
	inline XnFloat& Z()
	{
		return elements[2];
	}

protected:
	// Actually holding 4 elements, for alignment
	XnFloat elements[4];
};

#endif //_XNV_POINT_H_

