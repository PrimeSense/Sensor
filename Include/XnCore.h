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
#ifndef _XN_CORE_H_
#define _XN_CORE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnStatus.h>
#include <XnVersion.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#ifdef __cplusplus
	#ifdef XN_CORE_EXPORTS
		#define XN_CORE_API extern "C" XN_API_EXPORT
		#define XN_CORE_CPP_API XN_API_EXPORT
	#else
		#define XN_CORE_API extern "C" XN_API_IMPORT
		#define XN_CORE_CPP_API XN_API_IMPORT
	#endif
#else
	#ifdef XN_CORE_EXPORTS
		#define XN_CORE_API XN_API_EXPORT
	#else
		#define XN_CORE_API XN_API_IMPORT
	#endif
#endif

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
/**
 * This function initializes the core low-level SDK. 
 */
XN_CORE_API XnStatus XnInit();

/**
* This function initializes the core low-level SDK from an INI file. 
* Please refer to the low-level SDK overview/tutorial section for a complete list of INI entries.
* Note: This function is not very useful on its own. You should use the I/O subsystem initializing instead.
* 
* @param	cpINIFileName		[in]	A path to an INI file.
*/
XN_CORE_API XnStatus XnInitFromINIFile(const XnChar* cpINIFileName);

/**
* This function shuts down the core low-level SDK. 
* Note: This function is not very useful on its own. You should use the I/O subsystem shutdown instead.
*/
XN_CORE_API XnStatus XnShutdown();

/**
 * Returns the Xiron version as an integer calculated from this formula:
 * (Xiron major version * 1000 + Xiron minor version)
 *
 * @return An integer representation of the Xiron version.
 */
XN_CORE_API XnUInt32	XnGetVersion(void);

/**
 * Returns the Xiron version as a string in the following format:
 * "Major.Minor-Platform (MMM DD YYYY HH:MM:SS)"
 * For example: "1.0-Win32 (Sep 19 2006 11:22:33)"
 *
 * @return A string representation of the Xiron version.
 */
XN_CORE_API const XnChar*	XnGetVersionString(void);

#endif //_XN_CORE_H_
