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
#include "XnDeviceSensorInit.h"
#include "XnSensorImageStream.h"
#include "XnSensor.h"
#include "XnBayerImageProcessor.h"
#include "XnUncompressedBayerProcessor.h"
#include "XnPSCompressedImageProcessor.h"
#include "XnJpegImageProcessor.h"
#include "XnUncompressedYUVImageProcessor.h"
#include "XnUncompressedYUVtoRGBImageProcessor.h"
#include "YUV.h"
#include "Bayer.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_IMAGE_STREAM_DEFAULT_FPS				30
#define XN_IMAGE_STREAM_DEFAULT_RESOLUTION		XN_RESOLUTION_QVGA
#define XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT	XN_IO_IMAGE_FORMAT_YUV422
#define XN_IMAGE_STREAM_DEFAULT_OUTPUT_FORMAT	XN_OUTPUT_FORMAT_RGB24
#define XN_IMAGE_STREAM_DEFAULT_FLICKER			0
#define XN_IMAGE_STREAM_DEFAULT_QUALITY			10

//---------------------------------------------------------------------------
// XnSensorImageStream class
//---------------------------------------------------------------------------
XnSensorImageStream::XnSensorImageStream(const XnChar* StreamName, XnSensorObjects* pObjects, XnUInt32 nBufferCount) : 
	XnImageStream(StreamName, FALSE),
	m_Helper(pObjects),
	m_BufferPool(nBufferCount, StreamName, GetMaxBufferSize(m_Helper.GetFirmwareVersion())),
	m_SharedBufferName(XN_STREAM_PROPERTY_SHARED_BUFFER_NAME, m_BufferPool.GetSharedMemoryName()),
	m_InputFormat(XN_STREAM_PROPERTY_INPUT_FORMAT, XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT),
	m_AntiFlicker(XN_STREAM_PROPERTY_FLICKER, XN_IMAGE_STREAM_DEFAULT_FLICKER),
	m_ImageQuality(XN_STREAM_PROPERTY_QUALITY, XN_IMAGE_STREAM_DEFAULT_QUALITY),
	m_FirmwareMirror("FirmwareMirror", FALSE, StreamName),
	m_FirmwareCropSizeX("FirmwareCropSizeX", 0, StreamName),
	m_FirmwareCropSizeY("FirmwareCropSizeY", 0, StreamName),
	m_FirmwareCropOffsetX("FirmwareCropOffsetX", 0, StreamName),
	m_FirmwareCropOffsetY("FirmwareCropOffsetY", 0, StreamName),
	m_FirmwareCropEnabled("FirmwareCropEnabled", FALSE, StreamName),
	m_ActualRead(XN_STREAM_PROPERTY_ACTUAL_READ_DATA, FALSE)
{
	m_ActualRead.UpdateSetCallback(SetActualReadCallback, this);
}

XnStatus XnSensorImageStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = SetBufferPool(&m_BufferPool);
	XN_IS_STATUS_OK(nRetVal);

	// init base
	nRetVal = XnImageStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	m_InputFormat.UpdateSetCallback(SetInputFormatCallback, this);
	m_AntiFlicker.UpdateSetCallback(SetAntiFlickerCallback, this);
	m_ImageQuality.UpdateSetCallback(SetImageQualityCallback, this);

	// add properties
	XN_VALIDATE_ADD_PROPERTIES(this, &m_InputFormat, &m_AntiFlicker, &m_ImageQuality, 
		&m_SharedBufferName, &m_ActualRead);

	// set base properties default values
	nRetVal = ResolutionProperty().UnsafeUpdateValue(XN_IMAGE_STREAM_DEFAULT_RESOLUTION);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FPSProperty().UnsafeUpdateValue(XN_IMAGE_STREAM_DEFAULT_FPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = OutputFormatProperty().UnsafeUpdateValue(XN_IMAGE_STREAM_DEFAULT_OUTPUT_FORMAT);
	XN_IS_STATUS_OK(nRetVal);

	// init helper
	nRetVal = m_Helper.Init(this, this);
	XN_IS_STATUS_OK(nRetVal);

	// data processor
	nRetVal = m_Helper.RegisterDataProcessorProperty(m_InputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.RegisterDataProcessorProperty(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.RegisterDataProcessorProperty(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::Free()
{
	m_Helper.Free();
	XnImageStream::Free();
	return (XN_STATUS_OK);
}

XnUInt32 XnSensorImageStream::GetMaxBufferSize(XnFWVer version)
{
	if (version >= XN_SENSOR_FW_VER_5_3)
	{
		// max resolution is only SXGA
		return (XN_SXGA_X_RES * XN_SXGA_Y_RES * sizeof(XnRGB24Pixel));
	}
	else
	{
		// max resolution is UXGA
		return (XN_UXGA_X_RES * XN_UXGA_Y_RES * sizeof(XnRGB24Pixel));
	}
}

XnStatus XnSensorImageStream::MapPropertiesToFirmware()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_InputFormat, GetFirmwareParams()->m_ImageFormat, FALSE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(ResolutionProperty(), GetFirmwareParams()->m_ImageResolution, FALSE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(FPSProperty(), GetFirmwareParams()->m_ImageFPS, FALSE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_AntiFlicker, GetFirmwareParams()->m_ImageFlickerDetection, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_ImageQuality, GetFirmwareParams()->m_ImageQuality, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareMirror, GetFirmwareParams()->m_ImageMirror, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropSizeX, GetFirmwareParams()->m_ImageCropSizeX, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropSizeY, GetFirmwareParams()->m_ImageCropSizeY, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetX, GetFirmwareParams()->m_ImageCropOffsetX, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetY, GetFirmwareParams()->m_ImageCropOffsetY, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropEnabled, GetFirmwareParams()->m_ImageCropEnabled, TRUE));

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::ValidateMode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validity checks
	XnIOImageFormats nInputFormat = (XnIOImageFormats)m_InputFormat.GetValue();
	XnOutputFormats nOutputFormat = GetOutputFormat();
	XnResolutions nResolution = GetResolution();

	// check resolution
	if ((nResolution == XN_RESOLUTION_UXGA || nResolution == XN_RESOLUTION_SXGA) && nInputFormat != XN_IO_IMAGE_FORMAT_BAYER)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "UXGA resolution is only supported with BAYER input!");
	}

	// check output format
	if (nOutputFormat == XN_OUTPUT_FORMAT_GRAYSCALE8 && nInputFormat != XN_IO_IMAGE_FORMAT_BAYER)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Grayscale8 output requires BAYER input!");
	}
	else if (nOutputFormat == XN_OUTPUT_FORMAT_YUV422 && nInputFormat != XN_IO_IMAGE_FORMAT_YUV422 && nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "YUV output requires YUV input!");
	}

	// check input format
	if (nInputFormat == XN_IO_IMAGE_FORMAT_BAYER && nResolution != XN_RESOLUTION_UXGA && nResolution != XN_RESOLUTION_SXGA)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "BAYER input requires SXGA/UXGA resolution!");
	}
	else if (nInputFormat == XN_IO_IMAGE_FORMAT_JPEG && nOutputFormat != XN_OUTPUT_FORMAT_RGB24)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Jpeg input is only supported for RGB24 output!");
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::ConfigureStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp);

	nRetVal = SetActualRead(TRUE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ValidateMode();
	XN_IS_STATUS_OK(nRetVal);

	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_InputFormat));
	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(ResolutionProperty()));
	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(FPSProperty()));
	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_AntiFlicker));

	// image quality is only relevant for JPEG
	if (m_InputFormat.GetValue() == XN_IO_IMAGE_FORMAT_JPEG)
	{
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_ImageQuality));
	}

	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareMirror));

	if (GetResolution() != XN_RESOLUTION_UXGA && GetResolution() != XN_RESOLUTION_SXGA)
	{
		nRetVal = m_Helper.GetCmosInfo()->SetCmosConfig(XN_CMOS_TYPE_IMAGE, GetResolution(), GetFPS());
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetActualRead(XnBool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_ActualRead.GetValue() != bRead)
	{
		if (bRead)
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Creating USB image read thread...");
			nRetVal = xnUSBInitReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp, GetHelper()->GetPrivateData()->pSpecificImageUsb->nChunkReadBytes, XN_SENSOR_USB_IMAGE_BUFFERS, XN_SENSOR_READ_THREAD_TIMEOUT, XnDeviceSensorProtocolUsbEpCb, GetHelper()->GetPrivateData()->pSpecificImageUsb);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Shutting down USB image read thread...");
			xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp);
		}

		nRetVal = m_ActualRead.UnsafeUpdateValue(bRead);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::OpenStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_COLOR);
	XN_IS_STATUS_OK(nRetVal);

	// Cropping
	if (m_FirmwareCropEnabled.GetValue() == TRUE)
	{
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropSizeX));
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropSizeY));
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropOffsetX));
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropOffsetY));
	}
	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropEnabled));

	nRetVal = XnImageStream::Open();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::CloseStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_OFF);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::Close();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SetActualRead(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetOutputFormat(XnOutputFormats nOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nOutputFormat)
	{
	case XN_OUTPUT_FORMAT_GRAYSCALE8:
	case XN_OUTPUT_FORMAT_YUV422:
	case XN_OUTPUT_FORMAT_RGB24:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unsupported image output format: %d", nOutputFormat);
	}

	nRetVal = m_Helper.BeforeSettingDataProcessorProperty();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::SetOutputFormat(nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingDataProcessorProperty();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetMirror(XnBool bIsMirrored)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// set firmware mirror
	XnBool bFirmwareMirror = (bIsMirrored == TRUE && m_Helper.GetFirmwareVersion() >= XN_SENSOR_FW_VER_5_0);

	xnOSEnterCriticalSection(GetLock());

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareMirror, bFirmwareMirror);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSLeaveCriticalSection(GetLock());
		return (nRetVal);
	}

	// update prop
	nRetVal = XnImageStream::SetMirror(bIsMirrored);
	xnOSLeaveCriticalSection(GetLock());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetFPS(XnUInt32 nFPS)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.BeforeSettingFirmwareParam(FPSProperty(), nFPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::SetFPS(nFPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(FPSProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetResolution(XnResolutions nResolution)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nResolution)
	{
	case XN_RESOLUTION_QVGA:
	case XN_RESOLUTION_VGA:
	case XN_RESOLUTION_SXGA:
	case XN_RESOLUTION_UXGA:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unknown resolution: %u", nResolution);
	}

	nRetVal = m_Helper.BeforeSettingFirmwareParam(ResolutionProperty(), nResolution);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::SetResolution(nResolution);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetInputFormat(XnIOImageFormats nInputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validity checks
	switch (nInputFormat)
	{
	case XN_IO_IMAGE_FORMAT_YUV422:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422:
	case XN_IO_IMAGE_FORMAT_JPEG:
	case XN_IO_IMAGE_FORMAT_BAYER:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unknown image input format: %d", nInputFormat);
	}

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_InputFormat, nInputFormat);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetAntiFlicker(XnUInt32 nFrequency)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_Helper.SimpleSetFirmwareParam(m_AntiFlicker, nFrequency);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetImageQuality(XnUInt32 nQuality)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check relevance
	if (m_InputFormat.GetValue() != XN_IO_IMAGE_FORMAT_JPEG)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_UNSUPPORTED_PARAMETER, XN_MASK_DEVICE_SENSOR, "Image quality is only supported when input format is JPEG");
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetCropping(const XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateCropping(pCropping);
	XN_IS_STATUS_OK(nRetVal);

	xnOSEnterCriticalSection(GetLock());

	if (m_Helper.GetFirmwareVersion() > XN_SENSOR_FW_VER_3_0)
	{
		nRetVal = m_Helper.StartFirmwareTransaction();
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}

		if (pCropping->bEnabled)
		{
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropSizeX, pCropping->nXSize);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropSizeY, pCropping->nYSize);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropOffsetX, pCropping->nXOffset);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropOffsetY, pCropping->nYOffset);
		}

		if (nRetVal == XN_STATUS_OK)
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropEnabled, pCropping->bEnabled);

		if (nRetVal != XN_STATUS_OK)
		{
			m_Helper.RollbackFirmwareTransaction();
			m_Helper.UpdateFromFirmware(m_FirmwareCropEnabled);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetY);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeY);
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}

		nRetVal = m_Helper.CommitFirmwareTransactionAsBatch();
		if (nRetVal != XN_STATUS_OK)
		{
			m_Helper.UpdateFromFirmware(m_FirmwareCropEnabled);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetY);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeY);
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}
	}

	nRetVal = XnImageStream::SetCropping(pCropping);
	xnOSLeaveCriticalSection(GetLock());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::PostProcessFrame(XnStreamData* pFrameData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	m_Helper.GetFPS()->MarkOutputImage(pFrameData->nFrameID, pFrameData->nTimestamp);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::ReallocTripleFrameBuffer()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (IsOpen())
	{
		// before actually replacing buffer, lock the processor (so it will not continue to 
		// use old buffer)
		nRetVal = m_Helper.GetFirmware()->GetStreams()->LockStreamProcessor(GetType(), this);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = XnImageStream::ReallocTripleFrameBuffer();
	if (nRetVal != XN_STATUS_OK)
	{
		m_Helper.GetFirmware()->GetStreams()->UnlockStreamProcessor(GetType(), this);
		return (nRetVal);
	}

	if (IsOpen())
	{
		nRetVal = m_Helper.GetFirmware()->GetStreams()->UnlockStreamProcessor(GetType(), this);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::CropImpl(XnStreamData* pStreamOutput, const XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// if firmware cropping is disabled, crop
	if (m_FirmwareCropEnabled.GetValue() == FALSE)
	{
		nRetVal = XnImageStream::CropImpl(pStreamOutput, pCropping);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::Mirror(XnStreamData* pStreamOutput) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	// only perform mirror if it's our job. if mirror is performed by FW, we don't need to do anything.
	if (m_FirmwareMirror.GetValue() == FALSE)
	{
		nRetVal = XnImageStream::Mirror(pStreamOutput);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnUInt32 XnSensorImageStream::CalculateExpectedSize()
{
	XnUInt32 nExpectedImageBufferSize = GetXRes() * GetYRes();

	// when cropping is turned on, actual IR size is smaller
	const XnCropping* pCropping = GetCropping();
	if (pCropping->bEnabled)
	{
		nExpectedImageBufferSize = pCropping->nXSize * pCropping->nYSize;
	}

	switch (m_InputFormat.GetValue())
	{
	case XN_IO_IMAGE_FORMAT_YUV422:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422:
		// in YUV each pixel is represented in 2 bytes (actually 2 pixels are represented by 4 bytes)
		nExpectedImageBufferSize *= 2;
		break;
	case XN_IO_IMAGE_FORMAT_BAYER:
		// each pixel is one byte.
		break;
	case XN_IO_IMAGE_FORMAT_JPEG:
		// image should be in RGB now - 3 bytes a pixel
		nExpectedImageBufferSize *= 3;
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DEVICE_SENSOR, "Does not know to calculate expected size for input format %d", m_InputFormat.GetValue());
	}

	return nExpectedImageBufferSize;
}

XnStatus XnSensorImageStream::CreateDataProcessor(XnDataProcessor** ppProcessor)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnStreamProcessor* pNew;

	switch (m_InputFormat.GetValue())
	{
	case XN_IO_IMAGE_FORMAT_BAYER:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnBayerImageProcessor, this, &m_Helper);
		break;
	case XN_IO_IMAGE_FORMAT_YUV422:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnPSCompressedImageProcessor, this, &m_Helper);
		break;
	case XN_IO_IMAGE_FORMAT_JPEG:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnJpegImageProcessor, this, &m_Helper);
		break;
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422:
		if (GetOutputFormat() == XN_OUTPUT_FORMAT_YUV422)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedYUVImageProcessor, this, &m_Helper);
		}
		else if (GetOutputFormat() == XN_OUTPUT_FORMAT_RGB24)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedYUVtoRGBImageProcessor, this, &m_Helper);
		}
		else
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DEVICE_SENSOR, "invalid ouput format %d!", GetOutputFormat());
		}
		break;
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedBayerProcessor, this, &m_Helper);
		break;
	default:
		return XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT;
	}

	*ppProcessor = pNew;

	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetInputFormatCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetInputFormat((XnIOImageFormats)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetAntiFlickerCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetAntiFlicker((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetImageQualityCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetImageQuality((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetActualRead(nValue == TRUE);
}

