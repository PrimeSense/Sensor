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
