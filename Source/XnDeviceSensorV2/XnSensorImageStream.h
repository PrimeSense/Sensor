/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
#ifndef __XN_SENSOR_IMAGE_STREAM_H__
#define __XN_SENSOR_IMAGE_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnImageStream.h>
#include "XnSensorStreamHelper.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_IMAGE_STREAM_DEFAULT_FPS				30
#define XN_IMAGE_STREAM_DEFAULT_RESOLUTION		XN_RESOLUTION_QVGA
#define XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT	XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422
#define XN_IMAGE_STREAM_DEFAULT_OUTPUT_FORMAT	XN_OUTPUT_FORMAT_RGB24
#define XN_IMAGE_STREAM_DEFAULT_FLICKER			0
#define XN_IMAGE_STREAM_DEFAULT_QUALITY			3
#define XN_IMAGE_STREAM_DEFAULT_BRIGHTNESS		128
#define XN_IMAGE_STREAM_DEFAULT_CONTRAST		32
#define XN_IMAGE_STREAM_DEFAULT_SATURATION		128
#define XN_IMAGE_STREAM_DEFAULT_SHARPNESS		32
#define XN_IMAGE_STREAM_DEFAULT_AWB				TRUE
#define XN_IMAGE_STREAM_DEFAULT_COLOR_TEMP		5000
#define XN_IMAGE_STREAM_DEFAULT_BACKLIGHT_COMP	1
#define XN_IMAGE_STREAM_DEFAULT_GAIN			128
#define XN_IMAGE_STREAM_DEFAULT_ZOOM			100
#define XN_IMAGE_STREAM_DEFAULT_AUTO_EXPOSURE	TRUE
#define XN_IMAGE_STREAM_DEFAULT_EXPOSURE_BAR	100
#define XN_IMAGE_STREAM_DEFAULT_PAN				0
#define XN_IMAGE_STREAM_DEFAULT_TILT			0
#define XN_IMAGE_STREAM_DEFAULT_LOW_LIGHT_COMP	TRUE

//---------------------------------------------------------------------------
// XnSensorImageStream class
//---------------------------------------------------------------------------
class XnSensorImageStream : public XnImageStream, public IXnSensorStream
{
public:
	XnSensorImageStream(const XnChar* StreamName, XnSensorObjects* pObjects);
	~XnSensorImageStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	XnStatus Free();
	XnStatus BatchConfig(const XnActualPropertiesHash& props) { return m_Helper.BatchConfig(props); }

	XnUInt32 CalculateExpectedSize();

	inline XnSensorStreamHelper* GetHelper() { return &m_Helper; }

	friend class XnImageProcessor;

protected:
	inline XnSensorFirmwareParams* GetFirmwareParams() const { return m_Helper.GetFirmware()->GetParams(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Open() { return m_Helper.Open(); }
	XnStatus Close() { return m_Helper.Close(); }
	XnStatus PostProcessFrame(XnStreamData* pFrameData);
	XnStatus ReallocTripleFrameBuffer();
	XnStatus CropImpl(XnStreamData* pStreamOutput, const XnCropping* pCropping);
	XnStatus Mirror(XnStreamData* pStreamOutput) const;
	XnStatus ConfigureStreamImpl();
	XnStatus OpenStreamImpl();
	XnStatus CloseStreamImpl();
	XnStatus CreateDataProcessor(XnDataProcessor** ppProcessor);
	XnStatus MapPropertiesToFirmware();
	void GetFirmwareStreamConfig(XnResolutions* pnRes, XnUInt32* pnFPS) { *pnRes = GetResolution(); *pnFPS = GetFPS(); }
	XnStatus WriteImpl(XnStreamData* /*pStreamData*/) { return XN_STATUS_DEVICE_UNSUPPORTED_MODE; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetOutputFormat(XnOutputFormats nOutputFormat);
	XnStatus SetMirror(XnBool bIsMirrored);
	XnStatus SetResolution(XnResolutions nResolution);
	XnStatus SetFPS(XnUInt32 nFPS);
	virtual XnStatus SetInputFormat(XnIOImageFormats nInputFormat);
	virtual XnStatus SetAntiFlicker(XnUInt32 nFrequency);
	virtual XnStatus SetImageQuality(XnUInt32 nQuality);
	XnStatus SetCropping(const XnCropping* pCropping);
	XnStatus SetActualRead(XnBool bRead);
	XnStatus SetSharpness(XnInt32 nValue);
	XnStatus SetColorTemperature(XnInt32 nValue);
	XnStatus SetBacklightCompensation(XnInt32 nValue);
	XnStatus SetGain(XnInt32 nValue);
	XnStatus SetExposure(XnInt32 nValue);
	XnStatus SetLowLightCompensation(XnInt32 nValue);

private:
	XnStatus ValidateMode();

	static XnStatus XN_CALLBACK_TYPE SetInputFormatCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAntiFlickerCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetImageQualityCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetSharpnessCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetColorTemperatureCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetBacklightCompensationCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetGainCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetExposureCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetLowLightCompensationCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnSensorStreamHelper m_Helper;
	XnActualIntProperty m_InputFormat;
	XnActualIntProperty m_AntiFlicker;
	XnActualIntProperty m_ImageQuality;

	XnActualIntProperty m_Brightness;
	XnActualIntProperty m_Contrast;
	XnActualIntProperty m_Saturation;
	XnActualIntProperty m_Sharpness;
	XnActualIntProperty m_ColorTemperature;
	XnActualIntProperty m_BackLightCompensation;
	XnActualIntProperty m_Gain;
	XnActualIntProperty m_Exposure;
	XnActualIntProperty m_Zoom;
	XnActualIntProperty m_Pan;
	XnActualIntProperty m_Tilt;
	XnActualIntProperty m_LowLightCompensation;

	XnActualIntProperty m_FirmwareMirror;

	XnActualIntProperty m_FirmwareCropSizeX;
	XnActualIntProperty m_FirmwareCropSizeY;
	XnActualIntProperty m_FirmwareCropOffsetX;
	XnActualIntProperty m_FirmwareCropOffsetY;
	XnActualIntProperty m_FirmwareCropEnabled;
	XnActualIntProperty m_FirmwareExposure;
	XnActualIntProperty m_FirmwareAutoExposure;
	XnActualIntProperty m_FirmwareColorTemperature;
	XnActualIntProperty m_FirmwareAutoWhiteBalance;

	XnActualIntProperty m_ActualRead;
};

#endif //__XN_SENSOR_IMAGE_STREAM_H__