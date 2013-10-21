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

