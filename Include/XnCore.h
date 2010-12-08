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
