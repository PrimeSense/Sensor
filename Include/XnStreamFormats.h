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