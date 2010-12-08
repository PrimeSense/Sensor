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






#ifndef _XNV_MATH_COMMON_H_
#define _XNV_MATH_COMMON_H_

#include <math.h>
#include <XnOS.h>

namespace XnVMathCommon
{

inline void Exchange(XnFloat& a, XnFloat& b)
{
	XnFloat c = a;
	a = b;
	b = c;
}

inline void ExchangeSort(XnFloat& a, XnFloat& b)
{
	if (a > b)
		Exchange(a, b);
}

inline void ExchangeSort(XnFloat& a, XnFloat& b, XnFloat& c)
{
	if (a > b)
		Exchange(a, b);
	if (b > c)
		Exchange(b, c);
	if (a > b)
		Exchange(a, b);
}

inline XnFloat Sqr(XnFloat a)
{
	return a*a;
}

inline XnFloat Max(XnFloat a, XnFloat b)
{
	return (a > b ? a : b);
}

inline XnFloat Min(XnFloat a, XnFloat b)
{
	return (a < b ? a : b);
}

inline XnFloat MaxAbs(XnFloat a, XnFloat b)
{
	return Max(fabs(a), fabs(b));
}

inline XnFloat MinAbs(XnFloat a, XnFloat b)
{
	return Min(fabs(a), fabs(b));
}

inline XnUInt16 ArgMax(XnFloat a, XnFloat b)
{
	return (a > b ? 0 : 1);
}

inline XnUInt16 ArgMax(XnFloat a, XnFloat b, XnFloat c)
{
	return (a > b ? (a > c ? 0 : 2) : (b > c ? 1 : 2));
}

inline XnUInt16 ArgMin(XnFloat a, XnFloat b)
{
	return ArgMax(-a, -b);
}

inline XnUInt16 ArgMin(XnFloat a, XnFloat b, XnFloat c)
{
	return ArgMax(-a, -b, -c);
}

const XnFloat PI=3.14159265f;
const XnFloat HALF_PI=(PI/2);

inline XnBool IsZero(XnFloat f, XnFloat fTolerance)
{
	return fabs(f) < fTolerance;
}

}
#endif //_XNV_MATH_COMMON_H_
