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





#ifndef __XN_SENSOR_DEPTH_STREAM_H__
#define __XN_SENSOR_DEPTH_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnDepthStream.h>
#include "XnDeviceSensorProtocol.h"
#include "Registration.h"
#include "XnSensorStreamHelper.h"
#include "XnSharedMemoryBufferPool.h"


//---------------------------------------------------------------------------
// XnSensorDepthStream class
//---------------------------------------------------------------------------
class XnSensorDepthStream : public XnDepthStream, public IXnSensorStream
{
public:
	XnSensorDepthStream(const XnChar* strDeviceName, const XnChar* strName, XnSensorObjects* pObjects, XnUInt32 nBufferCount);
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
	XnStatus ReallocTripleFrameBuffer();
	XnStatus CropImpl(XnStreamData* pStreamOutput, const XnCropping* pCropping);
	XnStatus Mirror(XnStreamData* pStreamOutput) const;
	XnStatus ConfigureStreamImpl();
	XnStatus OpenStreamImpl();
	XnStatus CloseStreamImpl();
	XnStatus CreateDataProcessor(XnDataProcessor** ppProcessor);
	XnStatus MapPropertiesToFirmware();
	void GetFirmwareStreamConfig(XnResolutions* pnRes, XnUInt32* pnFPS) { *pnRes = GetResolution(); *pnFPS = GetFPS(); }
	XnStatus WriteImpl(XnStreamData* pStreamData) { return XN_STATUS_DEVICE_UNSUPPORTED_MODE; }
	XnSharedMemoryBufferPool* GetSharedMemoryBuffer() { return &m_BufferPool; }


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


private:
	XnUInt32 CalculateExpectedSize();
	XnStatus DecideFirmwareRegistration(XnBool bRegistration, XnProcessingType registrationType, XnResolutions nRes);
	XnStatus DecidePixelSizeFactor();

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


	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnSensorStreamHelper m_Helper;
	XnSharedMemoryBufferPool m_BufferPool;

	XnActualStringProperty m_SharedBufferName;
	XnActualIntProperty m_InputFormat;
	XnActualIntProperty m_DepthRegistration;
	XnActualIntProperty m_HoleFilter;
	XnActualIntProperty m_WhiteBalance;
	XnActualIntProperty m_Gain;
	XnActualIntProperty m_RegistrationType;
	XnGeneralProperty m_AGCBin;

	XnActualIntProperty m_FirmwareMirror;
	XnActualIntProperty m_FirmwareRegistration;

	XnActualIntProperty m_FirmwareCropSizeX;
	XnActualIntProperty m_FirmwareCropSizeY;
	XnActualIntProperty m_FirmwareCropOffsetX;
	XnActualIntProperty m_FirmwareCropOffsetY;
	XnActualIntProperty m_FirmwareCropEnabled;

	XnActualIntProperty m_ActualRead;
	XnActualIntProperty m_GMCMode;


	XnRegistration m_Registration;
	XnCallbackHandle m_hReferenceSizeChangedCallback;
};

#endif //__XN_SENSOR_DEPTH_STREAM_H__