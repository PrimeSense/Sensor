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
#include "XnJpegToRGBImageProcessor.h"
#include "XnUncompressedYUVImageProcessor.h"
#include "XnUncompressedYUVtoRGBImageProcessor.h"
#include "YUV.h"
#include "Bayer.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// XnSensorImageStream class
//---------------------------------------------------------------------------
XnSensorImageStream::XnSensorImageStream(const XnChar* StreamName, XnSensorObjects* pObjects) : 
	XnImageStream(StreamName, FALSE),
	m_Helper(pObjects),
	m_InputFormat(XN_STREAM_PROPERTY_INPUT_FORMAT, XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT),
	m_AntiFlicker(XN_STREAM_PROPERTY_FLICKER, XN_IMAGE_STREAM_DEFAULT_FLICKER),
	m_ImageQuality(XN_STREAM_PROPERTY_QUALITY, XN_IMAGE_STREAM_DEFAULT_QUALITY),
	m_Brightness(XN_STREAM_PROPERTY_BRIGHTNESS, XN_IMAGE_STREAM_DEFAULT_BRIGHTNESS),
	m_Contrast(XN_STREAM_PROPERTY_CONTRAST, XN_IMAGE_STREAM_DEFAULT_CONTRAST),
	m_Saturation(XN_STREAM_PROPERTY_SATURATION, XN_IMAGE_STREAM_DEFAULT_SATURATION),
	m_Sharpness(XN_STREAM_PROPERTY_SHARPNESS, XN_IMAGE_STREAM_DEFAULT_SHARPNESS),
	m_ColorTemperature(XN_STREAM_PROPERTY_COLOR_TEMPERATURE, XN_IMAGE_STREAM_DEFAULT_COLOR_TEMP),
	m_BackLightCompensation(XN_STREAM_PROPERTY_BACKLIGHT_COMPENSATION, XN_IMAGE_STREAM_DEFAULT_BACKLIGHT_COMP),
	m_Gain(XN_STREAM_PROPERTY_GAIN, XN_IMAGE_STREAM_DEFAULT_GAIN),
	m_Zoom(XN_STREAM_PROPERTY_ZOOM, XN_IMAGE_STREAM_DEFAULT_ZOOM),
	m_Exposure(XN_STREAM_PROPERTY_EXPOSURE, XN_IMAGE_STREAM_DEFAULT_EXPOSURE_BAR),
	m_AutoExposure(XN_STREAM_PROPERTY_AUTO_EXPOSURE, XN_IMAGE_STREAM_DEFAULT_AUTO_EXPOSURE),
	m_Pan(XN_STREAM_PROPERTY_PAN, XN_IMAGE_STREAM_DEFAULT_PAN),
	m_Tilt(XN_STREAM_PROPERTY_TILT, XN_IMAGE_STREAM_DEFAULT_TILT),
	m_LowLightCompensation(XN_STREAM_PROPERTY_LOW_LIGHT_COMPENSATION, XN_IMAGE_STREAM_DEFAULT_LOW_LIGHT_COMP),
	m_CroppingMode(XN_STREAM_PROPERTY_CROPPING_MODE, XN_CROPPING_MODE_NORMAL),
	m_FirmwareMirror("FirmwareMirror", FALSE, StreamName),
	m_FirmwareCropSizeX("FirmwareCropSizeX", 0, StreamName),
	m_FirmwareCropSizeY("FirmwareCropSizeY", 0, StreamName),
	m_FirmwareCropOffsetX("FirmwareCropOffsetX", 0, StreamName),
	m_FirmwareCropOffsetY("FirmwareCropOffsetY", 0, StreamName),
	m_FirmwareCropMode("FirmwareCropMode", XN_FIRMWARE_CROPPING_MODE_DISABLED, StreamName),
	m_FirmwareColorTemperature("FirmwareWhiteBalance", 0, StreamName),
	m_FirmwareAutoWhiteBalance("FirmwareAutoWhiteBalance", TRUE, StreamName),
	m_FirmwareExposure("FirmwareExposure", 0, StreamName),
	m_FirmwareAutoExposure("FirmwareAutoExposure", TRUE, StreamName),
	m_ActualRead(XN_STREAM_PROPERTY_ACTUAL_READ_DATA, FALSE)
{
	m_ActualRead.UpdateSetCallback(SetActualReadCallback, this);
}

static XnStatus AddSupportedMode(XnArray<XnCmosPreset>& modes, XnUInt16 nInputFormat, XnUInt16 nResolution, XnUInt16 nFPS)
{
	XnCmosPreset preset = { nInputFormat, nResolution, nFPS };
	return modes.AddLast(preset);
}

XnStatus XnSensorImageStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

// 	nRetVal = SetBufferPool(&m_BufferPool);
// 	XN_IS_STATUS_OK(nRetVal);

	// init base
	nRetVal = XnImageStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	m_InputFormat.UpdateSetCallback(SetInputFormatCallback, this);
	m_AntiFlicker.UpdateSetCallback(SetAntiFlickerCallback, this);
	m_ImageQuality.UpdateSetCallback(SetImageQualityCallback, this);
	m_Brightness.UpdateSetCallbackToDefault();
	m_Contrast.UpdateSetCallbackToDefault();
	m_Saturation.UpdateSetCallbackToDefault();
	m_Zoom.UpdateSetCallbackToDefault();
	m_Pan.UpdateSetCallbackToDefault();
	m_Tilt.UpdateSetCallbackToDefault();
	m_Sharpness.UpdateSetCallback(SetSharpnessCallback, this);
	m_ColorTemperature.UpdateSetCallback(SetColorTemperatureCallback, this);
	m_BackLightCompensation.UpdateSetCallback(SetBacklightCompensationCallback, this);
	m_Gain.UpdateSetCallback(SetGainCallback, this);
	m_Exposure.UpdateSetCallback(SetExposureCallback, this);
	m_AutoExposure.UpdateSetCallback(SetAutoExposureCallback, this);
	m_LowLightCompensation.UpdateSetCallback(SetLowLightCompensationCallback, this);
	m_CroppingMode.UpdateSetCallback(SetCroppingModeCallback, this);

	// add properties
	XN_VALIDATE_ADD_PROPERTIES(this, &m_InputFormat, &m_AntiFlicker, &m_ImageQuality, 
		&m_Brightness, &m_Contrast, &m_Saturation, &m_Sharpness, &m_CroppingMode, 
		&m_ColorTemperature, &m_BackLightCompensation, &m_Gain, &m_Zoom, 
		&m_Exposure, &m_AutoExposure, &m_Pan, &m_Tilt, &m_LowLightCompensation, &m_ActualRead);

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

	// register supported modes
	if (m_Helper.GetFirmware()->GetInfo()->bGetPresetsSupported)
	{
		// ask the firmware
		const XnUInt32 nAllocSize = 100;
		XnUInt32 nCount = nAllocSize;
		XnCmosPreset aSupportedModes[nAllocSize];
		nRetVal = XnHostProtocolGetCmosPresets(m_Helper.GetPrivateData(), XN_CMOS_TYPE_IMAGE, aSupportedModes, nCount);
		XN_IS_STATUS_OK(nRetVal);

		if (nCount == 0)
		{
			xnLogError(XN_MASK_DEVICE_SENSOR, "Device does not support any image mode!");
			return XN_STATUS_DEVICE_UNSUPPORTED_PARAMETER;
		}

		nRetVal = AddSupportedModes(aSupportedModes, nCount);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// Firmware doesn't support getting presets. Instead, we'll need to register presets
		// we know according to firmware version
		XnCmosPreset* pSupportedModes; 
		XnUInt8 nSupportedModes;
		XnUInt64 nDefaultInputFormat;

		if (m_Helper.GetPrivateData()->pSpecificImageUsb->pUsbConnection->bIsISO)
		{
			pSupportedModes = m_Helper.GetPrivateData()->FWInfo.imageIsoModes.GetData();
			nSupportedModes = m_Helper.GetPrivateData()->FWInfo.imageIsoModes.GetSize();
		}
		else
		{
			pSupportedModes = m_Helper.GetPrivateData()->FWInfo.imageBulkModes.GetData();
			nSupportedModes = m_Helper.GetPrivateData()->FWInfo.imageBulkModes.GetSize();
		}

		nRetVal = AddSupportedModes(pSupportedModes, nSupportedModes);
		XN_IS_STATUS_OK(nRetVal);
	}

	// check if our current (default) configuration is valid
	XnUInt16 nValidInputFormat = XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT;
	XnBool bModeFound = FALSE;

	const XnArray<XnCmosPreset>& aSupportedModes = GetSupportedModes();

	for (XnUInt32 i = 0; i < aSupportedModes.GetSize(); ++i)
	{
		if (aSupportedModes[i].nResolution == XN_IMAGE_STREAM_DEFAULT_RESOLUTION &&
			aSupportedModes[i].nFPS == XN_IMAGE_STREAM_DEFAULT_FPS)
		{
			// found
			if (!bModeFound)
			{
				bModeFound = TRUE;
				nValidInputFormat = aSupportedModes[i].nFormat;
			}

			if (aSupportedModes[i].nFormat == XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT)
			{
				nValidInputFormat = XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT;
				break;					
			}
		}
	}

	if (!bModeFound)
	{
		xnLogWarning(XN_MASK_DEVICE_SENSOR, "Default mode (res + FPS) is not supported by device. Changing defaults...");

		nRetVal = ResolutionProperty().UnsafeUpdateValue(aSupportedModes[0].nResolution);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = FPSProperty().UnsafeUpdateValue(aSupportedModes[0].nFPS);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = m_InputFormat.UnsafeUpdateValue(aSupportedModes[0].nFormat);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// just update input format
		nRetVal = m_InputFormat.UnsafeUpdateValue(nValidInputFormat);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::Free()
{
	m_Helper.Free();
	XnImageStream::Free();
	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::MapPropertiesToFirmware()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.MapFirmwareProperty(m_InputFormat, GetFirmwareParams()->m_ImageFormat, FALSE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(ResolutionProperty(), GetFirmwareParams()->m_ImageResolution, FALSE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(FPSProperty(), GetFirmwareParams()->m_ImageFPS, FALSE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_AntiFlicker, GetFirmwareParams()->m_ImageFlickerDetection, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_ImageQuality, GetFirmwareParams()->m_ImageQuality, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareMirror, GetFirmwareParams()->m_ImageMirror, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropSizeX, GetFirmwareParams()->m_ImageCropSizeX, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropSizeY, GetFirmwareParams()->m_ImageCropSizeY, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetX, GetFirmwareParams()->m_ImageCropOffsetX, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetY, GetFirmwareParams()->m_ImageCropOffsetY, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropMode, GetFirmwareParams()->m_ImageCropMode, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_Sharpness, GetFirmwareParams()->m_ImageSharpness, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareAutoWhiteBalance, GetFirmwareParams()->m_ImageAutoWhiteBalance, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareColorTemperature, GetFirmwareParams()->m_ImageColorTemperature, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_BackLightCompensation, GetFirmwareParams()->m_ImageBacklightCompensation, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_Gain, GetFirmwareParams()->m_ImageGain, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareExposure, GetFirmwareParams()->m_ImageExposureBar, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareAutoExposure, GetFirmwareParams()->m_ImageAutoExposure, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_LowLightCompensation, GetFirmwareParams()->m_ImageLowLightCompensation, TRUE);
	XN_IS_STATUS_OK(nRetVal);;

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::ValidateMode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validity checks
	XnIOImageFormats nInputFormat = (XnIOImageFormats)m_InputFormat.GetValue();
	XnOutputFormats nOutputFormat = GetOutputFormat();
	XnResolutions nResolution = GetResolution();
	XnUInt32 nFPS = GetFPS();

	// check that input format matches output format
	switch (nOutputFormat)
	{
	case XN_OUTPUT_FORMAT_RGB24:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_YUV422 &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422 &&
			nInputFormat != XN_IO_IMAGE_FORMAT_BAYER &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to RGB24!", nInputFormat);
		}
		break;
	case XN_OUTPUT_FORMAT_YUV422:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_YUV422 &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to YUV422!", nInputFormat);
		}
		break;
	case XN_OUTPUT_FORMAT_JPEG:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_JPEG)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to JPEG!", nInputFormat);
		}
		break;
	case XN_OUTPUT_FORMAT_GRAYSCALE8:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_GRAY8 &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER &&
			nInputFormat != XN_IO_IMAGE_FORMAT_BAYER)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to Gray8!", nInputFormat);
		}
		break;
	default:
		// we shouldn't have reached here. Theres a check at SetOutputFormat.
		XN_ASSERT(FALSE);
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unsupported image output format: %d!", nOutputFormat);
	}

	// now check that mode exists
	XnCmosPreset preset = { (XnUInt16)nInputFormat, (XnUInt16)nResolution, (XnUInt16)nFPS };
	nRetVal = ValidateSupportedMode(preset);
	XN_IS_STATUS_OK(nRetVal);

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

	nRetVal = m_Helper.ConfigureFirmware(m_InputFormat);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(FPSProperty());
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(m_AntiFlicker);
	XN_IS_STATUS_OK(nRetVal);;

	// image quality is only relevant for JPEG
	if (m_InputFormat.GetValue() == XN_IO_IMAGE_FORMAT_JPEG)
	{
		nRetVal = m_Helper.ConfigureFirmware(m_ImageQuality);
		XN_IS_STATUS_OK(nRetVal);;
	}

	nRetVal = m_Helper.ConfigureFirmware(m_FirmwareMirror);
	XN_IS_STATUS_OK(nRetVal);;

	if (GetResolution() != XN_RESOLUTION_UXGA && GetResolution() != XN_RESOLUTION_SXGA)
	{
		nRetVal = m_Helper.GetCmosInfo()->SetCmosConfig(XN_CMOS_TYPE_IMAGE, GetResolution(), GetFPS());
		XN_IS_STATUS_OK(nRetVal);
	}

	if (m_Helper.GetFirmwareVersion() >= XN_SENSOR_FW_VER_5_8)
	{
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareAutoExposure);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareExposure);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareAutoWhiteBalance);
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
			XnSpecificUsbDevice* pUSB = GetHelper()->GetPrivateData()->pSpecificImageUsb;
			nRetVal = xnUSBInitReadThread(pUSB->pUsbConnection->UsbEp, pUSB->nChunkReadBytes, XN_SENSOR_USB_IMAGE_BUFFERS, pUSB->nTimeout, XnDeviceSensorProtocolUsbEpCb, pUSB);
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
	if (m_FirmwareCropMode.GetValue() != XN_FIRMWARE_CROPPING_MODE_DISABLED)
	{
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropSizeX);
		XN_IS_STATUS_OK(nRetVal);;
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropSizeY);
		XN_IS_STATUS_OK(nRetVal);;
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropOffsetX);
		XN_IS_STATUS_OK(nRetVal);;
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropOffsetY);
		XN_IS_STATUS_OK(nRetVal);;
	}
	nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropMode);
	XN_IS_STATUS_OK(nRetVal);;

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
	case XN_OUTPUT_FORMAT_JPEG:
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

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareMirror, (XnUInt16)bFirmwareMirror);
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

	nRetVal = m_Helper.BeforeSettingFirmwareParam(FPSProperty(), (XnUInt16)nFPS);
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

	nRetVal = m_Helper.BeforeSettingFirmwareParam(ResolutionProperty(), (XnUInt16)nResolution);
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

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_InputFormat, (XnUInt16)nInputFormat);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetAntiFlicker(XnUInt32 nFrequency)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_Helper.SimpleSetFirmwareParam(m_AntiFlicker, (XnUInt16)nFrequency);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetImageQuality(XnUInt32 /*nQuality*/)
{
	// check relevance
	if (m_InputFormat.GetValue() != XN_IO_IMAGE_FORMAT_JPEG)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_UNSUPPORTED_PARAMETER, XN_MASK_DEVICE_SENSOR, "Image quality is only supported when input format is JPEG");
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetCroppingImpl(const XnCropping* pCropping, XnCroppingMode mode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnFirmwareCroppingMode firmwareMode = m_Helper.GetFirmwareCroppingMode(mode, pCropping->bEnabled);

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
		{
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropMode, (XnUInt16)firmwareMode);
		}

		if (nRetVal != XN_STATUS_OK)
		{
			m_Helper.RollbackFirmwareTransaction();
			m_Helper.UpdateFromFirmware(m_FirmwareCropMode);
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
			m_Helper.UpdateFromFirmware(m_FirmwareCropMode);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetY);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeY);
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}
	}

	nRetVal = m_CroppingMode.UnsafeUpdateValue(mode);
	XN_ASSERT(nRetVal == XN_STATUS_OK);

	nRetVal = XnImageStream::SetCropping(pCropping);
	xnOSLeaveCriticalSection(GetLock());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetCropping(const XnCropping* pCropping)
{
	return SetCroppingImpl(pCropping, (XnCroppingMode)m_CroppingMode.GetValue());
}

XnStatus XnSensorImageStream::SetCroppingMode(XnCroppingMode mode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (mode)
	{
	case XN_CROPPING_MODE_NORMAL:
	case XN_CROPPING_MODE_INCREASED_FPS:
	case XN_CROPPING_MODE_SOFTWARE_ONLY:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Bad cropping mode: %u", mode);
	}

	return SetCroppingImpl(GetCropping(), mode);
}

XnStatus XnSensorImageStream::SetSharpness( XnInt32 nValue )
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_Sharpness, (XnUInt16)nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetColorTemperature( XnInt32 nValue )
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnBool bIsAuto = (nValue == XN_AUTO_CONTROL);

	if (m_Helper.GetFirmwareVersion() < XN_SENSOR_FW_VER_5_8)
	{
		XnUInt16 nCmosRegValue; 

		nRetVal = XnHostProtocolSetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0xf0, 1);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnHostProtocolGetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0x6, nCmosRegValue);
		XN_IS_STATUS_OK(nRetVal);

		if(bIsAuto)
		{
			nCmosRegValue |= 0x2;	
		}
		else
		{		
			nCmosRegValue &= ~0x2;
		}	

		nRetVal = XnHostProtocolSetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0x6, nCmosRegValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareAutoWhiteBalance, (XnUInt16)bIsAuto);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (!bIsAuto)
	{
		if (nValue != XN_PAUSE_AUTO_CONTROL)
		{
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareColorTemperature, (XnUInt16)nValue);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	nRetVal = m_ColorTemperature.UnsafeUpdateValue(nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetBacklightCompensation( XnInt32 nValue )
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_BackLightCompensation, (XnUInt16)nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetGain( XnInt32 nValue )
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_Gain, (XnUInt16)nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetExposure( XnInt32 nValue )
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnBool bIsAuto = (nValue == XN_AUTO_CONTROL);

	if (m_Helper.GetFirmwareVersion() < XN_SENSOR_FW_VER_5_8)
	{
		// Implement me...
	}
	else
	{
		nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareExposure, (XnUInt16)nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = m_Exposure.UnsafeUpdateValue(nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetAutoExposure( XnInt32 nValue )
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_Helper.GetFirmwareVersion() < XN_SENSOR_FW_VER_5_8)
	{
		// Implement me...
	}
	else
	{
		nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareAutoExposure, (XnUInt16)nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = m_AutoExposure.UnsafeUpdateValue(nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetLowLightCompensation( XnInt32 nValue )
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_LowLightCompensation, (XnUInt16)nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::PostProcessFrame(XnStreamData* pFrameData)
{
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
	if (m_FirmwareCropMode.GetValue() == XN_FIRMWARE_CROPPING_MODE_DISABLED)
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

	XnFrameBufferManager* pBufferManager;
	nRetVal = GetTripleBuffer(&pBufferManager);
	XN_IS_STATUS_OK(nRetVal);

	XnStreamProcessor* pNew;

	switch (m_InputFormat.GetValue())
	{
	case XN_IO_IMAGE_FORMAT_BAYER:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnBayerImageProcessor, this, &m_Helper, pBufferManager);
		break;
	case XN_IO_IMAGE_FORMAT_YUV422:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnPSCompressedImageProcessor, this, &m_Helper, pBufferManager);
		break;
	case XN_IO_IMAGE_FORMAT_JPEG:
		if (GetOutputFormat() == XN_OUTPUT_FORMAT_JPEG)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnJpegImageProcessor, this, &m_Helper, pBufferManager);
		}
		else if (GetOutputFormat() == XN_OUTPUT_FORMAT_RGB24)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnJpegToRGBImageProcessor, this, &m_Helper, pBufferManager);
		}
		else
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "invalid output format %d!", GetOutputFormat());
		}
		break;
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422:
		if (GetOutputFormat() == XN_OUTPUT_FORMAT_YUV422)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedYUVImageProcessor, this, &m_Helper, pBufferManager);
		}
		else if (GetOutputFormat() == XN_OUTPUT_FORMAT_RGB24)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedYUVtoRGBImageProcessor, this, &m_Helper, pBufferManager);
		}
		else
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "invalid output format %d!", pBufferManager);
		}
		break;
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedBayerProcessor, this, &m_Helper, pBufferManager);
		break;
	default:
		return XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT;
	}

	*ppProcessor = pNew;

	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetInputFormatCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetInputFormat((XnIOImageFormats)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetAntiFlickerCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetAntiFlicker((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetImageQualityCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetImageQuality((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetActualReadCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetActualRead(nValue == TRUE);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetSharpnessCallback( XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie )
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetSharpness((XnInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetColorTemperatureCallback( XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie )
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetColorTemperature((XnInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetBacklightCompensationCallback( XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie )
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetBacklightCompensation((XnInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetGainCallback( XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie )
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetGain((XnInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetLowLightCompensationCallback( XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie )
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetLowLightCompensation((XnInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetExposureCallback( XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie )
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetExposure((XnInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetAutoExposureCallback( XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie )
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetAutoExposure((XnInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetCroppingModeCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensorImageStream* pStream = (XnSensorImageStream*)pCookie;
	return pStream->SetCroppingMode((XnCroppingMode)nValue);
}

