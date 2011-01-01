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





#ifndef __XN_SENSOR_IMAGE_STREAM_H__
#define __XN_SENSOR_IMAGE_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnImageStream.h>
#include "XnSensorStreamHelper.h"
#include "XnSharedMemoryBufferPool.h"

//---------------------------------------------------------------------------
// XnSensorImageStream class
//---------------------------------------------------------------------------
class XnSensorImageStream : public XnImageStream, public IXnSensorStream
{
public:
	XnSensorImageStream(const XnChar* strDeviceName, const XnChar* StreamName, XnSensorObjects* pObjects, XnUInt32 nBufferCount);
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
	XnStatus WriteImpl(XnStreamData* pStreamData) { return XN_STATUS_DEVICE_UNSUPPORTED_MODE; }
	XnSharedMemoryBufferPool* GetSharedMemoryBuffer() { return &m_BufferPool; }

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

private:
	XnStatus ValidateMode();
	static XnUInt32 GetMaxBufferSize(XnFWVer version);

	static XnStatus XN_CALLBACK_TYPE SetInputFormatCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAntiFlickerCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetImageQualityCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnSensorStreamHelper m_Helper;
	XnSharedMemoryBufferPool m_BufferPool;

	XnActualStringProperty m_SharedBufferName;
	XnActualIntProperty m_InputFormat;
	XnActualIntProperty m_AntiFlicker;
	XnActualIntProperty m_ImageQuality;

	XnActualIntProperty m_FirmwareMirror;

	XnActualIntProperty m_FirmwareCropSizeX;
	XnActualIntProperty m_FirmwareCropSizeY;
	XnActualIntProperty m_FirmwareCropOffsetX;
	XnActualIntProperty m_FirmwareCropOffsetY;
	XnActualIntProperty m_FirmwareCropEnabled;

	XnActualIntProperty m_ActualRead;
};

#endif //__XN_SENSOR_IMAGE_STREAM_H__