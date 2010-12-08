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






#ifndef _XN_FORMATS_H_
#define _XN_FORMATS_H_

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnStatus.h>
#include <XnFormatsStatus.h>
#include <XnStreamParams.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#ifdef __cplusplus
	#ifdef XN_FORMATS_EXPORTS
		#define XN_FORMATS_API extern "C" XN_API_EXPORT
		#define XN_FORMATS_CPP_API XN_API_EXPORT
	#else
		#define XN_FORMATS_API extern "C" XN_API_IMPORT
		#define XN_FORMATS_CPP_API XN_API_IMPORT
	#endif
#else
	#ifdef XN_FORMATS_EXPORTS
		#define XN_FORMATS_API XN_API_EXPORT
	#else
		#define XN_FORMATS_API XN_API_IMPORT
	#endif
#endif

#define XN_MASK_FORMATS "XnFormats"

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------

/**
* This function initializes the formats library.
*/
XN_FORMATS_API XnStatus XnFormatsInit();

/**
* This function initializes the formats library from an INI file.
*
* @param	cpINIFileName	[in]	The name of the INI file.
*/
XN_FORMATS_API XnStatus XnFormatsInitFromINIFile(const XnChar* cpINIFileName);

/**
* This function shuts down the formats library.
*/
XN_FORMATS_API XnStatus XnFormatsShutdown();

/**
* This function receives a buffer of pixel data of a known format, and mirrors it.
*
* @param	nOutputFormat	[in]	The format of the pixel data.
* @param	pBuffer			[in]	A pointer to the buffer.
* @param	nBufferSize		[in]	The size of the buffer, in bytes.
* @param	nXRes			[in]	X-resolution (line size in pixels) of the buffer.
*/
XN_FORMATS_API XnStatus XnFormatsMirrorPixelData(XnOutputFormats nOutputFormat, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nXRes);

#endif //_XN_FORMATS_H_
