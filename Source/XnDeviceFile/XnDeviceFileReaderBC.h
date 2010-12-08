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






#ifndef _XN_DEVICE_FILE_READER_BC_H_
#define _XN_DEVICE_FILE_READER_BC_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceFile.h"

#define _XN_IO_BC
#include <XnStream.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#pragma pack (push, 1)

typedef struct XnPackedStreamProperties
{
	XnStreamDepthCompressionFormat StreamDepthCompressionFormat;
	XnStreamImageCompressionFormat StreamImageCompressionFormat;
	XnStreamMiscCompressionFormat StreamMiscCompressionFormat;
	XnStreamAudioCompressionFormat StreamAudioCompressionFormat;
} XnPackedStreamProperties;

typedef struct XnPackedStreamPropertiesV1
{
	XnStreamDepthCompressionFormat StreamDepthCompressionFormat;
	XnStreamImageCompressionFormat StreamImageCompressionFormat;
	XnStreamMiscCompressionFormat StreamMiscCompressionFormat;
} XnPackedStreamPropertiesV1;

typedef XnPackedStreamPropertiesV1 XnPackedStreamPropertiesV2;

typedef struct XnPackedStreamFrameHeaderV1
{
	XnUInt32 nCompDepthBufferSize;
	XnUInt32 nCompImageBufferSize;
	XnUInt32 nCompMiscBufferSize;
} XnPackedStreamFrameHeaderV1;

typedef XnPackedStreamFrameHeaderV1 XnPackedStreamFrameHeaderV2;

typedef struct XnPackedStreamFrameHeaderV3
{
	XnUInt32 nCompDepthBufferSize;
	XnUInt32 nCompImageBufferSize;
	XnUInt32 nCompMiscBufferSize;
	XnUInt32 nCompAudioBufferSize;
} XnPackedStreamFrameHeaderV3;

typedef struct XnShift2DepthStruct 
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
} XnShift2DepthStruct;

typedef struct XnStreamPropertiesV3
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
	XnSampleRate		nAudioSampleRate;
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
	XnShift2DepthStruct		Shift2DepthData;

} XnStreamPropertiesV3;

#pragma pack (pop)

typedef struct XnStreamPropertiesV2
{
	/** A bit mask of Xiron stream flags. */
	XN_STREAM_FLAGS_TYPE	nStreamFlags;

	/** The number of frames in this Xiron stream. */
	XnUInt32				nNumOfFrames;
	/** The number of frames per second. Legal values are: 15-60.*/
	XnUInt8				nDepthFramesPerSecond;
	XnUInt8				nImageFramesPerSecond;

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

	/** The miscellaneous buffer format. */
	XnStreamMiscFormat		MiscFormat;
	/** The miscellaneous buffer size in bytes. OpenDevice will automatically update this field to the correct size.*/
	XnUInt32				nMiscBufferSize;
	/** The miscellaneous buffer element size in bits. */
	XnUInt32				nMiscTypeBitSize;

	/** The zero plane distance in depth units. */
	XnDepthPixel			nZeroPlaneDistance;
	/** The zero plane pixel size */
	XnFloat				fZeroPlanePixelSize;
	/** The distance between the emitter and the Depth Cmos */
	XnFloat				fEmitterDCmosDistance;

	/** Information relevant for Shift2Depth */
	struct {
		XnBool		bShift2DepthData;
		XnUInt32	nConstShift;
		XnUInt32	nPixelSizeFactor;
		XnUInt32	nMaxShiftValue;
		XnUInt32	nMaxDepthValue;
		XnUInt32	nParamCoeff;
		XnUInt32	nShiftScale;
	} Shift2DepthData;

} XnStreamPropertiesV2;

typedef struct XnStreamPropertiesV1
{
	/** A bit mask of Xiron stream flags. */
	XN_STREAM_FLAGS_TYPE	nStreamFlags;

	/** The number of frames in this Xiron stream. */
	XnUInt32				nNumOfFrames;
	/** The number of frames per second. Legal values are: 15-60.*/
	XnUInt8				nFramesPerSecond;

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

	/** The miscellaneous buffer format. */
	XnStreamMiscFormat		MiscFormat;
	/** The miscellaneous buffer size in bytes. OpenDevice will automatically update this field to the correct size.*/
	XnUInt32				nMiscBufferSize;
	/** The miscellaneous buffer element size in bits. */
	XnUInt32				nMiscTypeBitSize;

	/** The zero plane distance in depth units. */
	XnDepthPixel			nZeroPlaneDistance;
	/** The zero plane pixel size */
	XnFloat				fZeroPlanePixelSize;
} XnStreamPropertiesV1;

/**
  * Describes the Xiron stream frame properties.
  * Every single frame of the stream should have one defined.
  */
typedef struct XnStreamFramePropertiesV2
{
	union
	{
		/** Backward compatibility - general ID equals the Depth Frame ID */
		XnUInt32	nFrameID;
		/** The frame id of this stream frame. */
		XnUInt32	nDepthFrameID;
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
} XnStreamFramePropertiesV2;

typedef struct XnStreamFramePropertiesV1
{
	/** The frame id of this stream frame. */
	XnUInt32	nFrameID;

	/** The creation time of this stream frame. */
	XnUInt64	nTimeStamp;
} XnStreamFramePropertiesV1;

typedef struct XnDeviceFileHeaderV3
{
	XnChar cpMagic[XN_DEVICE_FILE_MAGIC_LEN];
	XnUInt16 nMajorVersion;
	XnUInt16 nMinorVersion;
	XnStreamProperties StreamProperties;
	XnPackedStreamProperties PackedStreamProperties;
} XnDeviceFileHeaderV3;

typedef struct XnDeviceFileHeaderV2
{
	XnChar cpMagic[XN_DEVICE_FILE_MAGIC_LEN];
	XnStreamPropertiesV2 StreamProperties;
	XnPackedStreamPropertiesV2 PackedStreamProperties;
} XnDeviceFileHeaderV2;

typedef struct XnDeviceFileHeaderV1
{
	XnChar cpMagic[XN_DEVICE_FILE_MAGIC_LEN];
	XnStreamPropertiesV1 StreamProperties;
	XnPackedStreamPropertiesV1 PackedStreamProperties;
} XnDeviceFileHeaderV1;

typedef struct XnDeviceFileFrameHeaderV3
{
	XnUInt32 nPackedStreamSize;
	XnUInt32 nReserved;
	XnStreamFrameProperties FrameProperties;
} XnDeviceFileFrameHeaderV3;

typedef struct XnDeviceFileFrameHeaderV2
{
	XnUInt32 nPackedStreamSize;
	XnStreamFramePropertiesV2 FrameProperties;
} XnDeviceFileFrameHeaderV2;

typedef struct XnDeviceFileFrameHeaderV1
{
	XnUInt32 nPackedStreamSize;
	XnStreamFramePropertiesV1 FrameProperties;
} XnDeviceFileFrameHeaderV1;

typedef struct XnFileBCData
{
	XN_FILE_HANDLE	FileHandle;

	XnUInt32	nFramePos;

	XnPackedStreamProperties	PackedStreamProperties;
	XnUInt8*					pPackedStreamBuffer;
	XnUInt32					nPackedStreamBufferSize;

	XnStreamPropertiesV3		StreamProperties;
} XnFileBCData;

typedef struct XnDeviceFileHeader
{
	XnChar cpMagic[XN_DEVICE_FILE_MAGIC_LEN];
	XnUInt16 nMajorVersion;
	XnUInt16 nMinorVersion;
	XnStreamProperties StreamProperties;
	XnPackedStreamProperties PackedStreamProperties;
} XnDeviceFileHeader;

#endif //_XN_DEVICE_FILE_READER_BC_H_