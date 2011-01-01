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






//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceFileReader.h"
#include "XnDeviceFileReaderBC.h"
//#include "XnFileReaderStreamHelper.h"
#include <XnFormats/XnStreamCompression.h>
#include <XnDDK/XnBackwardsCompatibility36.h>
#include <XnDDK/XnPropertySetInternal.h>
#include <XnDDK/XnStreamReaderStream.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnStatus XnIOAdjustStreamPropertiesV3(const XnStreamProperties* pStreamPropertiesV3, XnStreamProperties* pStreamProperties)
{
	pStreamProperties->nStreamFlags				= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nStreamFlags);
	pStreamProperties->nNumOfFrames				= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nNumOfFrames);
	pStreamProperties->nDepthFramesPerSecond	= pStreamPropertiesV3->nDepthFramesPerSecond;
	pStreamProperties->nImageFramesPerSecond	= pStreamPropertiesV3->nImageFramesPerSecond;
	pStreamProperties->DepthFormat				= XnStreamDepthFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV3->DepthFormat)));
	pStreamProperties->nDepthXRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nDepthXRes);
	pStreamProperties->nDepthYRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nDepthYRes);
	pStreamProperties->nDepthBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nDepthBufferSize);
	pStreamProperties->nDepthTypeBitSize		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nDepthTypeBitSize);
	pStreamProperties->nDepthMinValue			= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nDepthMinValue);
	pStreamProperties->nDepthMaxValue			= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nDepthMaxValue);
	pStreamProperties->nDepthNoSampleValue		= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nDepthNoSampleValue);
	pStreamProperties->nDepthShadowValue		= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nDepthShadowValue);
	pStreamProperties->ImageFormat				= XnStreamImageFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV3->ImageFormat)));
	pStreamProperties->nImageXRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nImageXRes);
	pStreamProperties->nImageYRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nImageYRes);
	pStreamProperties->nImageBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nImageBufferSize);
	pStreamProperties->nImageTypeBitSize		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nImageTypeBitSize);
	pStreamProperties->MiscFormat				= XnStreamMiscFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV3->MiscFormat)));
	pStreamProperties->nMiscBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nMiscBufferSize);
	pStreamProperties->nMiscTypeBitSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nMiscTypeBitSize);
	pStreamProperties->nZeroPlaneDistance		= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV3->nZeroPlaneDistance);
	pStreamProperties->fZeroPlanePixelSize		= XN_PREPARE_VAR_FLOAT_IN_BUFFER(pStreamPropertiesV3->fZeroPlanePixelSize);
	pStreamProperties->fEmitterDCmosDistance	= XN_PREPARE_VAR_FLOAT_IN_BUFFER(pStreamPropertiesV3->fEmitterDCmosDistance);
	pStreamProperties->Shift2DepthData.bShift2DepthData	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->Shift2DepthData.bShift2DepthData);
	pStreamProperties->Shift2DepthData.nConstShift		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->Shift2DepthData.nConstShift);
	pStreamProperties->Shift2DepthData.nPixelSizeFactor	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->Shift2DepthData.nPixelSizeFactor);
	pStreamProperties->Shift2DepthData.nMaxShiftValue	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->Shift2DepthData.nMaxShiftValue);
	pStreamProperties->Shift2DepthData.nMaxDepthValue	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->Shift2DepthData.nMaxDepthValue);
	pStreamProperties->Shift2DepthData.nParamCoeff		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->Shift2DepthData.nParamCoeff);
	pStreamProperties->Shift2DepthData.nShiftScale		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->Shift2DepthData.nShiftScale);
	pStreamProperties->AudioFormat				= XnStreamAudioFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV3->AudioFormat)));
	pStreamProperties->nAudioNumOfChannels		= pStreamPropertiesV3->nAudioNumOfChannels;
	pStreamProperties->nAudioSampleRate			= XnSampleRate(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV3->nAudioSampleRate)));
	pStreamProperties->nAudioBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nAudioBufferSize);
	pStreamProperties->AudioReadMode			= pStreamPropertiesV3->AudioReadMode;
	pStreamProperties->nAudioReadChunkSize		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV3->nAudioReadChunkSize);

	return (XN_STATUS_OK);
}

XnStatus XnIOAdjustStreamPropertiesV2 (const XnStreamPropertiesV2* pStreamPropertiesV2, XnStreamProperties* pStreamProperties)
{
	pStreamProperties->nStreamFlags				= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nStreamFlags);
	pStreamProperties->nNumOfFrames				= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nNumOfFrames);
	pStreamProperties->nDepthFramesPerSecond	= pStreamPropertiesV2->nDepthFramesPerSecond;
	pStreamProperties->nImageFramesPerSecond	= pStreamPropertiesV2->nImageFramesPerSecond;
	pStreamProperties->DepthFormat				= XnStreamDepthFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV2->DepthFormat)));
	pStreamProperties->nDepthXRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nDepthXRes);
	pStreamProperties->nDepthYRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nDepthYRes);
	pStreamProperties->nDepthBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nDepthBufferSize);
	pStreamProperties->nDepthTypeBitSize		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nDepthTypeBitSize);
	pStreamProperties->nDepthMinValue			= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nDepthMinValue);
	pStreamProperties->nDepthMaxValue			= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nDepthMaxValue);
	pStreamProperties->nDepthNoSampleValue		= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nDepthNoSampleValue);
	pStreamProperties->nDepthShadowValue		= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nDepthShadowValue);
	pStreamProperties->ImageFormat				= XnStreamImageFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV2->ImageFormat)));
	pStreamProperties->nImageXRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nImageXRes);
	pStreamProperties->nImageYRes				= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nImageYRes);
	pStreamProperties->nImageBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nImageBufferSize);
	pStreamProperties->nImageTypeBitSize		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nImageTypeBitSize);
	pStreamProperties->MiscFormat				= XnStreamMiscFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(pStreamPropertiesV2->MiscFormat)));
	pStreamProperties->nMiscBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nMiscBufferSize);
	pStreamProperties->nMiscTypeBitSize			= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->nMiscTypeBitSize);
	pStreamProperties->nZeroPlaneDistance		= XN_PREPARE_VAR16_IN_BUFFER(pStreamPropertiesV2->nZeroPlaneDistance);
	pStreamProperties->fZeroPlanePixelSize		= XN_PREPARE_VAR_FLOAT_IN_BUFFER(pStreamPropertiesV2->fZeroPlanePixelSize);
	pStreamProperties->fEmitterDCmosDistance	= XN_PREPARE_VAR_FLOAT_IN_BUFFER(pStreamPropertiesV2->fEmitterDCmosDistance);
	pStreamProperties->Shift2DepthData.bShift2DepthData	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->Shift2DepthData.bShift2DepthData);
	pStreamProperties->Shift2DepthData.nConstShift		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->Shift2DepthData.nConstShift);
	pStreamProperties->Shift2DepthData.nPixelSizeFactor	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->Shift2DepthData.nPixelSizeFactor);
	pStreamProperties->Shift2DepthData.nMaxShiftValue	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->Shift2DepthData.nMaxShiftValue);
	pStreamProperties->Shift2DepthData.nMaxDepthValue	= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->Shift2DepthData.nMaxDepthValue);
	pStreamProperties->Shift2DepthData.nParamCoeff		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->Shift2DepthData.nParamCoeff);
	pStreamProperties->Shift2DepthData.nShiftScale		= XN_PREPARE_VAR32_IN_BUFFER(pStreamPropertiesV2->Shift2DepthData.nShiftScale);

	// fix old bug: in versions 2.x (until 2.6) pixel size was written 2 when decimation was enabled
	// where it should be 2 when decimation is disabled. We don't know if decimation was used or not, 
	// so we deduce it from the FPS and resolution.
	// In this case, we need to fix the pixel size factor.
	if (pStreamProperties->Shift2DepthData.nPixelSizeFactor == 2 && pStreamProperties->nDepthXRes == 320 && pStreamProperties->nDepthFramesPerSecond == 30)
		pStreamProperties->Shift2DepthData.nPixelSizeFactor = 1;

	pStreamProperties->AudioFormat				= XnStreamAudioFormat(XN_PREPARE_VAR32_IN_BUFFER(XnUInt32(XN_AUDIO_FORMAT_DISABLED)));
	pStreamProperties->nAudioBufferSize			= XN_PREPARE_VAR32_IN_BUFFER(0);

	return (XN_STATUS_OK);
}

XnStatus XnIOAdjustStreamPropertiesV1 (const XnStreamPropertiesV1* pStreamPropertiesV1, XnStreamProperties* pStreamProperties)
{
	// adjust from V1 to V2
	XnStreamPropertiesV2 StreamPropertiesV2;

	StreamPropertiesV2.nStreamFlags				= pStreamPropertiesV1->nStreamFlags;
	StreamPropertiesV2.nNumOfFrames				= pStreamPropertiesV1->nNumOfFrames;
	StreamPropertiesV2.nDepthFramesPerSecond	= pStreamPropertiesV1->nFramesPerSecond;
	StreamPropertiesV2.nImageFramesPerSecond	= pStreamPropertiesV1->nFramesPerSecond;
	StreamPropertiesV2.DepthFormat				= pStreamPropertiesV1->DepthFormat;
	StreamPropertiesV2.nDepthXRes				= pStreamPropertiesV1->nDepthXRes;
	StreamPropertiesV2.nDepthYRes				= pStreamPropertiesV1->nDepthYRes;
	StreamPropertiesV2.nDepthBufferSize			= pStreamPropertiesV1->nDepthBufferSize;
	StreamPropertiesV2.nDepthTypeBitSize		= pStreamPropertiesV1->nDepthTypeBitSize;
	StreamPropertiesV2.nDepthMinValue			= pStreamPropertiesV1->nDepthMinValue;
	StreamPropertiesV2.nDepthMaxValue			= pStreamPropertiesV1->nDepthMaxValue;
	StreamPropertiesV2.nDepthNoSampleValue		= pStreamPropertiesV1->nDepthNoSampleValue;
	StreamPropertiesV2.nDepthShadowValue		= pStreamPropertiesV1->nDepthShadowValue;
	StreamPropertiesV2.ImageFormat				= pStreamPropertiesV1->ImageFormat;
	StreamPropertiesV2.nImageXRes				= pStreamPropertiesV1->nImageXRes;
	StreamPropertiesV2.nImageYRes				= pStreamPropertiesV1->nImageYRes;
	StreamPropertiesV2.nImageBufferSize			= pStreamPropertiesV1->nImageBufferSize;
	StreamPropertiesV2.nImageTypeBitSize		= pStreamPropertiesV1->nImageTypeBitSize;
	StreamPropertiesV2.MiscFormat				= pStreamPropertiesV1->MiscFormat;
	StreamPropertiesV2.nMiscBufferSize			= pStreamPropertiesV1->nMiscBufferSize;
	StreamPropertiesV2.nMiscTypeBitSize			= pStreamPropertiesV1->nMiscTypeBitSize;
	StreamPropertiesV2.nZeroPlaneDistance		= pStreamPropertiesV1->nZeroPlaneDistance;
	StreamPropertiesV2.fZeroPlanePixelSize		= pStreamPropertiesV1->fZeroPlanePixelSize;
	StreamPropertiesV2.fEmitterDCmosDistance	= 11;
	StreamPropertiesV2.Shift2DepthData.bShift2DepthData = false;

	// 1.4 Code: 1280/XRes. this is the factor, to be multiplied with 2.
	StreamPropertiesV2.Shift2DepthData.nPixelSizeFactor = (1280/2)/StreamPropertiesV2.nDepthXRes;

	// and now adjust from V2 to current
	return XnIOAdjustStreamPropertiesV2(&StreamPropertiesV2, pStreamProperties);
}

XnStatus XnIOAdjustStreamFramePropertiesV3(const XnStreamFrameProperties* pStreamFramePropertiesV3, XnStreamFrameProperties* pStreamFrameProperties)
{
	pStreamFrameProperties->nDepthFrameID	= XN_PREPARE_VAR32_IN_BUFFER(pStreamFramePropertiesV3->nDepthFrameID);
	pStreamFrameProperties->nImageFrameID	= XN_PREPARE_VAR32_IN_BUFFER(pStreamFramePropertiesV3->nImageFrameID);
	pStreamFrameProperties->nDepthTimeStamp	= XN_PREPARE_VAR64_IN_BUFFER(pStreamFramePropertiesV3->nDepthTimeStamp);
	pStreamFrameProperties->nImageTimeStamp	= XN_PREPARE_VAR64_IN_BUFFER(pStreamFramePropertiesV3->nImageTimeStamp);
	pStreamFrameProperties->nAudioTimeStamp = XN_PREPARE_VAR64_IN_BUFFER(pStreamFramePropertiesV3->nAudioTimeStamp);

	return (XN_STATUS_OK);
}

XnStatus XnIOAdjustStreamFramePropertiesV2 (const XnStreamFramePropertiesV2* pStreamFramePropertiesV2, XnStreamFrameProperties* pStreamFrameProperties)
{
	pStreamFrameProperties->nDepthFrameID	= XN_PREPARE_VAR32_IN_BUFFER(pStreamFramePropertiesV2->nDepthFrameID);
	pStreamFrameProperties->nImageFrameID	= XN_PREPARE_VAR32_IN_BUFFER(pStreamFramePropertiesV2->nImageFrameID);
	pStreamFrameProperties->nDepthTimeStamp	= XN_PREPARE_VAR64_IN_BUFFER(pStreamFramePropertiesV2->nDepthTimeStamp);
	pStreamFrameProperties->nImageTimeStamp	= XN_PREPARE_VAR64_IN_BUFFER(pStreamFramePropertiesV2->nImageTimeStamp);
	pStreamFrameProperties->nAudioTimeStamp = XN_PREPARE_VAR64_IN_BUFFER(0);

	return (XN_STATUS_OK);
}

XnStatus XnIOAdjustStreamFramePropertiesV1 (const XnStreamFramePropertiesV1* pStreamFramePropertiesV1, XnStreamFrameProperties* pStreamFrameProperties)
{
	// adjust to V2
	XnStreamFramePropertiesV2 StreamFramePropertiesV2;

	StreamFramePropertiesV2.nDepthFrameID	= pStreamFramePropertiesV1->nFrameID;
	StreamFramePropertiesV2.nImageFrameID	= pStreamFramePropertiesV1->nFrameID;
	StreamFramePropertiesV2.nDepthTimeStamp	= pStreamFramePropertiesV1->nTimeStamp;
	StreamFramePropertiesV2.nImageTimeStamp	= pStreamFramePropertiesV1->nTimeStamp;

	// and now adjust from V2 to current
	return XnIOAdjustStreamFramePropertiesV2(&StreamFramePropertiesV2, pStreamFrameProperties);
}

XnStatus XnIOAdjustPackedStreamPropertiesV2 (const XnPackedStreamPropertiesV2* pPackedStreamPropertiesV2, XnPackedStreamProperties* pPackedStreamProperties)
{
	pPackedStreamProperties->StreamDepthCompressionFormat	= XnStreamDepthCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamPropertiesV2->StreamDepthCompressionFormat));
	pPackedStreamProperties->StreamImageCompressionFormat	= XnStreamImageCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamPropertiesV2->StreamImageCompressionFormat));
	pPackedStreamProperties->StreamMiscCompressionFormat	= XnStreamMiscCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamPropertiesV2->StreamMiscCompressionFormat));
	pPackedStreamProperties->StreamAudioCompressionFormat	= XnStreamAudioCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(XN_COMPRESSED_AUDIO_FORMAT_SKIP));

	return XN_STATUS_OK;
}

XnStatus XnIOAdjustPackedStreamFrameHeaderV2 (const XnPackedStreamFrameHeaderV2* pPackedStreamFrameHeaderV2, XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeader)
{
	pPackedStreamFrameHeader->nCompDepthBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamFrameHeaderV2->nCompDepthBufferSize);
	pPackedStreamFrameHeader->nCompImageBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamFrameHeaderV2->nCompImageBufferSize);
	pPackedStreamFrameHeader->nCompMiscBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamFrameHeaderV2->nCompMiscBufferSize);
	pPackedStreamFrameHeader->nCompAudioBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(0);

	return XN_STATUS_OK;
}

XnStatus XnIOAdjustPackedStreamPropertiesV1 (const XnPackedStreamPropertiesV1* pPackedStreamPropertiesV1, XnPackedStreamProperties* pPackedStreamProperties)
{
	// V1 and V2 are the same, so adjust from V2 to current
	return XnIOAdjustPackedStreamPropertiesV2(pPackedStreamPropertiesV1, pPackedStreamProperties);
}

XnStatus XnIOAdjustPackedStreamFrameHeaderV1 (const XnPackedStreamFrameHeaderV1* pPackedStreamFrameHeaderV1, XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeader)
{
	// V1 and V2 are the same, so adjust from V2 to current
	return XnIOAdjustPackedStreamFrameHeaderV2(pPackedStreamFrameHeaderV1, pPackedStreamFrameHeader);
}

XnStatus XnIOAdjustPackedStreamPropertiesV3 (const XnPackedStreamProperties* pPackedStreamPropertiesV3, XnPackedStreamProperties* pPackedStreamProperties)
{
	pPackedStreamProperties->StreamDepthCompressionFormat	= XnStreamDepthCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamPropertiesV3->StreamDepthCompressionFormat));
	pPackedStreamProperties->StreamImageCompressionFormat	= XnStreamImageCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamPropertiesV3->StreamImageCompressionFormat));
	pPackedStreamProperties->StreamMiscCompressionFormat	= XnStreamMiscCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamPropertiesV3->StreamMiscCompressionFormat));
	pPackedStreamProperties->StreamAudioCompressionFormat	= XnStreamAudioCompressionFormat(XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamPropertiesV3->StreamAudioCompressionFormat));

	return XN_STATUS_OK;
}

XnStatus XnIOAdjustPackedStreamFrameHeaderV3 (const XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeaderV3, XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeader)
{
	pPackedStreamFrameHeader->nCompDepthBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamFrameHeaderV3->nCompDepthBufferSize);
	pPackedStreamFrameHeader->nCompImageBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamFrameHeaderV3->nCompImageBufferSize);
	pPackedStreamFrameHeader->nCompMiscBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamFrameHeaderV3->nCompMiscBufferSize);
	pPackedStreamFrameHeader->nCompAudioBufferSize	= XN_PREPARE_VAR32_IN_BUFFER(pPackedStreamFrameHeaderV3->nCompAudioBufferSize);

	return XN_STATUS_OK;
}

XnStatus XnDeviceFileReader::BCInit()
{
	XN_VALIDATE_CALLOC(m_pBCData, XnFileBCData, 1);
	return XN_STATUS_OK;
}

XnStatus BCSetDepthProperties(XnPropertySet* pSet, XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnPropertySetAddModule(pSet, XN_STREAM_NAME_DEPTH);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddStringProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_TYPE, XN_STREAM_TYPE_DEPTH);
	XN_IS_STATUS_OK(nRetVal);

	XnCropping cropping = {0};
	XnGeneralBuffer gbCropping = XN_PACK_GENERAL_BUFFER(cropping);
	nRetVal = XnPropertySetAddGeneralProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_CROPPING, &gbCropping);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_NUMBER_OF_FRAMES, (XnUInt64)pStreamProperties->nNumOfFrames);
	XN_IS_STATUS_OK(nRetVal);

	// set output format
	XnOutputFormats nOutputFormat;
	nRetVal = XnBCDepthFormatToOutputFormat(pStreamProperties->DepthFormat, &nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_OUTPUT_FORMAT, (XnUInt64)nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	// set resolution
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_X_RES, (XnUInt64)pStreamProperties->nDepthXRes);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_Y_RES, (XnUInt64)pStreamProperties->nDepthYRes);
	XN_IS_STATUS_OK(nRetVal);

	// set FPS
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_FPS, (XnUInt64)pStreamProperties->nDepthFramesPerSecond);
	XN_IS_STATUS_OK(nRetVal);

	// set required-output-size
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_REQUIRED_DATA_SIZE, pStreamProperties->nDepthBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	// set bytes per pixel
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_BYTES_PER_PIXEL, pStreamProperties->nDepthTypeBitSize / 8);
	XN_IS_STATUS_OK(nRetVal);

	// set cut-off
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_MAX_DEPTH, (XnUInt64)pStreamProperties->nDepthMaxValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_MIN_DEPTH, (XnUInt64)pStreamProperties->nDepthMinValue);
	XN_IS_STATUS_OK(nRetVal);

	// no-value and shadow-value
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_NO_SAMPLE, pStreamProperties->nDepthNoSampleValue);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_SHADOW, pStreamProperties->nDepthShadowValue);
	XN_IS_STATUS_OK(nRetVal);

	// zero plane
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, pStreamProperties->nZeroPlaneDistance);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = XnPropertySetAddRealProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, pStreamProperties->fZeroPlanePixelSize);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = XnPropertySetAddRealProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE, pStreamProperties->fEmitterDCmosDistance);
	XN_IS_STATUS_OK(nRetVal);

	// shift-to-depth
	if (pStreamProperties->Shift2DepthData.bShift2DepthData)
	{
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_CONST_SHIFT, pStreamProperties->Shift2DepthData.nConstShift);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR, pStreamProperties->Shift2DepthData.nPixelSizeFactor);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH, pStreamProperties->Shift2DepthData.nMaxDepthValue);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_MAX_SHIFT, pStreamProperties->Shift2DepthData.nMaxShiftValue);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_PARAM_COEFF, pStreamProperties->Shift2DepthData.nParamCoeff);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_SHIFT_SCALE, pStreamProperties->Shift2DepthData.nShiftScale);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// set some defaults
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_CONST_SHIFT, 200ULL);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR, 1ULL);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH, 4000ULL);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_MAX_SHIFT, 2047ULL);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_PARAM_COEFF, 4ULL);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_SHIFT_SCALE, 10ULL);
		XN_IS_STATUS_OK(nRetVal);
	}

	// set compression
	switch (pPackedStreamProperties->StreamDepthCompressionFormat)
	{
	case XN_COMPRESSED_DEPTH_FORMAT_UNCOMPRESSED:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_NONE);
		break;
	case XN_COMPRESSED_DEPTH_FORMAT_16Z:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_16Z);
		break;
	case XN_COMPRESSED_DEPTH_FORMAT_16ZEMBTABLE:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_DEPTH, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_16Z_EMB_TABLE);
		break;
	default:
		return XN_STATUS_DEVICE_FILE_CORRUPTED;
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus BCSetImageProperties(XnPropertySet* pSet, XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnPropertySetAddModule(pSet, XN_STREAM_NAME_IMAGE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddStringProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_TYPE, XN_STREAM_TYPE_IMAGE);
	XN_IS_STATUS_OK(nRetVal);

	XnCropping cropping = {0};
	XnGeneralBuffer gbCropping = XN_PACK_GENERAL_BUFFER(cropping);
	nRetVal = XnPropertySetAddGeneralProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_CROPPING, &gbCropping);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_NUMBER_OF_FRAMES, (XnUInt64)pStreamProperties->nNumOfFrames);
	XN_IS_STATUS_OK(nRetVal);

	// set output format
	XnOutputFormats nOutputFormat;
	nRetVal = XnBCImageFormatToOutputFormat(pStreamProperties->ImageFormat, &nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_OUTPUT_FORMAT, (XnUInt64)nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	// set resolution
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_X_RES, (XnUInt64)pStreamProperties->nImageXRes);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_Y_RES, (XnUInt64)pStreamProperties->nImageYRes);
	XN_IS_STATUS_OK(nRetVal);

	// set FPS
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_FPS, (XnUInt64)pStreamProperties->nImageFramesPerSecond);
	XN_IS_STATUS_OK(nRetVal);
	
	// set required-output-size
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_REQUIRED_DATA_SIZE, pStreamProperties->nImageBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	// set bytes per pixel
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_BYTES_PER_PIXEL, pStreamProperties->nImageTypeBitSize / 8);
	XN_IS_STATUS_OK(nRetVal);

	// set compression
	switch (pPackedStreamProperties->StreamImageCompressionFormat)
	{
	case XN_COMPRESSED_IMAGE_FORMAT_UNCOMPRESSED:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_NONE);
		break;
	case XN_COMPRESSED_IMAGE_FORMAT_8Z:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_COLOR_8Z);
		break;
	case XN_COMPRESSED_IMAGE_FORMAT_JPEG:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_JPEG);
		break;
	case XN_COMPRESSED_IMAGE_FORMAT_IR10:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_IMAGE, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_10BIT_PACKED);
		break;
	default:
		return XN_STATUS_DEVICE_FILE_CORRUPTED;
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus BCSetAudioProperties(XnPropertySet* pSet, XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnPropertySetAddModule(pSet, XN_STREAM_NAME_AUDIO);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddStringProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_TYPE, XN_STREAM_TYPE_AUDIO);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_NUMBER_OF_FRAMES, (XnUInt64)pStreamProperties->nNumOfFrames);
	XN_IS_STATUS_OK(nRetVal);

	// set output format
	XnOutputFormats nOutputFormat;
	nRetVal = XnBCAudioFormatToOutputFormat(pStreamProperties->AudioFormat, &nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_OUTPUT_FORMAT, (XnUInt64)nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	// set required-output-size
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_REQUIRED_DATA_SIZE, (XnUInt64)pStreamProperties->nAudioBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	// set SampleRate
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_SAMPLE_RATE, (XnUInt64)pStreamProperties->nAudioSampleRate);
	XN_IS_STATUS_OK(nRetVal);

	// set number of channels
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS, (XnUInt64)pStreamProperties->nAudioNumOfChannels);
	XN_IS_STATUS_OK(nRetVal);

	// set chunk size
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_READ_CHUNK_SIZE, (XnUInt64)pStreamProperties->nAudioReadChunkSize);
	XN_IS_STATUS_OK(nRetVal);

	// set compression
	switch (pPackedStreamProperties->StreamAudioCompressionFormat)
	{
	case XN_COMPRESSED_AUDIO_FORMAT_UNCOMPRESSED:
		nRetVal = XnPropertySetAddIntProperty(pSet, XN_STREAM_NAME_AUDIO, XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)XN_COMPRESSION_NONE);
		break;
	default:
		return XN_STATUS_DEVICE_FILE_CORRUPTED;
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus ConvertStreamPropertiesToPropertySet(XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties, XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	/** DEVICE module **/
	nRetVal = XnPropertySetAddModule(pSet, XN_MODULE_NAME_DEVICE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_MIRROR, (pStreamProperties->nStreamFlags & XN_STREAM_FLAG_MIRROR) == XN_STREAM_FLAG_MIRROR);
	XN_IS_STATUS_OK(nRetVal);

	// we assume BC files were always recorded with low-res timestamps
	nRetVal = XnPropertySetAddIntProperty(pSet, XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_HIGH_RES_TIMESTAMPS, FALSE);
	XN_IS_STATUS_OK(nRetVal);

	/** DEPTH stream **/
	if (pStreamProperties->DepthFormat != XN_DEPTH_FORMAT_DISABLED && pPackedStreamProperties->StreamDepthCompressionFormat != XN_COMPRESSED_DEPTH_FORMAT_SKIP)
	{
		nRetVal = BCSetDepthProperties(pSet, pStreamProperties, pPackedStreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}

	/** IMAGE stream (or IR) **/
	if (pStreamProperties->ImageFormat != XN_IMAGE_FORMAT_DISABLED && pPackedStreamProperties->StreamImageCompressionFormat != XN_COMPRESSED_IMAGE_FORMAT_SKIP)
	{
		nRetVal = BCSetImageProperties(pSet, pStreamProperties, pPackedStreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}

	/** AUDIO stream **/
	if (pStreamProperties->AudioFormat != XN_AUDIO_FORMAT_DISABLED && pPackedStreamProperties->StreamAudioCompressionFormat != XN_COMPRESSED_AUDIO_FORMAT_SKIP)
	{
		nRetVal = BCSetAudioProperties(pSet, pStreamProperties, pPackedStreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::BCCalculatePackedBufferSize()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnStreamPropertiesV3* pStreamProperties = &m_pBCData->StreamProperties;
	XnPackedStreamProperties* pPackedStreamProperties = &m_pBCData->PackedStreamProperties;

	XnUInt32 nBufferSize = 0;

	if (pStreamProperties->DepthFormat != XN_DEPTH_FORMAT_DISABLED)
	{
		if ((pStreamProperties->DepthFormat == XN_DEPTH_FORMAT_RAW12) || 
			(pStreamProperties->DepthFormat == XN_DEPTH_FORMAT_RAW10) ||
			(pStreamProperties->DepthFormat == XN_DEPTH_FORMAT_SHIFTS))
		{
			switch (pPackedStreamProperties->StreamDepthCompressionFormat)
			{
			case XN_COMPRESSED_DEPTH_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_DEPTH_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nDepthBufferSize;
				break;
			case XN_COMPRESSED_DEPTH_FORMAT_16Z:
			case XN_COMPRESSED_DEPTH_FORMAT_16ZEMBTABLE:
				nBufferSize += (XnUInt32)(pStreamProperties->nDepthBufferSize * XN_STREAM_COMPRESSION_DEPTH16Z_WORSE_RATIO);
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_DEPTH_COMPRESSION_FORMAT);
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_DEPTH_FORMAT);
		}
	}

	if (pStreamProperties->ImageFormat != XN_IMAGE_FORMAT_DISABLED)
	{
		if ((pStreamProperties->ImageFormat == XN_IMAGE_FORMAT_RGB24) || (pStreamProperties->ImageFormat == XN_IMAGE_FORMAT_GRAYSCALE8))
		{
			switch (pPackedStreamProperties->StreamImageCompressionFormat)
			{
			case XN_COMPRESSED_IMAGE_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nImageBufferSize;
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_8Z:
				nBufferSize += (XnUInt32)(pStreamProperties->nImageBufferSize * XN_STREAM_COMPRESSION_IMAGE8Z_WORSE_RATIO);
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_JPEG:
				nBufferSize += (XnUInt32)(pStreamProperties->nImageBufferSize * XN_STREAM_COMPRESSION_IMAGEJ_WORSE_RATIO);
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_IMAGE_COMPRESSION_FORMAT);
			}
		}
		else if (pStreamProperties->ImageFormat == XN_IMAGE_FORMAT_YUV422)
		{
			switch (pPackedStreamProperties->StreamImageCompressionFormat)
			{
			case XN_COMPRESSED_IMAGE_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nImageBufferSize;
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_IMAGE_COMPRESSION_FORMAT);
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT);
		}
	}

	if (pStreamProperties->MiscFormat != XN_MISC_FORMAT_DISABLED)
	{
		if (pStreamProperties->MiscFormat == XN_MISC_FORMAT_CONFIDENCE_MAP)
		{
			switch (pPackedStreamProperties->StreamMiscCompressionFormat)
			{
			case XN_COMPRESSED_MISC_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_MISC_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nMiscBufferSize;
				break;
			case XN_COMPRESSED_MISC_FORMAT_CONF4:
				nBufferSize += (XnUInt32)(pStreamProperties->nMiscBufferSize * XN_STREAM_COMPRESSION_CONF4_WORSE_RATIO);
				break;
			case XN_COMPRESSED_MISC_FORMAT_CONF4LZ:
				nBufferSize += (XnUInt32)(pStreamProperties->nMiscBufferSize * XN_STREAM_COMPRESSION_CONF4_WORSE_RATIO);
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_MISC_COMPRESSION_FORMAT);		
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_MISC_FORMAT);
		}
	}

	if (pStreamProperties->AudioFormat != XN_AUDIO_FORMAT_DISABLED)
	{
		if (pStreamProperties->AudioFormat == XN_AUDIO_FORMAT_PCM)
		{
			switch (pPackedStreamProperties->StreamAudioCompressionFormat)
			{
			case XN_COMPRESSED_AUDIO_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_AUDIO_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nAudioBufferSize;
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_AUDIO_COMPRESSION_FORMAT);
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_AUDIO_FORMAT);
		}
	}

	nBufferSize += sizeof(XnPackedStreamFrameHeaderV3);

	return nBufferSize;

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::BCReadInitialState(XnPropertySet* pSet)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	XnDeviceFileHeader DeviceFileHeader;
	XN_STREAM_FLAGS_TYPE nStreamFlags = 0;

	m_pBCData->nFramePos = 1;

	m_pBCData->pPackedStreamBuffer = NULL;
	m_pBCData->nPackedStreamBufferSize = 0;

	// read StreamProperties
	if (m_nFileVersion == 3)
	{
		// Current Version
		nRetVal = GetIOStream()->ReadData((XnUChar*)&DeviceFileHeader.nMajorVersion, sizeof(XnUInt16));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetIOStream()->ReadData((XnUChar*)&DeviceFileHeader.nMinorVersion, sizeof(XnUInt16));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetIOStream()->ReadData((XnUChar*)&DeviceFileHeader.StreamProperties, sizeof(XnStreamPropertiesV3));
		XN_IS_STATUS_OK(nRetVal);

		DeviceFileHeader.nMajorVersion = XN_PREPARE_VAR16_IN_BUFFER(DeviceFileHeader.nMajorVersion);
		DeviceFileHeader.nMinorVersion = XN_PREPARE_VAR16_IN_BUFFER(DeviceFileHeader.nMinorVersion);

		nRetVal = XnIOAdjustStreamPropertiesV3(&DeviceFileHeader.StreamProperties, &DeviceFileHeader.StreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (m_nFileVersion == 2)
	{
		// Version 2
		DeviceFileHeader.nMajorVersion = 0;
		DeviceFileHeader.nMinorVersion = 0;
		XnStreamPropertiesV2 StreamPropertiesV2;

		nRetVal = GetIOStream()->ReadData((XnUChar*)&StreamPropertiesV2, sizeof(XnStreamPropertiesV2));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnIOAdjustStreamPropertiesV2(&StreamPropertiesV2, &DeviceFileHeader.StreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (m_nFileVersion == 1)
	{
		// Version 1
		DeviceFileHeader.nMajorVersion = 0;
		DeviceFileHeader.nMinorVersion = 0;
		XnStreamPropertiesV1 StreamPropertiesV1;

		nRetVal = GetIOStream()->ReadData((XnUChar*)&StreamPropertiesV1, sizeof(XnStreamPropertiesV1));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnIOAdjustStreamPropertiesV1(&StreamPropertiesV1, &DeviceFileHeader.StreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// Bad Magic
		return XN_STATUS_IO_INVALID_STREAM_HEADER;		
	}

	// read packed stream properties
	switch (m_nFileVersion)
	{
	case 3:
		{
			nRetVal = GetIOStream()->ReadData((XnUChar*)&DeviceFileHeader.PackedStreamProperties, sizeof(XnPackedStreamProperties));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnIOAdjustPackedStreamPropertiesV3(&DeviceFileHeader.PackedStreamProperties, &DeviceFileHeader.PackedStreamProperties);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 2:
		{
			XnPackedStreamPropertiesV2 PackedStreamPropertiesV2;

			nRetVal = GetIOStream()->ReadData((XnUChar*)&PackedStreamPropertiesV2, sizeof(XnPackedStreamPropertiesV2));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnIOAdjustPackedStreamPropertiesV2(&PackedStreamPropertiesV2, &DeviceFileHeader.PackedStreamProperties);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 1:
		{
			XnPackedStreamPropertiesV1 PackedStreamPropertiesV1;

			nRetVal = GetIOStream()->ReadData((XnUChar*)&PackedStreamPropertiesV1, sizeof(XnPackedStreamPropertiesV1));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnIOAdjustPackedStreamPropertiesV1(&PackedStreamPropertiesV1, &DeviceFileHeader.PackedStreamProperties);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	default:
		return XN_STATUS_IO_INVALID_STREAM_HEADER;
	}

	// Save the stream properties into the private data (but keep the original stream flags)
	nStreamFlags = m_pBCData->StreamProperties.nStreamFlags;
	xnOSMemCopy(&m_pBCData->StreamProperties, &DeviceFileHeader.StreamProperties, sizeof(XnStreamProperties));
	m_pBCData->StreamProperties.nStreamFlags = nStreamFlags;

	if (m_pBCData->StreamProperties.Shift2DepthData.bShift2DepthData)
	{
		m_pBCData->StreamProperties.Shift2DepthData.nMaxDepthValue = 10000;
		m_pBCData->StreamProperties.nDepthMaxValue = 10000;
	}

	// Save the packed stream properties into the private data
	xnOSMemCopy(&m_pBCData->PackedStreamProperties, &DeviceFileHeader.PackedStreamProperties, sizeof(XnPackedStreamProperties));

	XnUInt32 nBufferSize = BCCalculatePackedBufferSize();
	if (nBufferSize != m_pBCData->nPackedStreamBufferSize)
	{
		xnOSFree(m_pBCData->pPackedStreamBuffer);
		XN_VALIDATE_ALIGNED_CALLOC(m_pBCData->pPackedStreamBuffer, XnUChar, nBufferSize, XN_DEFAULT_MEM_ALIGN);
		m_pBCData->nPackedStreamBufferSize = nBufferSize;
	}

	nRetVal = ConvertStreamPropertiesToPropertySet(&m_pBCData->StreamProperties, &m_pBCData->PackedStreamProperties, pSet);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileAdjustFileFrameHeaderV1(const XnDeviceFileFrameHeaderV1* pFileFrameHeaderV1, XnDeviceFileFrameHeaderV3* pFileFrameHeader)
{
	pFileFrameHeader->nPackedStreamSize = XN_PREPARE_VAR32_IN_BUFFER(pFileFrameHeaderV1->nPackedStreamSize);
	return XnIOAdjustStreamFramePropertiesV1(&pFileFrameHeaderV1->FrameProperties, &pFileFrameHeader->FrameProperties);
}

XnStatus XnDeviceFileAdjustFileFrameHeaderV2(const XnDeviceFileFrameHeaderV2* pFileFrameHeaderV2, XnDeviceFileFrameHeaderV3* pFileFrameHeader)
{
	pFileFrameHeader->nPackedStreamSize = XN_PREPARE_VAR32_IN_BUFFER(pFileFrameHeaderV2->nPackedStreamSize);
	return XnIOAdjustStreamFramePropertiesV2(&pFileFrameHeaderV2->FrameProperties, &pFileFrameHeader->FrameProperties);
}

XnStatus XnDeviceFileAdjustFileFrameHeaderV3(const XnDeviceFileFrameHeaderV3* pFileFrameHeaderV3, XnDeviceFileFrameHeaderV3* pFileFrameHeader)
{
	pFileFrameHeader->nPackedStreamSize = XN_PREPARE_VAR32_IN_BUFFER(pFileFrameHeaderV3->nPackedStreamSize);
	return XnIOAdjustStreamFramePropertiesV3(&pFileFrameHeaderV3->FrameProperties, &pFileFrameHeader->FrameProperties);
}

XnStatus XnDeviceFileReader::BCSeek(XnUInt64 nTimestamp)
{
	return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
}

XnStatus XnDeviceFileReader::BCSeekFrame(XnUInt32 nFrameID)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDeviceFileFrameHeaderV3 FileFrameHeader;
	XnUInt32 nReadBytes = 0;
	XnUInt32 nShouldRead = 0;
	XnInt32 nExpectedFrameID = 1;

	// go back to start of file
	nRetVal = Rewind();
	XN_IS_STATUS_OK(nRetVal);

	// get streams list
	XnDeviceModuleHolderList streams;
	nRetVal = GetStreamsList(streams);
	XN_IS_STATUS_OK(nRetVal);

	// Update the frame position to the new position (treat 0 as 1)
	m_pBCData->nFramePos = XN_MAX(nFrameID, 1);

	// Make sure we aren't trying to reach a frame that's beyond the number of frames
	if (m_pBCData->nFramePos > m_pBCData->StreamProperties.nNumOfFrames)
	{
		// Set the frame position to the last frame
		m_pBCData->nFramePos = m_pBCData->StreamProperties.nNumOfFrames;
	}

	// Set the file position to the first frame data (right after the file header)
	XnUInt32 nOffset = 0;
	switch (m_nFileVersion)
	{
	case 3:
		nOffset = sizeof(XnDeviceFileHeader);
		break;
	case 2:
		nOffset = sizeof(XnDeviceFileHeaderV2);
		break;
	case 1:
		nOffset = sizeof(XnDeviceFileHeaderV1);
		break;
	default:
		return (XN_STATUS_IO_INVALID_STREAM_HEADER);
	}

	nRetVal = GetIOStream()->Seek(nOffset);
	XN_IS_STATUS_OK(nRetVal);

	// If the wanted position was the first frame, we're already there
	if (m_pBCData->nFramePos == 1)
	{
		return (XN_STATUS_OK);
	}

	// Keep reading frames until we reach the wanted frame
	XnUInt32 nCurrFilePos = 1;
	while (nCurrFilePos < m_pBCData->nFramePos)
	{
		// Read the frame header
		switch (m_nFileVersion)
		{
		case 3:
			{
				nShouldRead = nReadBytes = sizeof(XnDeviceFileFrameHeaderV3);
				nRetVal = GetIOStream()->ReadData((XnUChar*)&FileFrameHeader, nReadBytes);
				XN_IS_STATUS_OK(nRetVal);
				nExpectedFrameID = nCurrFilePos;
			}
			break;
		case 2:
			{
				XnDeviceFileFrameHeaderV2 FileFrameHeaderV2;
				nShouldRead = nReadBytes = sizeof(XnDeviceFileFrameHeaderV2);
				nRetVal = GetIOStream()->ReadData((XnUChar*)&FileFrameHeaderV2, nReadBytes);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnDeviceFileAdjustFileFrameHeaderV2(&FileFrameHeaderV2, &FileFrameHeader);
				XN_IS_STATUS_OK(nRetVal);
				nExpectedFrameID = nCurrFilePos - 1;
			}
			break;
		case 1:
			{
				XnDeviceFileFrameHeaderV1 FileFrameHeaderV1;
				nShouldRead = nReadBytes = sizeof(XnDeviceFileFrameHeaderV1);
				nRetVal = GetIOStream()->ReadData((XnUChar*)&FileFrameHeaderV1, nReadBytes);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnDeviceFileAdjustFileFrameHeaderV1(&FileFrameHeaderV1, &FileFrameHeader);
				XN_IS_STATUS_OK(nRetVal);
				nExpectedFrameID = nCurrFilePos - 1;
			}
			break;
		default:
			return XN_STATUS_IO_INVALID_STREAM_HEADER;
		}

		// Make sure we got the right header size
		if (nReadBytes != nShouldRead)
		{
			return (XN_STATUS_IO_INVALID_STREAM_FRAME_HEADER);
		}

		// Skip the frame data
		XnUInt32 nPosition;
		nRetVal = GetIOStream()->Tell(&nPosition);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetIOStream()->Seek(FileFrameHeader.nPackedStreamSize + nPosition);
		XN_IS_STATUS_OK(nRetVal);

		// increment streams frame ID
		for (XnDeviceModuleHolderList::Iterator it = streams.begin(); it != streams.end(); ++it)
		{
			XnStreamReaderStream* pStream = (XnStreamReaderStream*)(*it)->GetModule();
			pStream->NewDataAvailable(0, 0);
		}

		// Make sure frame ids are sequential
		if (FileFrameHeader.FrameProperties.nDepthFrameID != nExpectedFrameID)
		{
			return (XN_STATUS_IO_STREAM_NOT_SEQUENTIAL);
		}

		// Update the current file frame position
		nCurrFilePos++;
	}

	// now read last frame (the one we wanted)
	XnBool bWrapOccured;
	nRetVal = BCReadFrame(&bWrapOccured);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::BCReadFrame(XnBool* pbWrapOccured)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	XnDeviceFileFrameHeaderV3 FileFrameHeader;
	XnUInt64 nCurrTime = 0;
	XnUInt64 nDiffTime = 0;
	XnUInt64 nFramesDiffTime = 0;
	XnUInt32 nSleepTime = 0;

	*pbWrapOccured = FALSE;

	// If we've reached the last frame, seek back to the first one
	if (m_pBCData->nFramePos > m_pBCData->StreamProperties.nNumOfFrames)
	{
		nRetVal = HandleEndOfStream();
		XN_IS_STATUS_OK(nRetVal);
		*pbWrapOccured = TRUE;
	}

	m_bFileHasData = TRUE;

	// Note: Since this is an internal function, the pointers are assumed to be checked by the caller
	// Read the frame header
	switch (m_nFileVersion)
	{
	case 3:
		{
			nRetVal = GetIOStream()->ReadData((XnUChar*)&FileFrameHeader, sizeof(XnDeviceFileFrameHeaderV3));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnDeviceFileAdjustFileFrameHeaderV3(&FileFrameHeader, &FileFrameHeader);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 2:
		{
			XnDeviceFileFrameHeaderV2 FileFrameHeaderV2;

			nRetVal = GetIOStream()->ReadData((XnUChar*)&FileFrameHeaderV2, sizeof(XnDeviceFileFrameHeaderV2));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnDeviceFileAdjustFileFrameHeaderV2(&FileFrameHeaderV2, &FileFrameHeader);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 1:
		{
			XnDeviceFileFrameHeaderV1 FileFrameHeaderV1;

			nRetVal = GetIOStream()->ReadData((XnUChar*)&FileFrameHeaderV1, sizeof(XnDeviceFileFrameHeaderV1));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnDeviceFileAdjustFileFrameHeaderV1(&FileFrameHeaderV1, &FileFrameHeader);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	default:
		return XN_STATUS_IO_INVALID_STREAM_HEADER;
	}

	FileFrameHeader.FrameProperties.nDepthFrameID = m_pBCData->nFramePos;
	FileFrameHeader.FrameProperties.nImageFrameID = m_pBCData->nFramePos;

	// Make sure we aren't going to overflow the packed stream buffer
	if (FileFrameHeader.nPackedStreamSize > m_pBCData->nPackedStreamBufferSize)
	{
		return (XN_STATUS_INPUT_BUFFER_OVERFLOW);
	}

	// Read the frame packed stream data into the packed stream buffer
	nRetVal = GetIOStream()->ReadData(m_pBCData->pPackedStreamBuffer, FileFrameHeader.nPackedStreamSize);
	XN_IS_STATUS_OK(nRetVal);

	// read the frame header
	XnPackedStreamFrameHeaderV3 PackedStreamHeader;
	XnUChar* pPackedBuffer = m_pBCData->pPackedStreamBuffer;

	switch (m_nFileVersion)
	{
	case 0:
	case 3:
		{
			xnOSMemCopy(&PackedStreamHeader, pPackedBuffer, sizeof(XnPackedStreamFrameHeaderV3));
			pPackedBuffer += sizeof(XnPackedStreamFrameHeaderV3);
			nRetVal = XnIOAdjustPackedStreamFrameHeaderV3(&PackedStreamHeader, &PackedStreamHeader);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case 2:
		{
			XnPackedStreamFrameHeaderV2* pPackedStreamHeaderV2 = (XnPackedStreamFrameHeaderV2*)pPackedBuffer;
			pPackedBuffer += sizeof(XnPackedStreamFrameHeaderV2);
			nRetVal = XnIOAdjustPackedStreamFrameHeaderV2(pPackedStreamHeaderV2, &PackedStreamHeader);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case 1:
		{
			XnPackedStreamFrameHeaderV1* pPackedStreamHeaderV1 = (XnPackedStreamFrameHeaderV1*)pPackedBuffer;
			pPackedBuffer += sizeof(XnPackedStreamFrameHeaderV1);
			nRetVal = XnIOAdjustPackedStreamFrameHeaderV1(pPackedStreamHeaderV1, &PackedStreamHeader);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	default:
		return XN_STATUS_IO_INVALID_STREAM_HEADER;
	}

	// Depth
	XnStreamDeviceStreamHolder* pStreamHolder;
	if (XN_STATUS_OK == FindStream(XN_STREAM_NAME_DEPTH, &pStreamHolder))
	{
		XnStreamReaderStream* pStream = (XnStreamReaderStream*)pStreamHolder->GetStream();

		XnStreamData* pStreamData = pStream->GetStreamData();

		// check size
		nRetVal = XnStreamDataCheckSize(pStreamData, pStream->GetRequiredDataSize());
		XN_IS_STATUS_OK(nRetVal);

		pStreamData->nDataSize = pStreamHolder->GetStream()->GetRequiredDataSize();
		nRetVal = pStreamHolder->GetCodec()->Decompress(pPackedBuffer, PackedStreamHeader.nCompDepthBufferSize, (XnUChar*)pStreamData->pData, &pStreamData->nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		pStreamData->nTimestamp = FileFrameHeader.FrameProperties.nDepthTimeStamp;
		pStreamData->nFrameID = FileFrameHeader.FrameProperties.nDepthFrameID;

		pPackedBuffer += PackedStreamHeader.nCompDepthBufferSize;

		pStream->NewDataAvailable(pStreamData->nTimestamp, pStreamData->nFrameID);
	}

	// Image
	if (XN_STATUS_OK == FindStream(XN_STREAM_NAME_IMAGE, &pStreamHolder))
	{
		XnStreamReaderStream* pStream = (XnStreamReaderStream*)pStreamHolder->GetStream();

		XnStreamData* pStreamData = pStream->GetStreamData();

		// check size
		nRetVal = XnStreamDataCheckSize(pStreamData, pStream->GetRequiredDataSize());
		XN_IS_STATUS_OK(nRetVal);

		pStreamData->nDataSize = pStreamHolder->GetStream()->GetRequiredDataSize();
		nRetVal = pStreamHolder->GetCodec()->Decompress(pPackedBuffer, PackedStreamHeader.nCompImageBufferSize, (XnUChar*)pStreamData->pData, &pStreamData->nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		pStreamData->nTimestamp = FileFrameHeader.FrameProperties.nImageTimeStamp;
		pStreamData->nFrameID = FileFrameHeader.FrameProperties.nImageFrameID;

		pPackedBuffer += PackedStreamHeader.nCompImageBufferSize;

		pStream->NewDataAvailable(pStreamData->nTimestamp, pStreamData->nFrameID);
	}

	// we do not support MISC
	pPackedBuffer += PackedStreamHeader.nCompMiscBufferSize;

	// Audio
	if (XN_STATUS_OK == FindStream(XN_STREAM_NAME_AUDIO, &pStreamHolder))
	{
		XnStreamReaderStream* pStream = (XnStreamReaderStream*)pStreamHolder->GetStream();

		XnStreamData* pStreamData = pStream->GetStreamData();

		// check size
		nRetVal = XnStreamDataCheckSize(pStreamData, pStream->GetRequiredDataSize());
		XN_IS_STATUS_OK(nRetVal);

		pStreamData->nDataSize = pStreamHolder->GetStream()->GetRequiredDataSize();
		nRetVal = pStreamHolder->GetCodec()->Decompress(pPackedBuffer, PackedStreamHeader.nCompAudioBufferSize, (XnUChar*)pStreamData->pData, &pStreamData->nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		pStreamData->nTimestamp = FileFrameHeader.FrameProperties.nAudioTimeStamp;
		pStreamData->nFrameID = 0;

		pPackedBuffer += PackedStreamHeader.nCompAudioBufferSize;

		pStream->NewDataAvailable(pStreamData->nTimestamp, pStreamData->nFrameID);
	}

	// Increase the file frame position
	m_pBCData->nFramePos++;

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::BCDestroy()
{
	if (m_pBCData != NULL)
	{
		xnOSFreeAligned(m_pBCData->pPackedStreamBuffer);
		xnOSFree(m_pBCData);
	}

	return XN_STATUS_OK;
}
