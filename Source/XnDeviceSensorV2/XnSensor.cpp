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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensor.h"

#include "XnSensorDepthStream.h"
#include "XnSensorImageStream.h"
#include "XnSensorIRStream.h"
#include "XnSensorAudioStream.h"
#include "XnDeviceSensor.h"
#include "XnHostProtocol.h"
#include "XnDeviceSensorInit.h"
#include <XnPsVersion.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_MAX_STREAM_COUNT						5
#define XN_SENSOR_FRAME_SYNC_MAX_DIFF					3
#define XN_SENSOR_DEFAULT_CLOSE_STREAMS_ON_SHUTDOWN		TRUE
#define XN_SENSOR_DEFAULT_HOST_TIMESTAMPS				FALSE
#define XN_GLOBAL_CONFIG_FILE_NAME						"GlobalDefaults.ini"

// on weak platforms (Arm), we prefer to use BULK
#if (XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	#define XN_SENSOR_DEFAULT_USB_INTERFACE				XN_SENSOR_USB_INTERFACE_BULK_ENDPOINTS
#else
	#define XN_SENSOR_DEFAULT_USB_INTERFACE				XN_SENSOR_USB_INTERFACE_ISO_ENDPOINTS
#endif

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnWaitForSycnhedFrameData
{
	XnSensor* pThis;
	const XnChar* strDepthStream;
	const XnChar* strImageStream;
} XnWaitForSycnhedFrameData;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSensor::XnSensor(XnBool bResetOnStartup /* = TRUE */, XnBool bLeanInit /* = FALSE */) :
	XnDeviceBase(XN_DEVICE_NAME, TRUE),
	m_ErrorState(XN_MODULE_PROPERTY_ERROR_STATE, XN_STATUS_OK),
	m_ResetSensorOnStartup(XN_MODULE_PROPERTY_RESET_SENSOR_ON_STARTUP, bResetOnStartup),
	m_LeanInit(XN_MODULE_PROPERTY_LEAN_INIT, bLeanInit),
	m_Interface(XN_MODULE_PROPERTY_USB_INTERFACE, bResetOnStartup ? XN_SENSOR_DEFAULT_USB_INTERFACE : XN_SENSOR_USB_INTERFACE_DEFAULT),
	m_ReadFromEP1(XN_MODULE_PROPERTY_READ_ENDPOINT_1, TRUE),
	m_ReadFromEP2(XN_MODULE_PROPERTY_READ_ENDPOINT_2, TRUE),
	m_ReadFromEP3(XN_MODULE_PROPERTY_READ_ENDPOINT_3, TRUE),
	m_ReadData("ReadData", FALSE),
	m_FrameSync(XN_MODULE_PROPERTY_FRAME_SYNC, FALSE),
	m_FirmwareParam(XN_MODULE_PROPERTY_FIRMWARE_PARAM, NULL),
	m_CmosBlankingUnits(XN_MODULE_PROPERTY_CMOS_BLANKING_UNITS, NULL),
	m_CmosBlankingTime(XN_MODULE_PROPERTY_CMOS_BLANKING_TIME, NULL),
	m_Reset(XN_MODULE_PROPERTY_RESET),
	m_FirmwareMode(XN_MODULE_PROPERTY_FIRMWARE_MODE),
	m_Version(XN_MODULE_PROPERTY_VERSION, &m_DevicePrivateData.Version, sizeof(m_DevicePrivateData.Version), NULL),
	m_FixedParam(XN_MODULE_PROPERTY_FIXED_PARAMS, NULL),
	m_CloseStreamsOnShutdown(XN_MODULE_PROPERTY_CLOSE_STREAMS_ON_SHUTDOWN, XN_SENSOR_DEFAULT_CLOSE_STREAMS_ON_SHUTDOWN),
	m_HostTimestamps(XN_MODULE_PROPERTY_HOST_TIMESTAMPS, XN_SENSOR_DEFAULT_HOST_TIMESTAMPS),
	m_ID(XN_MODULE_PROPERTY_ID),
	m_InstancePointer(XN_SENSOR_PROPERTY_INSTANCE_POINTER),
	m_USBPath(XN_MODULE_PROPERTY_USB_PATH),
	m_DeviceName(XN_MODULE_PROPERTY_PHYSICAL_DEVICE_NAME),
	m_VendorSpecificData(XN_MODULE_PROPERTY_VENDOR_SPECIFIC_DATA),
	m_PlatformString(XN_MODULE_PROPERTY_SENSOR_PLATFORM_STRING),
	m_AudioSupported(XN_MODULE_PROPERTY_AUDIO_SUPPORTED),
	m_ImageSupported(XN_MODULE_PROPERTY_IMAGE_SUPPORTED),
	m_ImageControl(XN_MODULE_PROPERTY_IMAGE_CONTROL, NULL),
	m_DepthControl(XN_MODULE_PROPERTY_DEPTH_CONTROL, NULL),
	m_AHB(XN_MODULE_PROPERTY_AHB, NULL),
	m_Firmware(&m_DevicePrivateData),
	m_SensorIO(&m_DevicePrivateData.SensorHandle),
	m_FPS(),
	m_CmosInfo(&m_Firmware, &m_DevicePrivateData),
	m_Objects(&m_Firmware, &m_DevicePrivateData, &m_FPS, &m_CmosInfo),
	m_FrameSyncDump(NULL),
	m_bInitialized(FALSE)
{
	// reset all data
	xnOSMemSet(&m_DevicePrivateData, 0, sizeof(XnDevicePrivateData));
	m_strGlobalConfigFile[0] = '\0';

	m_ResetSensorOnStartup.UpdateSetCallbackToDefault();
	m_LeanInit.UpdateSetCallbackToDefault();
	m_Interface.UpdateSetCallback(SetInterfaceCallback, this);
	m_ReadFromEP1.UpdateSetCallback(SetReadEndpoint1Callback, this);
	m_ReadFromEP2.UpdateSetCallback(SetReadEndpoint2Callback, this);
	m_ReadFromEP3.UpdateSetCallback(SetReadEndpoint3Callback, this);
	m_ReadData.UpdateSetCallback(SetReadDataCallback, this);
	m_FrameSync.UpdateSetCallbackToDefault();
	m_FirmwareParam.UpdateSetCallback(SetFirmwareParamCallback, this);
	m_FirmwareParam.UpdateGetCallback(GetFirmwareParamCallback, this);
	m_CmosBlankingUnits.UpdateSetCallback(SetCmosBlankingUnitsCallback, this);
	m_CmosBlankingUnits.UpdateGetCallback(GetCmosBlankingUnitsCallback, this);
	m_CmosBlankingTime.UpdateSetCallback(SetCmosBlankingTimeCallback, this);
	m_CmosBlankingTime.UpdateGetCallback(GetCmosBlankingTimeCallback, this);
	m_Reset.UpdateSetCallback(ResetCallback, this);
	m_FirmwareMode.UpdateSetCallback(SetFirmwareModeCallback, this);
	m_FirmwareMode.UpdateGetCallback(GetFirmwareModeCallback, this);
	m_FixedParam.UpdateGetCallback(GetFixedParamsCallback, this);
	m_CloseStreamsOnShutdown.UpdateSetCallbackToDefault();
	m_HostTimestamps.UpdateSetCallbackToDefault();
	m_AudioSupported.UpdateGetCallback(GetAudioSupportedCallback, this);
	m_ImageSupported.UpdateGetCallback(GetImageSupportedCallback, this);
	m_InstancePointer.UpdateGetCallback(GetInstanceCallback, this);
	m_ImageControl.UpdateSetCallback(SetImageCmosRegisterCallback, this);
	m_ImageControl.UpdateGetCallback(GetImageCmosRegisterCallback, this);
	m_DepthControl.UpdateSetCallback(SetDepthCmosRegisterCallback, this);
	m_DepthControl.UpdateGetCallback(GetDepthCmosRegisterCallback, this);
	m_AHB.UpdateSetCallback(WriteAHBCallback, this);
	m_AHB.UpdateGetCallback(ReadAHBCallback, this);

}

XnSensor::~XnSensor()
{
	XnSensor::Destroy();
}

XnStatus XnSensor::GetDefinition(XnDeviceDefinition* pDeviceDefinition)
{
	XN_VALIDATE_OUTPUT_PTR(pDeviceDefinition);

	pDeviceDefinition->cpName = XN_DEVICE_NAME;
	pDeviceDefinition->cpDescription = XN_DEVICE_DESCRIPTION;
	pDeviceDefinition->nMajorVersion = XN_DEVICE_MAJORVERSION;
	pDeviceDefinition->nMinorVersion = XN_DEVICE_MINORVERSION;
	pDeviceDefinition->nXironVersion = XN_PS_MAJOR_VERSION;

	return (XN_STATUS_OK);
}

XnStatus XnSensor::Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_INPUT_PTR(pnCount);

	nRetVal = XnSensorIO::EnumerateSensors(aConnectionStrings, pnCount);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::InitImpl(const XnDeviceConfig *pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Initializing device sensor...");


	// Frame Sync
	XnCallbackHandle hCallbackDummy;
	nRetVal = m_FrameSync.OnChangeEvent().Register(FrameSyncPropertyChangedCallback, this, hCallbackDummy);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetFirmware()->GetParams()->m_Stream0Mode.OnChangeEvent().Register(FrameSyncPropertyChangedCallback, this, hCallbackDummy);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetFirmware()->GetParams()->m_Stream1Mode.OnChangeEvent().Register(FrameSyncPropertyChangedCallback, this, hCallbackDummy);
	XN_IS_STATUS_OK(nRetVal);

	// other stuff
	m_FrameSyncDump = xnDumpFileOpen(XN_DUMP_FRAME_SYNC, "FrameSync.csv");
	xnDumpFileWriteString(m_FrameSyncDump, "HostTime(us),DepthNewData,DepthTimestamp(ms),ImageNewData,ImageTimestamp(ms),Diff(ms),Action\n");

	nRetVal = XnDeviceBase::InitImpl(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	// now that everything is configured, open the sensor
	nRetVal = InitSensor(pDeviceConfig);
	if (nRetVal != XN_STATUS_OK)
	{
		Destroy();
		return (nRetVal);
	}

	xnLogInfo(XN_MASK_DEVICE_SENSOR, "Device sensor initialized");

	return (XN_STATUS_OK);
}

XnStatus XnSensor::InitSensor(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnDevicePrivateData* pDevicePrivateData = GetDevicePrivateData();

	pDevicePrivateData->pSensor = this;

	pDevicePrivateData->nDepthFramePos = 0;
	pDevicePrivateData->nImageFramePos = 0;
	xnOSMemCopy(&pDevicePrivateData->DeviceConfig, pDeviceConfig, sizeof(XnDeviceConfig));

	xnOSMemSet(pDevicePrivateData->cpSensorID, 0, XN_SENSOR_PROTOCOL_SENSOR_ID_LENGTH);

	switch (pDeviceConfig->DeviceMode)
	{
	case XN_DEVICE_MODE_READ:
		break;
	case XN_DEVICE_MODE_WRITE:
		return (XN_STATUS_IO_DEVICE_MODE_NOT_SUPPORTED);
	default: 
		return (XN_STATUS_IO_DEVICE_INVALID_MODE);
	}

	// Register USB event callback
	#if WIN32
	nRetVal = m_SensorIO.SetCallback(&USBEventCallback, this);
	XN_IS_STATUS_OK(nRetVal);
	#endif

	// open IO
	nRetVal = m_SensorIO.OpenDevice(pDeviceConfig->cpConnectionString);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_USBPath.UnsafeUpdateValue(m_SensorIO.GetDevicePath());
	XN_IS_STATUS_OK(nRetVal);

	// initialize
	nRetVal = XnDeviceSensorInit(pDevicePrivateData);
	XN_IS_STATUS_OK(nRetVal);

	// init firmware
	nRetVal = m_Firmware.Init((XnBool)m_ResetSensorOnStartup.GetValue(), (XnBool)m_LeanInit.GetValue());
	XN_IS_STATUS_OK(nRetVal);
	m_bInitialized = TRUE;

	m_ResetSensorOnStartup.UpdateSetCallback(NULL, NULL);
	m_LeanInit.UpdateSetCallback(NULL, NULL);

	// update device info properties
	nRetVal = m_DeviceName.UnsafeUpdateValue(GetFixedParams()->GetDeviceName());
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_VendorSpecificData.UnsafeUpdateValue(GetFixedParams()->GetVendorData());
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_ID.UnsafeUpdateValue(GetFixedParams()->GetSensorSerial());
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = m_PlatformString.UnsafeUpdateValue(GetFixedParams()->GetPlatformString());
	XN_IS_STATUS_OK(nRetVal);

	// Add supported streams
	AddSupportedStream(XN_STREAM_TYPE_DEPTH);
	AddSupportedStream(XN_STREAM_TYPE_IR);

	if (GetFirmware()->GetInfo()->bImageSupported)
	{
		AddSupportedStream(XN_STREAM_TYPE_IMAGE);
	}

	if (GetFirmware()->GetInfo()->bAudioSupported)
	{
		AddSupportedStream(XN_STREAM_TYPE_AUDIO);
	}

	return XN_STATUS_OK;
}

XnStatus XnSensor::Destroy()
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnDevicePrivateData* pDevicePrivateData = GetDevicePrivateData();


	// if needed, close the streams
	if (m_bInitialized && m_CloseStreamsOnShutdown.GetValue() == TRUE && m_ReadData.GetValue() == TRUE)
	{
		nRetVal = m_Firmware.GetParams()->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_OFF);
		nRetVal = m_Firmware.GetParams()->m_Stream1Mode.SetValue(XN_VIDEO_STREAM_OFF);
		nRetVal = m_Firmware.GetParams()->m_Stream2Mode.SetValue(XN_AUDIO_STREAM_OFF);
	}

	// close IO (including all reading threads)
	m_SensorIO.CloseDevice();
	m_bInitialized = FALSE;


	if (pDevicePrivateData->hEndPointsCS != NULL)
	{
		xnOSCloseCriticalSection(&pDevicePrivateData->hEndPointsCS);
		pDevicePrivateData->hEndPointsCS = NULL;
	}

	// free buffers
	XnDeviceSensorFreeBuffers(pDevicePrivateData);

	if (pDevicePrivateData->hExecuteMutex != NULL)
	{
		xnOSCloseMutex(&pDevicePrivateData->hExecuteMutex);
		pDevicePrivateData->hExecuteMutex = NULL;
	}

	// Register USB event callback
#if WIN32
	nRetVal = m_SensorIO.SetCallback(NULL, this);
	XN_IS_STATUS_OK(nRetVal);
#endif

	XnDeviceBase::Destroy();

	// close dumps
	xnDumpFileClose(pDevicePrivateData->TimestampsDump);
	xnDumpFileClose(pDevicePrivateData->BandwidthDump);
	xnDumpFileClose(pDevicePrivateData->MiniPacketsDump);
	xnDumpFileClose(m_FrameSyncDump);


	m_Firmware.Free();

	return (XN_STATUS_OK);
}

XnStatus XnSensor::CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnDeviceBase::CreateDeviceModule(ppModuleHolder);
	XN_IS_STATUS_OK(nRetVal);

	// add sensor properties
	XnDeviceModule* pModule = (*ppModuleHolder)->GetModule();
	XnProperty* pProps[] = 
	{ 
		&m_ErrorState, &m_ResetSensorOnStartup, &m_LeanInit, &m_Interface, &m_ReadFromEP1,
		&m_ReadFromEP2, &m_ReadFromEP3, &m_ReadData, &m_FirmwareParam, 
		&m_CmosBlankingUnits, &m_CmosBlankingTime, &m_Reset, &m_FirmwareMode, &m_Version, 
		&m_FixedParam, &m_FrameSync, &m_CloseStreamsOnShutdown, &m_InstancePointer, &m_ID,
		&m_USBPath, &m_DeviceName, &m_VendorSpecificData, &m_AudioSupported, &m_ImageSupported,
		&m_ImageControl, &m_DepthControl, &m_AHB, &m_HostTimestamps, &m_PlatformString,
	};

	nRetVal = pModule->AddProperties(pProps, sizeof(pProps)/sizeof(XnProperty*));
	if (nRetVal != XN_STATUS_OK)
	{
		DestroyModule(*ppModuleHolder);
		*ppModuleHolder = NULL;
		return (nRetVal);
	}

	// configure it from global file
	if (m_strGlobalConfigFile[0] != '\0')
	{
		nRetVal = pModule->LoadConfigFromFile(m_strGlobalConfigFile);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::CreateStreamImpl(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnDeviceBase::CreateStreamImpl(strType, strName, pInitialSet);
	XN_IS_STATUS_OK(nRetVal);

	// and configure it from global config file
	nRetVal = ConfigureModuleFromGlobalFile(strName, strType);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// make sure reading from streams is turned on
	if (!m_ReadData.GetValue())
	{
		nRetVal = m_ReadData.SetValue(TRUE);
		XN_IS_STATUS_OK(nRetVal);
	}

	XnDeviceStream* pStream;
	XnSensorStreamHelper* pHelper;

	// create stream
	if (strcmp(StreamType, XN_STREAM_TYPE_DEPTH) == 0)
	{
		XnSensorDepthStream* pDepthStream;
		XN_VALIDATE_NEW(pDepthStream, XnSensorDepthStream, StreamName, &m_Objects);
		pStream = pDepthStream;
		pHelper = pDepthStream->GetHelper();
	}
	else if (strcmp(StreamType, XN_STREAM_TYPE_IMAGE) == 0)
	{
		XnSensorImageStream* pImageStream;
		XN_VALIDATE_NEW(pImageStream, XnSensorImageStream, StreamName, &m_Objects);
		pStream = pImageStream;
		pHelper = pImageStream->GetHelper();
	}
	else if (strcmp(StreamType, XN_STREAM_TYPE_IR) == 0)
	{
		XnSensorIRStream* pIRStream;
		XN_VALIDATE_NEW(pIRStream, XnSensorIRStream, StreamName, &m_Objects);
		pStream = pIRStream;
		pHelper = pIRStream->GetHelper();
	}
	else if (strcmp(StreamType, XN_STREAM_TYPE_AUDIO) == 0)
	{
		if (!m_Firmware.GetInfo()->bAudioSupported)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_UNSUPPORTED_STREAM, XN_MASK_DEVICE_SENSOR, "Audio is not supported by this FW!");
		}

		// TODO: use the allow other users property when constructing the audio stream
		XnSensorAudioStream* pAudioStream;
		XN_VALIDATE_NEW(pAudioStream, XnSensorAudioStream, GetUSBPath(), StreamName, &m_Objects, FALSE);
		pStream = pAudioStream;
		pHelper = pAudioStream->GetHelper();
	}
	else
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_UNSUPPORTED_STREAM, XN_MASK_DEVICE_SENSOR, "Unsupported stream type: %s", StreamType);
	}

	*ppStreamHolder = XN_NEW(XnSensorStreamHolder, pStream, pHelper);

	return (XN_STATUS_OK);
}

void XnSensor::DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder)
{
	XN_DELETE(pStreamHolder->GetModule());
	XN_DELETE(pStreamHolder);
}

XnStatus XnSensor::OpenAllStreams()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Opening all streams...");

	// take a list of all the streams
	const XnChar* astrStreams[XN_SENSOR_MAX_STREAM_COUNT];
	XnUInt32 nStreamCount = XN_SENSOR_MAX_STREAM_COUNT;
	XnDeviceStream* apStreams[XN_SENSOR_MAX_STREAM_COUNT];
	XnSensorStreamHolder* apSensorStreams[XN_SENSOR_MAX_STREAM_COUNT];

	nRetVal = GetStreamNames(astrStreams, &nStreamCount);
	XN_IS_STATUS_OK(nRetVal);

	for (XnUInt32 i = 0; i < nStreamCount; ++i)
	{
		XnDeviceModuleHolder* pHolder;
		nRetVal = FindStream(astrStreams[i], &pHolder);
		XN_IS_STATUS_OK(nRetVal);

		apSensorStreams[i] = (XnSensorStreamHolder*)(pHolder);
		apStreams[i] = apSensorStreams[i]->GetStream();
	}

	// NOTE: the following is an ugly patch. When depth and IR both exist, Depth stream MUST be configured
	// and opened BEFORE IR stream. So, generally, if one of the streams is depth, we move it to be first.
	for (XnUInt32 i = 1; i < nStreamCount; ++i)
	{
		if (strcmp(apStreams[i]->GetType(), XN_STREAM_TYPE_DEPTH) == 0)
		{
			// switch it with the one in location 0
			const XnChar* strTempName = astrStreams[0];
			XnDeviceStream* pTempStream = apStreams[0];
			XnSensorStreamHolder* pTempHolder = apSensorStreams[0];

			astrStreams[0] = astrStreams[i];
			apStreams[0] = apStreams[i];
			apSensorStreams[0] = apSensorStreams[i];

			astrStreams[i] = strTempName;
			apStreams[i] = pTempStream;
			apSensorStreams[i] = pTempHolder;
			break;
		}
	}

	// now configure them all
	for (XnUInt32 i = 0; i < nStreamCount; ++i)
	{
		if (!apStreams[i]->IsOpen())
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Configuring stream %s...", apStreams[i]->GetName());
			nRetVal = apSensorStreams[i]->Configure();
			XN_IS_STATUS_OK(nRetVal);
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Stream %s is configured", apStreams[i]->GetName());
		}
		else
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Stream %s is already open.", apStreams[i]->GetName());
		}
	}

	// and open them all
	for (XnUInt32 i = 0; i < nStreamCount; ++i)
	{
		if (!apStreams[i]->IsOpen())
		{
			nRetVal = apSensorStreams[i]->FinalOpen();
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::ReadStream(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check internal state
	nRetVal = CheckIfReadingAllowed();
	XN_IS_STATUS_OK(nRetVal);

	// call base
	nRetVal = XnDeviceBase::ReadStream(pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::GetBufferPool(const XnChar* strStream, XnBufferPool** ppBufferPool)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDeviceModuleHolder* pHolder;
	nRetVal = FindStream(strStream, &pHolder);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorStreamHolder* pSensorStreamHolder = (XnSensorStreamHolder*)(pHolder);
	XnDeviceStream* pStream = pSensorStreamHolder->GetStream();

	XnUInt64 nFrameBased;
	nRetVal = pStream->GetProperty(XN_STREAM_PROPERTY_IS_FRAME_BASED, &nFrameBased);
	XN_IS_STATUS_OK(nRetVal);

	if (nFrameBased == 0)
	{
		return XN_STATUS_BAD_TYPE;
	}

	XnFrameStream* pFrameStream = (XnFrameStream*)pStream;
	*ppBufferPool = pFrameStream->GetBufferPool();
	
	return (XN_STATUS_OK);
}


XnStatus XnSensor::GetStream(const XnChar* strStream, XnDeviceStream** ppStream)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModuleHolder* pHolder;
	nRetVal = FindStream(strStream, &pHolder);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorStreamHolder* pSensorStreamHolder = (XnSensorStreamHolder*)(pHolder);
	*ppStream = pSensorStreamHolder->GetStream();

	return XN_STATUS_OK;
}

XnBool XnSensor::HasSynchedFrameArrived(const XnChar* strDepthStream, const XnChar* strImageStream)
{
	// find both streams
	XnDeviceStream* pDepth;
	XnDeviceStream* pImage;

	if (XN_STATUS_OK != FindStream(strDepthStream, &pDepth))
		return FALSE;

	if (XN_STATUS_OK != FindStream(strImageStream, &pImage))
		return FALSE;

	XnUInt32 nThreshold = XN_SENSOR_FRAME_SYNC_MAX_DIFF;
	if (IsHighResTimestamps())
		nThreshold *= 1000;

	// wait for both to advance, and time difference to be less than threshold
	XnInt32 nTimestampDiff = XnInt32(pDepth->GetLastTimestamp() - pImage->GetLastTimestamp());

	XnBool bConditionMet = (
		pDepth->IsNewDataAvailable() &&
		pImage->IsNewDataAvailable() &&
		(XnUInt32)abs(nTimestampDiff) <= nThreshold
		);

	if (xnLogIsDumpMaskEnabled(XN_DUMP_FRAME_SYNC))
	{
		XnUInt64 nNow;
		xnOSGetHighResTimeStamp(&nNow);
		xnDumpFileWriteString(m_FrameSyncDump, "%llu,%u,%llu,%u,%llu,%s\n",
			nNow,
			pDepth->IsNewDataAvailable(),
			pDepth->GetLastTimestamp(),
			pImage->IsNewDataAvailable(),
			pImage->GetLastTimestamp(),
			bConditionMet ? "OK" : "Waiting");
	}

	return bConditionMet;
}

XnBool XnSensor::HasSynchedFrameArrived(void* pCookie)
{
	XnWaitForSycnhedFrameData* pData = (XnWaitForSycnhedFrameData*)pCookie;
	return pData->pThis->HasSynchedFrameArrived(pData->strDepthStream, pData->strImageStream);
}

XnStatus XnSensor::WaitForPrimaryStream(XN_EVENT_HANDLE hNewDataEvent, XnStreamDataSet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_FrameSync.GetValue() == TRUE)
	{
		// FrameSync is on. check if we have both the image and the depth stream
		XnStreamData* apStreamData[XN_DEVICE_BASE_MAX_STREAMS_COUNT];
		XnUInt32 nCount = XN_DEVICE_BASE_MAX_STREAMS_COUNT;

		nRetVal = XnStreamDataSetCopyToArray(pSet, apStreamData, &nCount);
		XN_IS_STATUS_OK(nRetVal);

		const XnChar* strImageName = NULL;
		const XnChar* strDepthName = NULL;
		XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];

		for (XnUInt32 i = 0; i < nCount; ++i)
		{
			nRetVal = GetProperty(apStreamData[i]->StreamName, XN_STREAM_PROPERTY_TYPE, strType);
			XN_IS_STATUS_OK(nRetVal);

			if (strcmp(strType, XN_STREAM_TYPE_DEPTH) == 0)
			{
				strDepthName = apStreamData[i]->StreamName;
			}
			else if (strcmp(strType, XN_STREAM_TYPE_IMAGE) == 0)
			{
				strImageName = apStreamData[i]->StreamName;
			}

			// if both are present, wait for frame sync
			if (strImageName != NULL && strDepthName != NULL)
			{
				XnWaitForSycnhedFrameData WaitData;
				WaitData.pThis = this;
				WaitData.strDepthStream = strDepthName;
				WaitData.strImageStream = strImageName;

				nRetVal = xnOSWaitForCondition(hNewDataEvent, XN_DEVICE_READ_FRAME_TIMEOUT, &XnSensor::HasSynchedFrameArrived, &WaitData);
				if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
				{
					xnLogError(XN_MASK_DDK, "Not responding - Didn't get any synced frame");
					return (XN_STATUS_DEVICE_FRAMES_NOT_SYNCHED);
				}
				else
				{
					XN_IS_STATUS_OK(nRetVal);
				}
			}
		} // for loop
	}

	// if we reached here, either frame sync is off, or one of the streams is not read from.
	// either way, we should just wait for the primary stream.
	nRetVal = XnDeviceBase::WaitForPrimaryStream(hNewDataEvent, pSet);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::Read(XnStreamDataSet* pStreamOutputSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check internal state
	nRetVal = CheckIfReadingAllowed();
	XN_IS_STATUS_OK(nRetVal);

	// call base
	nRetVal = XnDeviceBase::Read(pStreamOutputSet);
	XN_IS_STATUS_OK(nRetVal);

	m_FPS.MarkReadCalled();

	return (XN_STATUS_OK);
}

XnStatus XnSensor::WriteStream(XnStreamData* /*pStreamOutput*/)
{
	return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
}

XnStatus XnSensor::Write(XnStreamDataSet* /*pStreamOutputSet*/)
{
	return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
}

XnStatus XnSensor::Seek(XnUInt64 /*nTimestamp*/)
{
	return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
}

XnStatus XnSensor::SeekFrame(XnUInt32 /*nFrameID*/)
{
	return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
}

XnStatus XnSensor::LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(csINIFilePath);
	XN_VALIDATE_INPUT_PTR(csSectionName);

	// we first need to configure the USB interface (we want to do so BEFORE creating streams)
	nRetVal = m_Interface.ReadValueFromFile(csINIFilePath, XN_MODULE_NAME_DEVICE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_ReadFromEP1.ReadValueFromFile(csINIFilePath, XN_MODULE_NAME_DEVICE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_ReadFromEP2.ReadValueFromFile(csINIFilePath, XN_MODULE_NAME_DEVICE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_ReadFromEP3.ReadValueFromFile(csINIFilePath, XN_MODULE_NAME_DEVICE);
	XN_IS_STATUS_OK(nRetVal);

	// now create all streams
	nRetVal = CreateStreamsFromFile(csINIFilePath, csSectionName);
	XN_IS_STATUS_OK(nRetVal);

	// now configure DEVICE module (primary stream, global mirror, etc.)
	nRetVal = DeviceModule()->LoadConfigFromFile(csINIFilePath, XN_MODULE_NAME_DEVICE);
	XN_IS_STATUS_OK(nRetVal);

	// and now configure the streams
	XnDeviceModuleHolderList streams;
	nRetVal = GetStreamsList(streams);
	XN_IS_STATUS_OK(nRetVal);

	for (XnDeviceModuleHolderList::Iterator it = streams.Begin(); it != streams.End(); ++it)
	{
		XnDeviceModuleHolder* pHolder = *it;
		nRetVal = pHolder->GetModule()->LoadConfigFromFile(csINIFilePath);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::CheckIfReadingAllowed()
{
	return GetErrorState();
}

XnStatus XnSensor::InitReading()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// open data endpoints
	nRetVal = m_SensorIO.OpenDataEndPoints((XnSensorUsbInterface)m_Interface.GetValue(), *m_Firmware.GetInfo());
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Interface.UnsafeUpdateValue(m_SensorIO.GetCurrentInterface());
	XN_IS_STATUS_OK(nRetVal);

	// take frequency information
	XnFrequencyInformation FrequencyInformation;

	nRetVal = XnHostProtocolAlgorithmParams(&m_DevicePrivateData, XN_HOST_PROTOCOL_ALGORITHM_FREQUENCY, &FrequencyInformation, sizeof(XnFrequencyInformation), (XnResolutions)0, 0);
	if (nRetVal != XN_STATUS_OK)
		return nRetVal;

	m_DevicePrivateData.fDeviceFrequency = XN_PREPARE_VAR_FLOAT_IN_BUFFER(FrequencyInformation.fDeviceFrequency);

	// Init Dumps
	m_DevicePrivateData.BandwidthDump = xnDumpFileOpen(XN_DUMP_BANDWIDTH, "Bandwidth.csv");
	xnDumpFileWriteString(m_DevicePrivateData.BandwidthDump, "Timestamp,Frame Type,Frame ID,Size\n");
	m_DevicePrivateData.TimestampsDump = xnDumpFileOpen(XN_DUMP_TIMESTAMPS, "Timestamps.csv");
	xnDumpFileWriteString(m_DevicePrivateData.TimestampsDump, "Host Time (us),Stream,Device TS,Time (ms),Comments\n");
	m_DevicePrivateData.MiniPacketsDump = xnDumpFileOpen(XN_DUMP_MINI_PACKETS, "MiniPackets.csv");
	xnDumpFileWriteString(m_DevicePrivateData.MiniPacketsDump, "HostTS,Type,ID,Size,Timestamp\n");

	m_DevicePrivateData.nGlobalReferenceTS = 0;
	nRetVal = xnOSCreateCriticalSection(&m_DevicePrivateData.hEndPointsCS);
	XN_IS_STATUS_OK(nRetVal);

	// NOTE: when we go up, some streams might be open, and so we'll receive lots of garbage.
	// wait till streams are turned off, and then start reading.
//	pDevicePrivateData->bIgnoreDataPackets = TRUE;

	// open input threads
	nRetVal = XnDeviceSensorOpenInputThreads(GetDevicePrivateData(), (XnBool)m_ReadFromEP1.GetValue(), (XnBool)m_ReadFromEP2.GetValue(), (XnBool)m_ReadFromEP3.GetValue());
	XN_IS_STATUS_OK(nRetVal);


	return XN_STATUS_OK;
}


XnStatus XnSensor::ValidateSensorID(XnChar* csSensorID)
{
	if (strcmp(csSensorID, XN_DEVICE_SENSOR_DEFAULT_ID) != 0)
	{
		if (strcmp(csSensorID, GetFixedParams()->GetSensorSerial()) != 0)
		{
			return (XN_STATUS_IO_DEVICE_WRONG_SERIAL);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::ResolveGlobalConfigFileName(XnChar* strConfigFile, XnUInt32 nBufSize, const XnChar* strConfigDir)
{
	XnUInt32 nWritten = 0;
	return xnOSStrFormat(strConfigFile, nBufSize, &nWritten, "%s%s%s", strConfigDir, XN_FILE_DIR_SEP, XN_GLOBAL_CONFIG_FILE_NAME);
}

XnStatus XnSensor::SetGlobalConfigFile(const XnChar* strConfigFile)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnOSStrCopy(m_strGlobalConfigFile, strConfigFile, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	XnBool bExists;
	nRetVal = xnOSDoesFileExist(m_strGlobalConfigFile, &bExists);
	XN_IS_STATUS_OK(nRetVal);

	if (!bExists)
	{
		xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Global configuration file '%s' was not found.", m_strGlobalConfigFile);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::ConfigureModuleFromGlobalFile(const XnChar* strModule, const XnChar* strSection /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(strModule, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->LoadConfigFromFile(m_strGlobalConfigFile, strSection);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::GetFirmwareParam(XnInnerParamData* pParam)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnHostProtocolGetParam(&m_DevicePrivateData, pParam->nParam, pParam->nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::ReadAHB(XnAHBData* pAHB)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnHostProtocolReadAHB(&m_DevicePrivateData, pAHB->nRegister, pAHB->nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::WriteAHB(const XnAHBData* pAHB)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnHostProtocolWriteAHB(&m_DevicePrivateData, pAHB->nRegister, pAHB->nValue, pAHB->nMask);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}


XnStatus XnSensor::GetCmosBlankingUnits(XnCmosBlankingUnits* pBlanking)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_Firmware.GetInfo()->nFWVer < XN_SENSOR_FW_VER_5_1)
	{
		return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
	}

	nRetVal = XnHostProtocolGetCmosBlanking(&m_DevicePrivateData, pBlanking->nCmosID, &pBlanking->nUnits);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::GetCmosBlankingTime(XnCmosBlankingTime* pBlanking)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check version
	if (m_Firmware.GetInfo()->nFWVer < XN_SENSOR_FW_VER_5_1)
	{
		return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
	}

	// get value in units
	XnCmosBlankingUnits blankingUnits;
	blankingUnits.nCmosID = pBlanking->nCmosID;
	nRetVal = GetCmosBlankingUnits(&blankingUnits);
	XN_IS_STATUS_OK(nRetVal);

	// get coefficients
	const XnCmosBlankingCoefficients* pCoeffs = m_CmosInfo.GetBlankingCoefficients(pBlanking->nCmosID);

	// translate to time
	pBlanking->nTimeInMilliseconds = (pCoeffs->fA * blankingUnits.nUnits + pCoeffs->fB)/1000;
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::GetFirmwareMode(XnParamCurrentMode* pnMode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_Firmware.GetInfo()->nFWVer == XN_SENSOR_FW_VER_0_17)
	{
		*pnMode = m_Firmware.GetInfo()->nCurrMode;
	}
	else
	{
		XnUInt16 nMode;
		nRetVal = XnHostProtocolGetMode(&m_DevicePrivateData, nMode);
		XN_IS_STATUS_OK(nRetVal);

		switch (nMode)
		{
		case XN_HOST_PROTOCOL_MODE_PS:
			*pnMode = XN_MODE_PS;
			break;
		case XN_HOST_PROTOCOL_MODE_MAINTENANCE:
			*pnMode = XN_MODE_MAINTENANCE;
			break;
		case XN_HOST_PROTOCOL_MODE_SAFE_MODE:
			*pnMode = XN_MODE_SAFE_MODE;
			break;
		default:
			printf("Got Unknown Firmware Mode %d\n", nMode);
			return XN_STATUS_DEVICE_BAD_PARAM;
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::GetDepthCmosRegister(XnControlProcessingData* pRegister)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_Firmware.GetInfo()->nFWVer >= XN_SENSOR_FW_VER_3_0)
	{
		nRetVal = XnHostProtocolGetCMOSRegisterI2C(&m_DevicePrivateData, XN_CMOS_TYPE_DEPTH, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = XnHostProtocolGetCMOSRegister(&m_DevicePrivateData, XN_CMOS_TYPE_DEPTH, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::GetImageCmosRegister(XnControlProcessingData* pRegister)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_Firmware.GetInfo()->nFWVer >= XN_SENSOR_FW_VER_3_0)
	{
		nRetVal = XnHostProtocolGetCMOSRegisterI2C(&m_DevicePrivateData, XN_CMOS_TYPE_IMAGE, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = XnHostProtocolGetCMOSRegister(&m_DevicePrivateData, XN_CMOS_TYPE_IMAGE, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}


XnStatus XnSensor::GetFixedParams(XnDynamicSizeBuffer* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (pBuffer->nMaxSize < sizeof(XnFixedParams))
	{
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
	}

	XnFixedParams fixed;
	nRetVal = XnHostProtocolGetFixedParams(GetDevicePrivateData(), fixed);
	XN_IS_STATUS_OK(nRetVal);

	xnOSMemCopy(pBuffer->pData, &fixed, sizeof(XnFixedParams));
	pBuffer->nDataSize = sizeof(XnFixedParams);
	
	return (XN_STATUS_OK);
}


XnStatus XnSensor::SetErrorState(XnStatus errorState)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (errorState != GetErrorState())
	{
		if (errorState == XN_STATUS_OK)
		{
			xnLogInfo(XN_MASK_DEVICE_SENSOR, "Device is back to normal state.");
		}
		else
		{
			xnLogError(XN_MASK_DEVICE_SENSOR, "Device has entered error mode: %s", xnGetStatusString(errorState));
		}

		nRetVal = m_ErrorState.UnsafeUpdateValue((XnUInt64)errorState);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetInterface(XnSensorUsbInterface nInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// we don't allow change if requested value is specific and different than current
	if (m_ReadData.GetValue() == TRUE &&
		nInterface != XN_SENSOR_USB_INTERFACE_DEFAULT &&
		nInterface != m_SensorIO.GetCurrentInterface())
	{
		return (XN_STATUS_DEVICE_PROPERTY_READ_ONLY);
	}

	nRetVal = m_Interface.UnsafeUpdateValue(nInterface);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetHostTimestamps(XnBool bHostTimestamps)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// we don't allow change if requested value is specific and different than current
	if (m_ReadData.GetValue() == TRUE &&
		bHostTimestamps != m_HostTimestamps.GetValue())
	{
		return (XN_STATUS_DEVICE_PROPERTY_READ_ONLY);
	}

	nRetVal = m_HostTimestamps.UnsafeUpdateValue(bHostTimestamps);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetReadEndpoint1(XnBool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// This is a special func. It can only be changed BEFORE reading starts
	if (m_ReadData.GetValue() == FALSE)
	{
		nRetVal = m_ReadFromEP1.UnsafeUpdateValue(bRead);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// check it's the same value
		if (bRead != m_ReadFromEP1.GetValue())
		{
			return (XN_STATUS_DEVICE_PROPERTY_READ_ONLY);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetReadEndpoint2(XnBool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// This is a special func. It can only be changed BEFORE reading starts
	if (m_ReadData.GetValue() == FALSE)
	{
		nRetVal = m_ReadFromEP2.UnsafeUpdateValue(bRead);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// check it's the same value
		if (bRead != m_ReadFromEP2.GetValue())
		{
			return (XN_STATUS_DEVICE_PROPERTY_READ_ONLY);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetReadEndpoint3(XnBool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// This is a special func. It can only be changed BEFORE reading starts
	if (m_ReadData.GetValue() == FALSE)
	{
		nRetVal = m_ReadFromEP3.UnsafeUpdateValue(bRead);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// check it's the same value
		if (bRead != m_ReadFromEP3.GetValue())
		{
			return (XN_STATUS_DEVICE_PROPERTY_READ_ONLY);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetReadData(XnBool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (!bRead)
	{
		return XN_STATUS_ERROR;
	}
	else
	{
		nRetVal = InitReading();
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_ReadData.UnsafeUpdateValue(TRUE);
		XN_IS_STATUS_OK(nRetVal);

		// no longer needed
		m_ReadData.UpdateSetCallback(NULL, NULL);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetDepthCmosRegister(const XnControlProcessingData* pRegister)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_Firmware.GetInfo()->nFWVer >= XN_SENSOR_FW_VER_3_0)
	{
		nRetVal = XnHostProtocolSetCMOSRegisterI2C(&m_DevicePrivateData, XN_CMOS_TYPE_DEPTH, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = XnHostProtocolSetCMOSRegister(&m_DevicePrivateData, XN_CMOS_TYPE_DEPTH, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetImageCmosRegister(const XnControlProcessingData* pRegister)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_Firmware.GetInfo()->nFWVer >= XN_SENSOR_FW_VER_3_0)
	{
		nRetVal = XnHostProtocolSetCMOSRegisterI2C(&m_DevicePrivateData, XN_CMOS_TYPE_IMAGE, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = XnHostProtocolSetCMOSRegister(&m_DevicePrivateData, XN_CMOS_TYPE_IMAGE, pRegister->nRegister, pRegister->nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}


XnStatus XnSensor::SetFirmwareParam(const XnInnerParamData* pParam)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnHostProtocolSetParam(&m_DevicePrivateData, pParam->nParam, pParam->nValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}


XnStatus XnSensor::SetCmosBlankingUnits(const XnCmosBlankingUnits* pBlanking)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_Firmware.GetInfo()->nFWVer < XN_SENSOR_FW_VER_5_1)
	{
		return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
	}

	nRetVal = XnHostProtocolSetCmosBlanking(&m_DevicePrivateData, pBlanking->nUnits, pBlanking->nCmosID, pBlanking->nNumberOfFrames);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetCmosBlankingTime(const XnCmosBlankingTime* pBlanking)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check version
	if (m_Firmware.GetInfo()->nFWVer < XN_SENSOR_FW_VER_5_1)
	{
		return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
	}

	// get coefficients
	const XnCmosBlankingCoefficients* pCoeffs = m_CmosInfo.GetBlankingCoefficients(pBlanking->nCmosID);

	// translate to units request
	XnCmosBlankingUnits blankingUnits;
	blankingUnits.nCmosID = pBlanking->nCmosID;
	blankingUnits.nNumberOfFrames = pBlanking->nNumberOfFrames;
	blankingUnits.nUnits = XnUInt16((pBlanking->nTimeInMilliseconds*1000 - pCoeffs->fB)/pCoeffs->fA);

	nRetVal = SetCmosBlankingUnits(&blankingUnits);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensor::Reset(XnParamResetType nType)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnHostProtocolReset(&m_DevicePrivateData, (XnUInt16)nType);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensor::SetFirmwareMode(XnParamCurrentMode nMode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_Firmware.GetInfo()->nFWVer == XN_SENSOR_FW_VER_0_17)
	{
		m_Firmware.GetInfo()->nCurrMode = nMode;
		return (XN_STATUS_OK);
	}

	XnHostProtocolModeType nActualValue;

	switch (nMode)
	{
	case XN_MODE_PS:
		nActualValue = XN_HOST_PROTOCOL_MODE_PS;
		break;
	case XN_MODE_MAINTENANCE:
		nActualValue = XN_HOST_PROTOCOL_MODE_MAINTENANCE;
		break;
	default:
		return XN_STATUS_DEVICE_UNSUPPORTED_MODE;
	}

	nRetVal = XnHostProtocolSetMode(&m_DevicePrivateData, (XnUInt16)nActualValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}


XnStatus XnSensor::OnFrameSyncPropertyChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_ReadData.GetValue() == TRUE)
	{
		// decide firmware frame sync - both streams are on, and user asked for it
		XnBool bFrameSync = (
			m_FrameSync.GetValue() == TRUE &&
			GetFirmware()->GetParams()->m_Stream0Mode.GetValue() == XN_VIDEO_STREAM_COLOR &&
			GetFirmware()->GetParams()->m_Stream1Mode.GetValue() == XN_VIDEO_STREAM_DEPTH
			);

		nRetVal = GetFirmware()->GetParams()->m_FrameSyncEnabled.SetValue(bFrameSync);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetInterfaceCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->XnSensor::SetInterface((XnSensorUsbInterface)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetHostTimestampsCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->XnSensor::SetHostTimestamps(nValue == 1);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetReadEndpoint1Callback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->XnSensor::SetReadEndpoint1((XnBool)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetReadEndpoint2Callback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->XnSensor::SetReadEndpoint2((XnBool)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetReadEndpoint3Callback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->XnSensor::SetReadEndpoint3((XnBool)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetReadDataCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->XnSensor::SetReadData((XnBool)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetFirmwareParamCallback(XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnInnerParamData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->SetFirmwareParam((const XnInnerParamData*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetCmosBlankingUnitsCallback(XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnCmosBlankingUnits);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->SetCmosBlankingUnits((const XnCmosBlankingUnits*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetCmosBlankingTimeCallback(XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnCmosBlankingTime);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->SetCmosBlankingTime((const XnCmosBlankingTime*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::ResetCallback(XnIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->Reset((XnParamResetType)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetFirmwareModeCallback(XnIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->SetFirmwareMode((XnParamCurrentMode)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetFirmwareParamCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnInnerParamData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->GetFirmwareParam((XnInnerParamData*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetCmosBlankingUnitsCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnCmosBlankingUnits);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->GetCmosBlankingUnits((XnCmosBlankingUnits*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetCmosBlankingTimeCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnCmosBlankingTime);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->GetCmosBlankingTime((XnCmosBlankingTime*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetFirmwareModeCallback(const XnIntProperty* /*pSender*/, XnUInt64* pnValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	XnParamCurrentMode nMode;
	XnStatus nRetVal = pThis->GetFirmwareMode(&nMode);
	XN_IS_STATUS_OK(nRetVal);

	*pnValue = nMode;
	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetAudioSupportedCallback(const XnIntProperty* /*pSender*/, XnUInt64* pnValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	*pnValue = pThis->m_Firmware.GetInfo()->bAudioSupported;
	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetImageSupportedCallback(const XnIntProperty* /*pSender*/, XnUInt64* pnValue, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	*pnValue = pThis->m_Firmware.GetInfo()->bImageSupported;
	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnSensor::FrameSyncPropertyChangedCallback(const XnProperty* /*pSender*/, void* pCookie)
{
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->OnFrameSyncPropertyChanged();
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetFixedParamsCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnDynamicSizeBuffer);
	XnSensor* pThis = (XnSensor*)pCookie;
	XnDynamicSizeBuffer* pBuffer = (XnDynamicSizeBuffer*)gbValue.pData;
	return pThis->GetFixedParams(pBuffer);
}

XnBool XN_CALLBACK_TYPE XnSensor::USBEventCallback(XnUSBEventType USBEventType, XnChar* /*cpDevPath*/, void* pCallbackData)
{
	XnSensor* pXnSensor = (XnSensor*)pCallbackData;
	if (USBEventType == XN_USB_EVENT_DEVICE_DISCONNECT)
	{
		pXnSensor->SetErrorState(XN_STATUS_DEVICE_NOT_CONNECTED);
	}
	/*else
	{
		pXnSensor->SetErrorState(XN_STATUS_OK);
	}*/
	//TODO: Uncomment this once we can deal with re-connections

	return TRUE;
}


XnStatus XN_CALLBACK_TYPE XnSensor::GetInstanceCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	if (gbValue.nDataSize != sizeof(void*))
	{
		return XN_STATUS_DEVICE_PROPERTY_SIZE_DONT_MATCH;
	}

	*(void**)gbValue.pData = pCookie;
	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetDepthCmosRegisterCallback(XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnControlProcessingData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->SetDepthCmosRegister((const XnControlProcessingData*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::SetImageCmosRegisterCallback(XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnControlProcessingData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->SetImageCmosRegister((const XnControlProcessingData*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetDepthCmosRegisterCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnControlProcessingData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->GetDepthCmosRegister((XnControlProcessingData*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::GetImageCmosRegisterCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnControlProcessingData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->GetImageCmosRegister((XnControlProcessingData*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::WriteAHBCallback(XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnAHBData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->WriteAHB((const XnAHBData*)gbValue.pData);
}

XnStatus XN_CALLBACK_TYPE XnSensor::ReadAHBCallback(const XnGeneralProperty* /*pSender*/, const XnGeneralBuffer& gbValue, void* pCookie)
{
	XN_VALIDATE_GENERAL_BUFFER_TYPE(gbValue, XnAHBData);
	XnSensor* pThis = (XnSensor*)pCookie;
	return pThis->ReadAHB((XnAHBData*)gbValue.pData);
}

