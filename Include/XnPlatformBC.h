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






#ifndef __XN_PS_PLATFORM_H__
#define __XN_PS_PLATFORM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#define _XN_DEPRECATED_TYPE(newType, oldType)	\
	XN_API_DEPRECATED("Please use " XN_STRINGIFY(newType) " instead.") typedef newType oldType;

_XN_DEPRECATED_TYPE(XnChar, XN_CHAR)
_XN_DEPRECATED_TYPE(XnUChar, XN_UCHAR)
_XN_DEPRECATED_TYPE(XnInt8, XN_INT8)
_XN_DEPRECATED_TYPE(XnUInt8, XN_UINT8)
_XN_DEPRECATED_TYPE(XnInt16, XN_INT16)
_XN_DEPRECATED_TYPE(XnUInt16, XN_UINT16)
_XN_DEPRECATED_TYPE(XnInt32, XN_INT32)
_XN_DEPRECATED_TYPE(XnUInt32, XN_UINT32)
_XN_DEPRECATED_TYPE(XnInt64, XN_INT64)
_XN_DEPRECATED_TYPE(XnUInt64, XN_UINT64)
_XN_DEPRECATED_TYPE(XnFloat, XN_FLOAT)
_XN_DEPRECATED_TYPE(XnDouble, XN_DOUBLE)
_XN_DEPRECATED_TYPE(XnBool, XN_BOOL)
_XN_DEPRECATED_TYPE(XnWChar, XN_WCHAR)
_XN_DEPRECATED_TYPE(XnInt, XN_LONG)
_XN_DEPRECATED_TYPE(XnUInt, XN_ULONG)

XN_API_DEPRECATED("Please use XnDepthPixel instead") typedef XnUInt16 XN_DEPTH_TYPE; 
XN_API_DEPRECATED("Please use XnRGB24Pixel instead") typedef XnUChar XN_IMAGE_TYPE; 

#endif // __XN_PS_PLATFORM_H__
