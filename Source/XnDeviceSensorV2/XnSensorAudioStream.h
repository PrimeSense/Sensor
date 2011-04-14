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





#ifndef __XN_SENSOR_AUDIO_STREAM_H__
#define __XN_SENSOR_AUDIO_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnAudioStream.h>
#include "XnSensorStreamHelper.h"
#include "XnSharedMemoryBufferPool.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_AUDIO_STREAM_DEFAULT_VOLUME					12
#define XN_AUDIO_STREAM_DEFAULT_SAMPLE_RATE				48000
#define XN_AUDIO_STREAM_DEFAULT_NUMBER_OF_CHANNELS		2
#define XN_AUDIO_STREAM_DEFAULT_OUTPUT_FORMAT			XN_OUTPUT_FORMAT_PCM
#define XN_AUDIO_STREAM_DEFAULT_CHUNK_SIZE				2120

//---------------------------------------------------------------------------
// XnSensorAudioStream class
//---------------------------------------------------------------------------
class XnSensorAudioStream : public XnAudioStream, public IXnSensorStream
{
public:
	XnSensorAudioStream(const XnChar* strDeviceName, const XnChar* StreamName, XnSensorObjects* pObjects);
	~XnSensorAudioStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	XnStatus Free();
	XnStatus BatchConfig(const XnActualPropertiesHash& props) { return m_Helper.BatchConfig(props); }

	inline XnSensorStreamHelper* GetHelper() { return &m_Helper; }

	friend class XnAudioProcessor;
protected:
	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Open() { return m_Helper.Open(); }
	XnStatus Close() { return m_Helper.Close(); }
	XnStatus ConfigureStreamImpl();
	XnStatus OpenStreamImpl();
	XnStatus CloseStreamImpl();
	XnStatus CreateDataProcessor(XnDataProcessor** ppProcessor);
	XnStatus MapPropertiesToFirmware();
	void GetFirmwareStreamConfig(XnResolutions* pnRes, XnUInt32* pnFPS) { *pnRes = XN_RESOLUTION_CUSTOM; *pnFPS = 0; }
	XnSharedMemoryBufferPool* GetSharedMemoryBuffer() { return NULL; }

	XnStatus WriteImpl(XnStreamData* pStreamData) { return XN_STATUS_DEVICE_UNSUPPORTED_MODE; }
	XnStatus ReadImpl(XnStreamData* pStreamOutput);
	XnStatus Mirror(XnStreamData* pStreamOutput) const { return XN_STATUS_OK; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetOutputFormat(XnOutputFormats nOutputFormat);
	XnStatus SetLeftChannelVolume(XnUInt32 nVolume);
	XnStatus SetRightChannelVolume(XnUInt32 nVolume);
	XnStatus SetSampleRate(XnSampleRate nSampleRate);
	XnStatus SetNumberOfChannels(XnUInt32 nNumberOfChannels);
	XnStatus SetActualRead(XnBool bRead);

private:
	XnStatus NewData();
	XnStatus ReallocBuffer();

	inline XnSensorFirmwareParams* GetFirmwareParams() const { return m_Helper.GetFirmware()->GetParams(); }

	static XnStatus ConvertNumberOfChannelsToStereo(XnUInt64 nSource, XnUInt64* pnDest);
	static XnStatus ConvertStereoToNumberOfChannels(XnUInt64 nSource, XnUInt64* pnDest);
	static XnStatus ConvertSampleRateToFirmwareRate(XnUInt64 nSource, XnUInt64* pnDest);
	static XnStatus ConvertFirmwareRateToSampleRate(XnUInt64 nSource, XnUInt64* pnDest);

	static XnStatus XN_CALLBACK_TYPE SetLeftChannelVolumeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetRightChannelVolumeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE NewDataCallback(void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnSensorStreamHelper m_Helper;

	const XnChar* m_strDeviceName;
	XnActualStringProperty m_SharedBufferName;
	XnActualIntProperty m_LeftChannelVolume;
	XnActualIntProperty m_RightChannelVolume;

	XnActualIntProperty m_ActualRead;

	XnUInt32 m_nOrigAudioPacketSize;

	XN_SHARED_MEMORY_HANDLE m_hSharedMemory;
	XnAudioSharedBuffer* m_pSharedHeader;
	XnUInt32 m_nFrameID;
};

#endif //__XN_SENSOR_AUDIO_STREAM_H__