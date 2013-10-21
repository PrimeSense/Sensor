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
#ifndef __XN_SENSOR_DEPTH_STREAM_H__
#define __XN_SENSOR_DEPTH_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnDepthStream.h>
#include "XnDeviceSensorProtocol.h"
#include "Registration.h"
#include "XnSensorStreamHelper.h"


//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#if (XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	#define XN_DEPTH_STREAM_DEFAULT_INPUT_FORMAT				XN_IO_DEPTH_FORMAT_UNCOMPRESSED_12_BIT
	#define XN_DEPTH_STREAM_DEFAULT_RESOLUTION					XN_RESOLUTION_QQVGA
#else
	#define XN_DEPTH_STREAM_DEFAULT_INPUT_FORMAT				XN_IO_DEPTH_FORMAT_UNCOMPRESSED_11_BIT
	#define XN_DEPTH_STREAM_DEFAULT_RESOLUTION					XN_RESOLUTION_QVGA
#endif

#define XN_DEPTH_STREAM_DEFAULT_FPS							30
#define XN_DEPTH_STREAM_DEFAULT_OUTPUT_FORMAT				XN_OUTPUT_FORMAT_DEPTH_VALUES
#define XN_DEPTH_STREAM_DEFAULT_REGISTRATION				FALSE
#define XN_DEPTH_STREAM_DEFAULT_REGISTRATION_TYPE			XN_PROCESSING_DONT_CARE
#define XN_DEPTH_STREAM_DEFAULT_HOLE_FILLER					TRUE
#define XN_DEPTH_STREAM_DEFAULT_WHITE_BALANCE				TRUE
#define XN_DEPTH_STREAM_DEFAULT_GAIN_OLD					50
#define XN_DEPTH_STREAM_DEFAULT_GMC_MODE					TRUE
#define XN_DEPTH_STREAM_DEFAULT_CLOSE_RANGE					FALSE
#define XN_DEPTH_STREAM_DEFAULT_SHIFT_MAP_APPENDED			TRUE


//---------------------------------------------------------------------------
// XnSensorDepthStream class
//---------------------------------------------------------------------------
class XnSensorDepthStream : public XnDepthStream, public IXnSensorStream
{
public:
	XnSensorDepthStream(const XnChar* strName, XnSensorObjects* pObjects);
	~XnSensorDepthStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	XnStatus Free();
	XnStatus BatchConfig(const XnActualPropertiesHash& props) { return m_Helper.BatchConfig(props); }

	inline XnSensorStreamHelper* GetHelper() { return &m_Helper; }

	friend class XnDepthProcessor;

protected:
	inline XnSensorFirmwareParams* GetFirmwareParams() const { return m_Helper.GetFirmware()->GetParams(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Open() { return m_Helper.Open(); }
	XnStatus Close() { return m_Helper.Close(); }
	XnStatus PostProcessFrame(XnStreamData* pFrameData);
	XnStatus CalcRequiredSize(XnUInt32* pnRequiredSize) const;
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


protected:
	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetOutputFormat(XnOutputFormats nOutputFormat);
	XnStatus SetMirror(XnBool bIsMirrored);
	XnStatus SetResolution(XnResolutions nResolution);
	XnStatus SetFPS(XnUInt32 nFPS);
	virtual XnStatus SetInputFormat(XnIODepthFormats nInputFormat);
	virtual XnStatus SetRegistration(XnBool bRegistration);
	virtual XnStatus SetHoleFilter(XnBool bHoleFilter);
	virtual XnStatus SetWhiteBalance(XnBool bWhiteBalance);
	virtual XnStatus SetGain(XnUInt32 nGain);
	virtual XnStatus SetRegistrationType(XnProcessingType type);
	virtual XnStatus SetAGCBin(const XnDepthAGCBin* pBin);
	virtual XnStatus GetAGCBin(XnDepthAGCBin* pBin);
	XnStatus SetCropping(const XnCropping* pCropping);
	XnStatus SetActualRead(XnBool bRead);
	virtual XnStatus SetGMCMode(XnBool bGMCMode);
	virtual XnStatus SetCloseRange(XnBool bCloseRange);
	virtual XnStatus SetCroppingMode(XnCroppingMode mode);
	XnStatus GetImageCoordinatesOfDepthPixel(XnUInt32 x, XnUInt32 y, XnDepthPixel z, XnUInt32 imageXRes, XnUInt32 imageYRes, XnUInt32& imageX, XnUInt32& imageY);


private:
	XnUInt32 CalculateExpectedSize();
	XnStatus DecideFirmwareRegistration(XnBool bRegistration, XnProcessingType registrationType, XnResolutions nRes);
	XnStatus DecidePixelSizeFactor();
	XnStatus SetCroppingImpl(const XnCropping* pCropping, XnCroppingMode mode);
	XnStatus CloseRangeControl(XnBool bEnabled);

	static XnStatus XN_CALLBACK_TYPE SetInputFormatCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetRegistrationCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetHoleFilterCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetWhiteBalanceCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetGainCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetRegistrationTypeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAGCBinCallback(XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetAGCBinCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE DecideFirmwareRegistrationCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE DecidePixelSizeFactorCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadAGCBinsFromFile(XnGeneralProperty* pSender, const XnChar* csINIFile, const XnChar* csSection);
	static XnStatus XN_CALLBACK_TYPE SetGMCModeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCloseRangeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCroppingModeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetShiftsMapCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetPixelRegistrationCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);


	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnSensorStreamHelper m_Helper;
	XnActualIntProperty m_InputFormat;
	XnActualIntProperty m_DepthRegistration;
	XnActualIntProperty m_HoleFilter;
	XnActualIntProperty m_WhiteBalance;
	XnActualIntProperty m_Gain;
	XnActualIntProperty m_RegistrationType;
	XnActualIntProperty m_CroppingMode;
	XnGeneralProperty m_AGCBin;

	XnActualIntProperty m_FirmwareMirror;
	XnActualIntProperty m_FirmwareRegistration;

	XnActualIntProperty m_FirmwareCropSizeX;
	XnActualIntProperty m_FirmwareCropSizeY;
	XnActualIntProperty m_FirmwareCropOffsetX;
	XnActualIntProperty m_FirmwareCropOffsetY;
	XnActualIntProperty m_FirmwareCropMode;

	XnActualIntProperty m_ActualRead;
	XnActualIntProperty m_GMCMode;
	XnActualIntProperty m_CloseRange;
	XnGeneralProperty m_ShiftsMap;
	XnGeneralProperty m_PixelRegistration;
	const XnUInt16* m_pLastFrameShiftsMap;


	XnRegistration m_Registration;
	XnCallbackHandle m_hReferenceSizeChangedCallback;
};

#endif //__XN_SENSOR_DEPTH_STREAM_H__
