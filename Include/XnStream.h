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



#ifndef _XN_STREAM_H_
#define _XN_STREAM_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCore.h>
#include <XnStreamParams.h>

#if (!defined _XN_IO_BC && !defined XN_USE_DEVICE_3_6)
	#define XN_OLD_IO_API XN_API_DEPRECATED("This is old XnIO API. Please use XnVDevice instead.")
	#define _XN_DEPRECATE_OLD_IO
#else
	#define XN_OLD_IO_API
#endif

// some structs here that are deprecated holds other deprecated, so diable warnings
#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)

/** The stream flag unit. */
#define XN_STREAM_FLAGS_TYPE XnInt32

/** Perform a horizontal flip on the output buffers. */
#define XN_STREAM_FLAG_MIRROR 1

#ifdef _XN_DEPRECATE_OLD_IO
#pragma deprecated ("XN_STREAM_FLAGS_TYPE", "XN_STREAM_FLAG_MIRROR")
#endif

typedef XN_OLD_IO_API enum XnAudioReadMode
{
	XN_AUDIO_READ_FRAMES,
	XN_AUDIO_READ_STREAM
} XnAudioReadMode;

//---------------------------------------------------------------------------
// Stream Buffer Formats
//---------------------------------------------------------------------------
/** The different types of depth buffers. */
typedef XN_OLD_IO_API enum {
	/** Disable the depth buffer. */
	XN_DEPTH_FORMAT_DISABLED = 0,
	/** 10-bit depth values stored in a 16-bit buffer. */
	XN_DEPTH_FORMAT_RAW10 = 1,
	/** 12-bit depth values stored in a 16-bit buffer. */
	XN_DEPTH_FORMAT_RAW12 = 2,
	/** Shift values, in pixels. */
	XN_DEPTH_FORMAT_SHIFTS = 3,
} XnStreamDepthFormat;

/** The different types of image buffers. */
typedef XN_OLD_IO_API enum {
	/** Disable the image buffer. */
	XN_IMAGE_FORMAT_DISABLED = 0,
	/** Grayscale only image buffer. */
	XN_IMAGE_FORMAT_GRAYSCALE8 = 1,
	/** Red, Green and Blue image buffer. */
	XN_IMAGE_FORMAT_RGB24 = 2,
	/** YUV422 (uyvy) image buffer. */
	XN_IMAGE_FORMAT_YUV422 = 3,
} XnStreamImageFormat;

/** The different types of audio buffers. */
typedef XN_OLD_IO_API enum XnStreamAudioFormat {
	/** Disable the audio buffer. */
	XN_AUDIO_FORMAT_DISABLED = 0,
	/** PCM audio buffer. */
	XN_AUDIO_FORMAT_PCM,
} XnStreamAudioFormat;

/** The different types of miscellaneous buffers. */
typedef XN_OLD_IO_API enum {
	/** Disable the miscellaneous buffer. */
	XN_MISC_FORMAT_DISABLED = 0,
	/** 4-bit confidence map stored in an 8-bit buffer. */
	XN_MISC_FORMAT_CONFIDENCE_MAP = 1
} XnStreamMiscFormat;

/** The default depth buffer format. */
#define XN_STREAM_DEFAULT_DEPTH_FORMAT XN_DEPTH_FORMAT_RAW12
/** The default image buffer format. */
#define XN_STREAM_DEFAULT_IMAGE_FORMAT XN_IMAGE_FORMAT_RGB24
/** The default miscellaneous buffer format. */
#define XN_STREAM_DEFAULT_MISC_FORMAT XN_MISC_FORMAT_CONFIDENCE_MAP

#ifdef _XN_DEPRECATE_OLD_IO
#pragma deprecated (XN_STREAM_DEFAULT_DEPTH_FORMAT, XN_STREAM_DEFAULT_IMAGE_FORMAT, XN_STREAM_DEFAULT_MISC_FORMAT)
#endif

//---------------------------------------------------------------------------
// Stream Compressions
//---------------------------------------------------------------------------

/** The different types of depth stream compression formats. */
typedef XN_OLD_IO_API enum {
	/** Disable the depth buffer. */
	XN_COMPRESSED_DEPTH_FORMAT_SKIP = 0,
	/** Uncompressed. */
	XN_COMPRESSED_DEPTH_FORMAT_UNCOMPRESSED = 1,
	/** 16-bit Prime Sense Lossless Depth Compression. */
	XN_COMPRESSED_DEPTH_FORMAT_16Z = 2,
	/** 16-bit Prime Sense Lossless Depth Compression + embedded tables. */
	XN_COMPRESSED_DEPTH_FORMAT_16ZEMBTABLE = 4,
} XnStreamDepthCompressionFormat;

/** The different types of image stream compression formats. */
typedef XN_OLD_IO_API enum {
	/** Disable the image buffer. */
	XN_COMPRESSED_IMAGE_FORMAT_SKIP = 0,
	/** Uncompressed. */
	XN_COMPRESSED_IMAGE_FORMAT_UNCOMPRESSED = 1,
	/** 8-bit Prime Sense Lossless Image Compression. */
	XN_COMPRESSED_IMAGE_FORMAT_8Z = 2,
	/** JPEG compression (default) */
	XN_COMPRESSED_IMAGE_FORMAT_JPEG = 3,
	/** IR packed 10 bit */
	XN_COMPRESSED_IMAGE_FORMAT_IR10 = 4,
} XnStreamImageCompressionFormat;

/** The different types of miscellaneous stream compression formats. */
typedef XN_OLD_IO_API enum {
	/** Disable the miscellaneous buffer. */
	XN_COMPRESSED_MISC_FORMAT_SKIP = 0,
	/** Uncompressed. */
	XN_COMPRESSED_MISC_FORMAT_UNCOMPRESSED = 1,
	/** 4-bit Confidence map. */
	XN_COMPRESSED_MISC_FORMAT_CONF4 = 2,	
	/** 4-bit Confidence map + simple LZ compression (UNSUPPORTED!). */
	XN_COMPRESSED_MISC_FORMAT_CONF4LZ = 3
} XnStreamMiscCompressionFormat;

/** The different types of audio stream compression formats. */
typedef XN_OLD_IO_API enum {
	/** Disable the audio buffer. */
	XN_COMPRESSED_AUDIO_FORMAT_SKIP = 0,
	/** Uncompressed. */
	XN_COMPRESSED_AUDIO_FORMAT_UNCOMPRESSED = 1,
} XnStreamAudioCompressionFormat;

/** The default depth compression format. */
#define XN_STREAM_DEFAULT_DEPTH_COMPRESSION_FORMAT XN_COMPRESSED_DEPTH_FORMAT_16ZEMBTABLE
/** The default image compression format. */
#define XN_STREAM_DEFAULT_IMAGE_COMPRESSION_FORMAT XN_COMPRESSED_IMAGE_FORMAT_JPEG
/** The default misc compression format. */
#define XN_STREAM_DEFAULT_MISC_COMPRESSION_FORMAT XN_COMPRESSED_MISC_FORMAT_SKIP
/** The default audio compression format. */
#define XN_STREAM_DEFAULT_AUDIO_COMPRESSION_FORMAT XN_COMPRESSED_AUDIO_FORMAT_UNCOMPRESSED


//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------
#pragma pack (push, 1)

typedef XN_OLD_IO_API struct Shift2DepthStruct 
{
	/** True if shift-to-depth params are available. */
	XnBool		bShift2DepthData;
	XnUInt32	nConstShift;
	XnUInt32	nPixelSizeFactor;
	/** The maximum possible shift value from this device. */
	XnUInt32	nMaxShiftValue;
	/** The maximum possible depth from this device (as opposed to a cut-off). */
	XnUInt32	nMaxDepthValue;
	XnUInt32	nParamCoeff;
	XnUInt32	nShiftScale;
} Shift2DepthStruct;

/**
 * Describes the Xiron stream properties.
 * It is defined once per opened stream and will be filled automatically by the device if empty.
 */
typedef XN_OLD_IO_API struct XnStreamProperties
{
	/** A bit mask of Xiron stream flags. */
	XN_STREAM_FLAGS_TYPE	nStreamFlags;

	 /** The number of frames in this Xiron stream. */
	XnUInt32				nNumOfFrames;

	 /** The number of frames per second. Legal values are: 15-60.*/
	XnUInt8				nDepthFramesPerSecond;
	XnUInt8				nImageFramesPerSecond;

	/** Padding. */
	XnUInt16				nReserved;

	/** The depth buffer format. */
	XnStreamDepthFormat		DepthFormat;
	/** The depth X resolution. Legal values are: 160-640 and must be a multiple of 4. */
	XnUInt16				nDepthXRes;
	/** The depth Y resolution. Legal values are: 120-512 and must be a multiple of 4. */
	XnUInt16				nDepthYRes;
	/** The depth buffer size in bytes. OpenDevice will automatically update this field to the correct size.*/
	XnUInt32				nDepthBufferSize;
	/** The depth buffer element size in bits. */
	XnUInt32				nDepthTypeBitSize;

	/** The minimum depth value in the depth buffer. */
	XnDepthPixel			nDepthMinValue;
	/** The maximum depth value in the depth buffer. */
	XnDepthPixel			nDepthMaxValue;
	/** The value that represents no-sample in the depth buffer. */
	XnDepthPixel			nDepthNoSampleValue;
	/** The value that represents shadow in the depth buffer. */
	XnDepthPixel			nDepthShadowValue;

	/** The image buffer format. */
	XnStreamImageFormat		ImageFormat;
	/** The image X resolution. Legal values are: 160-640 and must be a multiply of 4. */
	XnUInt16				nImageXRes;
	/** The image Y resolution. Legal values are: 120-512 and must be a multiply of 4. */
	XnUInt16				nImageYRes;
	/** The image buffer size in bytes. OpenDevice will automatically update this field to the correct size.*/
	XnUInt32				nImageBufferSize;
	/** The image buffer element size in bits. */
	XnUInt32				nImageTypeBitSize;

	/** The audio buffer format. */
	XnStreamAudioFormat		AudioFormat;
	/** The number of audio channels. */
	XnUInt8				nAudioNumOfChannels;

	/** Padding. */
	XnUInt8				nReserved2;
	XnUInt16				nReserved3;

	/** The audio sample rate. */
	XnSampleRate			nAudioSampleRate;
	/** The audio buffer size in bytes. OpenDevice will automatically update this field to the correct size.*/
	XnUInt32				nAudioBufferSize;
	/** Audio read mode. */
	XnAudioReadMode			AudioReadMode;
	/** When AudioReadMode is XN_AUDIO_READ_STREAM, this member is the number of bytes that will be read each time. */
	XnUInt32				nAudioReadChunkSize;

	 /** The miscellaneous buffer format. */
	XnStreamMiscFormat		MiscFormat;
	 /** The miscellaneous buffer size in bytes. OpenDevice will automatically update this field to the correct size.*/
	XnUInt32				nMiscBufferSize;
	/** The miscellaneous buffer element size in bits. */
	XnUInt32				nMiscTypeBitSize;

	/** The zero plane distance in depth units. */
	XnDepthPixel			nZeroPlaneDistance;

	/** Padding. */
	XnUInt16				nReserved4;

	/** The zero plane pixel size */
	XnFloat				fZeroPlanePixelSize;
	/** The distance between the emitter and the Depth Cmos */
	XnFloat				fEmitterDCmosDistance;

	/** Information relevant for Shift2Depth */
	Shift2DepthStruct Shift2DepthData;

} XnStreamProperties;

/**
  * Describes the Xiron stream frame buffer that actually holds the raw data.
  * Can be reused between different streams as long as it is big enough to store the data.
  */
typedef XN_OLD_IO_API struct XnStreamFrameBuffer
{
	/** The stream properties that were used to create this frame buffer. */
	XnStreamProperties	StreamProperties;

	/** A pointer to the depth buffer. */
	XnDepthPixel*		pDepthBuffer;
	/** The depth buffer size in bytes. */
	XnUInt32			nDepthBufferSize;

	/** A pointer to the image buffer. */
	XnUInt8*			pImageBuffer;
	/** The image buffer size in bytes. */
	XnUInt32			nImageBufferSize;

	/** A pointer to the miscellaneous buffer. */
	XN_MISC_TYPE*		pMiscBuffer;
	/** The miscellaneous buffer size in bytes. */
	XnUInt32			nMiscBufferSize;

	/** A pointer to the audio buffer. */
	XnUChar*			pAudioBuffer;
	/** The audio buffer size in bytes. */
	XnUInt32			nAudioBufferSize;
	/** The actual number of bytes written in the audio buffer. */
	XnUInt32			nAudioWrittenBytes;

	void*				pInternalData;
} XnStreamFrameBuffer;

/**
  * Describes the Xiron stream frame properties.
  * Every single frame of the stream should have one defined.
  */
typedef XN_OLD_IO_API struct XnStreamFrameProperties
{
	union
	{
		/** Backward compatibility - general ID equals the Depth Frame ID */
		XnUInt32	nFrameID;
		/** The frame id of this stream frame. */
		XnInt32	nDepthFrameID;
	};
	XnUInt32	nImageFrameID;

	union
	{
		/** Backward compatibility - general TimeStamp equals the Depth Frame ID */
		XnUInt64	nTimeStamp;
		/** The creation time of this stream frame. */
		XnUInt64	nDepthTimeStamp;
	};
	XnUInt64	nImageTimeStamp;

	/** The creation time of last audio packet in the buffer. */
	XnUInt64	nAudioTimeStamp;
} XnStreamFrameProperties;

#pragma pack (pop)

#pragma warning (pop)

#endif //_XN_STREAM_H_
