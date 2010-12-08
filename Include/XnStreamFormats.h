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






#ifndef __XN_STREAM_FORMATS_H__
#define __XN_STREAM_FORMATS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnTypes.h>

//---------------------------------------------------------------------------
// Pixel Types
//---------------------------------------------------------------------------
/** The basic depth buffer unit. */
//#define XN_DEPTH_TYPE XnUInt16
/** The basic image buffer unit. */
//#define XN_IMAGE_TYPE XnUInt8
/** The basic miscellaneous buffer unit. */
#define XN_MISC_TYPE  XnUInt8
/** The basic audio buffer unit. */
#define XN_AUDIO_TYPE XnUInt8
/** The miscellaneous confidence map buffer unit. */
#define XN_MISC_CONF_TYPE XnUInt8

//---------------------------------------------------------------------------
// Formats
//---------------------------------------------------------------------------

/** The different types of stream outputs. */
typedef enum
{
	XN_OUTPUT_FORMAT_SHIFT_VALUES = 0,
	XN_OUTPUT_FORMAT_DEPTH_VALUES = 1,
	XN_OUTPUT_FORMAT_GRAYSCALE8 = 2,
	XN_OUTPUT_FORMAT_GRAYSCALE16 = 3,
	XN_OUTPUT_FORMAT_YUV422 = 4,
	XN_OUTPUT_FORMAT_RGB24 = 5,
	XN_OUTPUT_FORMAT_JPEG = 6,
	XN_OUTPUT_FORMAT_PCM = 7,
	XN_OUTPUT_FORMATS_COUNT,
} XnOutputFormats;

typedef enum
{
	/** Data is stored uncompressed. */
	XN_COMPRESSION_NONE = 0,
	/** Data is compressed using PS lossless 16-bit depth compression. */
	XN_COMPRESSION_16Z = 1,
	/** Data is compressed using PS lossless 16-bit depth compression with embedded tables. */
	XN_COMPRESSION_16Z_EMB_TABLE = 2,
	/** Data is compressed using PS lossless 8-bit image compression (for grayscale). */
	XN_COMPRESSION_COLOR_8Z = 3,
	/** Data is compressed using JPEG. */
	XN_COMPRESSION_JPEG = 4,
	/** Data is packed in 10-bit values. */
	XN_COMPRESSION_10BIT_PACKED = 5,
} XnCompressionFormats;

#endif //__XN_STREAM_FORMATS_H__