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
#include "XnSensorAudioStream.h"
#include "XnSensor.h"
#include "XnAudioProcessor.h"
#include <XnFormatsStatus.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_AUDIO_MAX_SAMPLE_RATE				48000
#define XN_AUDIO_MAX_NUMBER_OF_CHANNELS			2

#define XN_SENSOR_PROTOCOL_AUDIO_PACKET_SIZE_BULK		424
#define XN_SENSOR_PROTOCOL_AUDIO_PACKET_SIZE_ISO		180

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSensorAudioStream::XnSensorAudioStream(const XnChar* strDeviceName, const XnChar* StreamName, XnSensorObjects* pObjects) :
	XnAudioStream(StreamName, XN_AUDIO_MAX_NUMBER_OF_CHANNELS),
	m_strDeviceName(strDeviceName),
	m_Helper(pObjects),
	m_LeftChannelVolume(XN_STREAM_PROPERTY_LEFT_CHANNEL_VOLUME, XN_AUDIO_STREAM_DEFAULT_VOLUME),
	m_RightChannelVolume(XN_STREAM_PROPERTY_RIGHT_CHANNEL_VOLUME, XN_AUDIO_STREAM_DEFAULT_VOLUME),
	m_SharedBufferName(XN_STREAM_PROPERTY_SHARED_BUFFER_NAME),
	m_ActualRead(XN_STREAM_PROPERTY_ACTUAL_READ_DATA, FALSE),
	m_hSharedMemory(NULL),
	m_pSharedHeader(NULL),
	m_nFrameID(0)
{
	m_LeftChannelVolume.UpdateSetCallback(SetLeftChannelVolumeCallback, this);
	m_RightChannelVolume.UpdateSetCallback(SetRightChannelVolumeCallback, this);
	m_ActualRead.UpdateSetCallback(SetActualReadCallback, this);
}

XnStatus XnSensorAudioStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnAudioStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	// init helper
	nRetVal = m_Helper.Init(this, this);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SetReadChunkSize(XN_AUDIO_STREAM_DEFAULT_CHUNK_SIZE);
	XN_IS_STATUS_OK(nRetVal);

	// add properties
	XN_VALIDATE_ADD_PROPERTIES(this, &m_LeftChannelVolume, &m_RightChannelVolume, &m_SharedBufferName, &m_ActualRead);

	// check what's the firmware audio packet size
	if (m_Helper.GetPrivateData()->SensorHandle.MiscConnection.bIsISO)
		m_nOrigAudioPacketSize = XN_SENSOR_PROTOCOL_AUDIO_PACKET_SIZE_ISO;
	else
		m_nOrigAudioPacketSize = XN_SENSOR_PROTOCOL_AUDIO_PACKET_SIZE_BULK;

	// alloc buffer
	nRetVal = ReallocBuffer();
	XN_IS_STATUS_OK(nRetVal);

	m_Helper.GetPrivateData()->pAudioCallback = NewDataCallback;
	m_Helper.GetPrivateData()->pAudioCallbackCookie = this;

	// data processor
	nRetVal = m_Helper.RegisterDataProcessorProperty(NumberOfChannelsProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::Free()
{
	m_Helper.Free();
	XnAudioStream::Free();
	if (m_hSharedMemory != NULL)
	{
		xnOSCloseSharedMemory(m_hSharedMemory);
		m_hSharedMemory = NULL;
	}
	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::MapPropertiesToFirmware()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.MapFirmwareProperty(SampleRateProperty(), GetFirmwareParams()->m_AudioSampleRate, FALSE, ConvertSampleRateToFirmwareRate);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(NumberOfChannelsProperty(), GetFirmwareParams()->m_AudioStereo, FALSE, ConvertNumberOfChannelsToStereo);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_LeftChannelVolume, GetFirmwareParams()->m_AudioLeftChannelGain, TRUE);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_RightChannelVolume, GetFirmwareParams()->m_AudioRightChannelGain, TRUE);
	XN_IS_STATUS_OK(nRetVal);;

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::ConfigureStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificMiscUsb->pUsbConnection->UsbEp);

	nRetVal = SetActualRead(TRUE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.ConfigureFirmware(SampleRateProperty());
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(NumberOfChannelsProperty());
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(m_LeftChannelVolume);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(m_RightChannelVolume);
	XN_IS_STATUS_OK(nRetVal);;

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::SetActualRead(XnBool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_ActualRead.GetValue() != bRead)
	{
		if (bRead)
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Creating USB audio read thread...");
			XnSpecificUsbDevice* pUSB = GetHelper()->GetPrivateData()->pSpecificMiscUsb;
			nRetVal = xnUSBInitReadThread(pUSB->pUsbConnection->UsbEp, pUSB->nChunkReadBytes, XN_SENSOR_USB_MISC_BUFFERS, pUSB->nTimeout, XnDeviceSensorProtocolUsbEpCb, pUSB);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Shutting down USB audio read thread...");
			xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificMiscUsb->pUsbConnection->UsbEp);
		}

		nRetVal = m_ActualRead.UnsafeUpdateValue(bRead);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::OpenStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream2Mode.SetValue(XN_AUDIO_STREAM_ON);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnAudioStream::Open();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::CloseStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream2Mode.SetValue(XN_AUDIO_STREAM_OFF);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnAudioStream::Close();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SetActualRead(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::CreateDataProcessor(XnDataProcessor** ppProcessor)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDataProcessor* pAudioProcessor;
	XN_VALIDATE_NEW_AND_INIT(pAudioProcessor, XnAudioProcessor, this, &m_Helper, m_nOrigAudioPacketSize);

	*ppProcessor = pAudioProcessor;

	return XN_STATUS_OK;
}

XnStatus XnSensorAudioStream::SetOutputFormat(XnOutputFormats nOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nOutputFormat)
	{
	case XN_OUTPUT_FORMAT_PCM:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Output format %d, isn't supported by sensor audio stream!", nOutputFormat);
	}

	nRetVal = XnAudioStream::SetOutputFormat(nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::SetSampleRate(XnSampleRate nSampleRate)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_Helper.BeforeSettingFirmwareParam(SampleRateProperty(), nSampleRate);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnAudioStream::SetSampleRate(nSampleRate);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(SampleRateProperty());
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::SetNumberOfChannels(XnUInt32 nNumberOfChannels)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_Helper.BeforeSettingFirmwareParam(NumberOfChannelsProperty(), nNumberOfChannels);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnAudioStream::SetNumberOfChannels(nNumberOfChannels);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReallocBuffer();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(NumberOfChannelsProperty());
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::NewData()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDevicePrivateData* pDevicePrivateData = m_Helper.GetPrivateData();

	// check how many buffers we have
	XnInt32 nAvailbalePackets = pDevicePrivateData->nAudioWriteIndex - pDevicePrivateData->nAudioReadIndex;
	if (nAvailbalePackets < 0)
		nAvailbalePackets += pDevicePrivateData->nAudioBufferNumOfPackets;

	if ((XnUInt32)nAvailbalePackets * pDevicePrivateData->nAudioPacketSize >= GetReadChunkSize())
	{
		// update last write index (the last written byte)
		m_pSharedHeader->nWritePacketIndex = pDevicePrivateData->nAudioWriteIndex;
		// take first packet timestamp
		NewDataAvailable(pDevicePrivateData->pAudioPacketsTimestamps[pDevicePrivateData->nAudioReadIndex], 0);
	}

	return XN_STATUS_OK;
}

XnStatus XnSensorAudioStream::ReadImpl(XnStreamData *pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDevicePrivateData* pDevicePrivateData = m_Helper.GetPrivateData();

	pStreamOutput->nDataSize = 0;

	XN_AUDIO_TYPE* pAudioBuf = (XN_AUDIO_TYPE*)pStreamOutput->pData;

	xnOSEnterCriticalSection(&pDevicePrivateData->hAudioBufferCriticalSection);

	// check how many buffers we have
	XnInt32 nAvailbalePackets = pDevicePrivateData->nAudioWriteIndex - pDevicePrivateData->nAudioReadIndex;
	if (nAvailbalePackets < 0)
		nAvailbalePackets += pDevicePrivateData->nAudioBufferNumOfPackets;

	// now check if stream frame buffer has enough space
	if (GetRequiredDataSize() < (XnUInt32)nAvailbalePackets * pDevicePrivateData->nAudioPacketSize)
	{
		xnOSLeaveCriticalSection(&pDevicePrivateData->hAudioBufferCriticalSection);
		return (XN_STATUS_IO_INVALID_STREAM_AUDIO_BUFFER_SIZE);
	}

	// take first packet timestamp
	pStreamOutput->nTimestamp = pDevicePrivateData->pAudioPacketsTimestamps[pDevicePrivateData->nAudioReadIndex];
	XnUChar* pPacketData = pDevicePrivateData->pAudioBuffer + (pDevicePrivateData->nAudioReadIndex * pDevicePrivateData->nAudioPacketSize);

	// copy
	while (pDevicePrivateData->nAudioReadIndex != pDevicePrivateData->nAudioWriteIndex)
	{
		xnOSMemCopy(pAudioBuf, pPacketData, pDevicePrivateData->nAudioPacketSize);
		pAudioBuf += pDevicePrivateData->nAudioPacketSize;
		pStreamOutput->nDataSize += pDevicePrivateData->nAudioPacketSize;

		pDevicePrivateData->nAudioReadIndex++;
		pPacketData += pDevicePrivateData->nAudioPacketSize;
		if (pDevicePrivateData->nAudioReadIndex == pDevicePrivateData->nAudioBufferNumOfPackets)
		{
			pDevicePrivateData->nAudioReadIndex = 0;
			pPacketData = pDevicePrivateData->pAudioBuffer;
		}
	}

	xnOSLeaveCriticalSection(&pDevicePrivateData->hAudioBufferCriticalSection);

	++m_nFrameID;
	pStreamOutput->nFrameID = m_nFrameID;
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::ConvertNumberOfChannelsToStereo(XnUInt64 nSource, XnUInt64* pnDest)
{
	*pnDest = (nSource == 2);
	return XN_STATUS_OK;
}

XnStatus XnSensorAudioStream::ConvertStereoToNumberOfChannels(XnUInt64 nSource, XnUInt64* pnDest)
{
	*pnDest = nSource ? 2 : 1;
	return XN_STATUS_OK;
}

XnStatus XnSensorAudioStream::ConvertSampleRateToFirmwareRate(XnUInt64 nSource, XnUInt64* pnDest)
{
	switch (nSource)
	{
	case XN_SAMPLE_RATE_8K:
		*pnDest = A2D_SAMPLE_RATE_8KHZ;
		break;
	case XN_SAMPLE_RATE_11K:
		*pnDest = A2D_SAMPLE_RATE_11KHZ;
		break;
	case XN_SAMPLE_RATE_12K:
		*pnDest = A2D_SAMPLE_RATE_12KHZ;
		break;
	case XN_SAMPLE_RATE_16K:
		*pnDest = A2D_SAMPLE_RATE_16KHZ;
		break;
	case XN_SAMPLE_RATE_22K:
		*pnDest = A2D_SAMPLE_RATE_22KHZ;
		break;
	case XN_SAMPLE_RATE_24K:
		*pnDest = A2D_SAMPLE_RATE_24KHZ;
		break;
	case XN_SAMPLE_RATE_32K:
		*pnDest = A2D_SAMPLE_RATE_32KHZ;
		break;
	case XN_SAMPLE_RATE_44K:
		*pnDest = A2D_SAMPLE_RATE_44KHZ;
		break;
	case XN_SAMPLE_RATE_48K:
		*pnDest = A2D_SAMPLE_RATE_48KHZ;
		break;
	default:
		return XN_STATUS_DEVICE_UNSUPPORTED_MODE;
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::ConvertFirmwareRateToSampleRate(XnUInt64 nSource, XnUInt64* pnDest)
{
	switch (nSource)
	{
	case A2D_SAMPLE_RATE_8KHZ:
		*pnDest = XN_SAMPLE_RATE_8K;
		break;
	case A2D_SAMPLE_RATE_11KHZ:
		*pnDest = XN_SAMPLE_RATE_11K;
		break;
	case A2D_SAMPLE_RATE_12KHZ:
		*pnDest = XN_SAMPLE_RATE_12K;
		break;
	case A2D_SAMPLE_RATE_16KHZ:
		*pnDest = XN_SAMPLE_RATE_16K;
		break;
	case A2D_SAMPLE_RATE_22KHZ:
		*pnDest = XN_SAMPLE_RATE_22K;
		break;
	case A2D_SAMPLE_RATE_24KHZ:
		*pnDest = XN_SAMPLE_RATE_24K;
		break;
	case A2D_SAMPLE_RATE_32KHZ:
		*pnDest = XN_SAMPLE_RATE_32K;
		break;
	case A2D_SAMPLE_RATE_44KHZ:
		*pnDest = XN_SAMPLE_RATE_44K;
		break;
	case A2D_SAMPLE_RATE_48KHZ:
		*pnDest = XN_SAMPLE_RATE_48K;
		break;
	default:
		return XN_STATUS_DEVICE_UNSUPPORTED_MODE;
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::SetLeftChannelVolume(XnUInt32 nVolume)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_Helper.SimpleSetFirmwareParam(m_LeftChannelVolume, nVolume);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::SetRightChannelVolume(XnUInt32 nVolume)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_RightChannelVolume, nVolume);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioStream::ReallocBuffer()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDevicePrivateData* pDevicePrivateData = m_Helper.GetPrivateData();

	if (m_hSharedMemory == NULL)
	{
		// first time, create shared memory

		// we allocate enough for 5 seconds of audio
		XnUInt32 nSampleSize = 2 * 2; // 16-bit per channel (2 bytes) * max number of channels (2)
		XnUInt32 nSamples = 48000 * 5; // max sample rate * number of seconds

		XnUInt32 nMaxBufferSize = nSamples * nSampleSize;

		// find min packet size (so we'll have max packet count)
		XnUInt32 nMinPacketSize = XN_MIN(XN_SENSOR_PROTOCOL_AUDIO_PACKET_SIZE_BULK, XN_SENSOR_PROTOCOL_AUDIO_PACKET_SIZE_ISO);
		XnUInt32 nMaxPacketCount = nMaxBufferSize / nMinPacketSize - 1;

		XnUInt32 nSharedBufferSize = 
			sizeof(XnAudioSharedBuffer) + // header
			sizeof(XnUInt64) * nMaxPacketCount + // packet timestamps
			nMaxBufferSize;

		// to make the name unique, we'll add process ID
		XN_PROCESS_ID procID;
		xnOSGetCurrentProcessID(&procID);
		XnChar strSharedName[XN_DEVICE_MAX_STRING_LENGTH];
		sprintf(strSharedName, "%u_%s_%s", procID, m_strDeviceName, GetName());

		nRetVal = m_SharedBufferName.UnsafeUpdateValue(strSharedName);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = RequiredSizeProperty().UnsafeUpdateValue(nMaxBufferSize);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnOSCreateSharedMemory(strSharedName, nSharedBufferSize, XN_OS_FILE_READ | XN_OS_FILE_WRITE, &m_hSharedMemory);
		XN_IS_STATUS_OK(nRetVal);

		XnUChar* pAddress;
		nRetVal = xnOSSharedMemoryGetAddress(m_hSharedMemory, (void**)&pAddress);
		XN_IS_STATUS_OK(nRetVal);

		m_pSharedHeader = (XnAudioSharedBuffer*)pAddress;
		pDevicePrivateData->pAudioPacketsTimestamps = (XnUInt64*)(pAddress + sizeof(XnAudioSharedBuffer));
		pDevicePrivateData->pAudioBuffer = (XN_AUDIO_TYPE*)(pAddress + sizeof(XnAudioSharedBuffer) + sizeof(XnUInt64) * nMaxPacketCount);
		pDevicePrivateData->nAudioBufferSize = nMaxBufferSize;

		m_pSharedHeader->nTimestampsListOffset = sizeof(XnAudioSharedBuffer);
		m_pSharedHeader->nBufferOffset = pDevicePrivateData->pAudioBuffer - pAddress;
	}

	// calculate current packet size
	pDevicePrivateData->nAudioPacketSize = m_nOrigAudioPacketSize;

	if (m_Helper.GetFirmwareVersion() >= XN_SENSOR_FW_VER_5_2 && GetNumberOfChannels() == 1)
	{
		pDevicePrivateData->nAudioPacketSize /= 2;
	}

	pDevicePrivateData->nAudioBufferNumOfPackets = pDevicePrivateData->nAudioBufferSize / pDevicePrivateData->nAudioPacketSize;
	pDevicePrivateData->nAudioBufferSize = pDevicePrivateData->nAudioBufferNumOfPackets * pDevicePrivateData->nAudioPacketSize;

	m_pSharedHeader->nPacketCount = pDevicePrivateData->nAudioBufferNumOfPackets;
	m_pSharedHeader->nPacketSize = pDevicePrivateData->nAudioPacketSize;

	// set read and write indices
	pDevicePrivateData->nAudioReadIndex = 0;
	pDevicePrivateData->nAudioWriteIndex = 0;

	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnSensorAudioStream::SetLeftChannelVolumeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorAudioStream* pThis = (XnSensorAudioStream*)pCookie;
	return pThis->SetLeftChannelVolume((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorAudioStream::SetRightChannelVolumeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorAudioStream* pThis = (XnSensorAudioStream*)pCookie;
	return pThis->SetRightChannelVolume((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorAudioStream::SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorAudioStream* pThis = (XnSensorAudioStream*)pCookie;
	return pThis->SetActualRead(nValue == TRUE);
}

XnStatus XN_CALLBACK_TYPE XnSensorAudioStream::NewDataCallback(void* pCookie)
{
	XnSensorAudioStream* pThis = (XnSensorAudioStream*)pCookie;
	return pThis->NewData();
}

