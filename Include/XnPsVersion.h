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






#ifndef _XN_PS_VERSION_H_
#define _XN_PS_VERSION_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/** Xiron major version. */ 
#define XN_PS_MAJOR_VERSION 5
/** Xiron minor version. */ 
#define XN_PS_MINOR_VERSION 0
/** Xiron maintenance version. */ 
#define XN_PS_MAINTENANCE_VERSION 3
/** Xiron build version. */ 
#define XN_PS_BUILD_VERSION 3

/** Xiron version (in brief string format): "Major.Minor.Maintenance (Build)" */ 
#define XN_PS_BRIEF_VERSION_STRING \
	XN_STRINGIFY(XN_PS_MAJOR_VERSION) "." \
	XN_STRINGIFY(XN_PS_MINOR_VERSION) "." \
	XN_STRINGIFY(XN_PS_MAINTENANCE_VERSION) \
	" (Build " XN_STRINGIFY(XN_PS_BUILD_VERSION) ")"

/** Xiron version (in numeric format): (Xiron major version * 100000000 + Xiron minor version * 1000000 + Xiron maintenance version * 10000 + Xiron build version). */
#define XN_PS_VERSION (XN_PS_MAJOR_VERSION*100000000 + XN_PS_MINOR_VERSION*1000000 + XN_PS_MAINTENANCE_VERSION*10000 + XN_PS_BUILD_VERSION)

/** Xiron version (in string format): "Major.Minor.Maintenance.Build-Platform (MMM DD YYYY HH:MM:SS)". */ 
#define XN_PS_VERSION_STRING \
		XN_PS_BRIEF_VERSION_STRING  "-" \
		XN_PLATFORM_STRING " (" XN_TIMESTAMP ")"

#endif //_XN_VERSION_H_

