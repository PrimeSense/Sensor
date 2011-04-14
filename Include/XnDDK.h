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






#ifndef _XN_DDK_H_
#define _XN_DDK_H_

#include <XnPlatform.h>
#include <XnStatus.h>
#include <XnDDKStatus.h>
#include <XnStreamParams.h>

#ifdef __cplusplus
	#ifdef XN_DDK_EXPORTS
		#define XN_DDK_API extern "C" XN_API_EXPORT	
		#define XN_DDK_CPP_API XN_API_EXPORT
	#else
		#define XN_DDK_API extern "C" XN_API_IMPORT
		#define XN_DDK_CPP_API XN_API_IMPORT
	#endif

	#ifdef XN_DEVICE_EXPORTS
		#define XN_DEVICE_API extern "C" XN_API_EXPORT	
	#else
		#define XN_DEVICE_API extern "C" XN_API_IMPORT
	#endif
#else
	#ifdef XN_DDK_EXPORTS
		#define XN_DDK_API XN_API_EXPORT	
	#else
		#define XN_DDK_API XN_API_IMPORT
	#endif

	#ifdef XN_DEVICE_EXPORTS
		#define XN_DEVICE_API XN_API_EXPORT
	#else
		#define XN_DEVICE_API XN_API_IMPORT
	#endif
#endif

#define XN_MASK_DDK	"DDK"

/**
* This function initializes the DDK library. 
* This function must be called BEFORE calling any other method of a device.
*/
XN_DDK_API XnStatus XnDDKInit(const XnChar* strDevicesDir);

/**
* This function initializes the DDK library from an INI file.
*
* @param	cpINIFileName	[in]	The name of the INI file.
*/
XN_DDK_API XnStatus XnDDKInitFromINIFile(const XnChar* cpINIFileName);

/**
* This function shuts down the DDK library.
*/
XN_DDK_API XnStatus XnDDKShutdown();

XN_DDK_API XnResolutions XnDDKGetResolutionFromXY(XnUInt32 nXRes, XnUInt32 nYRes);
XN_DDK_API XnBool XnDDKGetXYFromResolution(XnResolutions res, XnUInt32* pnXRes, XnUInt32* pnYRes);
XN_DDK_API const XnChar*  XnDDKGetResolutionName(XnResolutions res);

#endif //_XN_DDK_H_
