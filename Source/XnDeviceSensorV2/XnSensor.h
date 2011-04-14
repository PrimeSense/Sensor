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





#ifndef __XN_SENSOR_H__
#define __XN_SENSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnDeviceBase.h>
#include "XnDeviceSensorIO.h"
#include "XnParams.h"
#include "XnDeviceSensor.h"
#include "XnSensorFixedParams.h"
#include "XnSensorFirmwareParams.h"
#include <XnDDK/XnDeviceStream.h>
#include "XnSensorFirmware.h"
#include "XnCmosInfo.h"
#include "IXnSensorStream.h"
#include <XnDDK/XnIntPropertySynchronizer.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_PROPERTY_INSTANCE_POINTER	"InstancePointer"

//---------------------------------------------------------------------------
// XnSensor class
//---------------------------------------------------------------------------
class XnSensor : public XnDeviceBase
{
	friend class XnServerSensorInvoker;

public:
	XnSensor();
	~XnSensor();

	static XnStatus GetDefinition(XnDeviceDefinition* pDeviceDefinition);
	static XnStatus Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount);

	virtual XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus Destroy();
	virtual XnStatus OpenAllStreams();
	virtual XnStatus ReadStream(XnStreamData* pStreamOutput);
	virtual XnStatus Read(XnStreamDataSet* pStreamOutputSet);
	virtual XnStatus WriteStream(const XnStreamData* pStreamOutput);
	virtual XnStatus Write(const XnStreamDataSet* pStreamOutputSet);
	virtual XnStatus Seek(XnUInt64 nTimestamp);
	virtual XnStatus SeekFrame(XnUInt32 nFrameID);
	virtual XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName);

public:
	inline const XnSensorFixedParams* GetFixedParams() const { return &m_FixedParams; }
	inline XnSensorFirmware* GetFirmware() { return &m_Firmware; }
	inline XnSensorFPS* GetFPSCalculator() { return &m_FPS; }

	XnStatus SetCmosConfiguration(XnCMOSType nCmos, XnResolutions nRes, XnUInt32 nFPS);

	inline XnDevicePrivateData* GetDevicePrivateData() { return &m_DevicePrivateData; }

	XnStatus ConfigPropertyFromFile(XnStringProperty* pProperty, const XnChar* csINIFilePath, const XnChar* csSectionName);
	XnStatus ConfigPropertyFromFile(XnIntProperty* pProperty, const XnChar* csINIFilePath, const XnChar* csSectionName);

	inline XnBool IsMiscSupported() const { return m_SensorIO.IsMiscEndpointSupported(); }
	inline XnBool IsLowBandwidth() const { return m_SensorIO.IsLowBandwidth(); }

	XnStatus GetSharedBufferPool(const XnChar* strStream, XnSharedMemoryBufferPool** ppBufferPool);

	inline XnStatus GetErrorState() { return (XnStatus)m_ErrorState.GetValue(); }
	XnStatus SetErrorState(XnStatus errorState);

	static XnStatus ResolveGlobalConfigFileName(XnChar* strConfigFile, XnUInt32 nBufSize, const XnChar* strConfigDir);
	XnStatus SetGlobalConfigFile(const XnChar* strConfigFile);
	XnStatus ConfigureModuleFromGlobalFile(const XnChar* strModule, const XnChar* strSection = NULL);

	const XnChar* GetUSBPath() { return m_USBPath.GetValue(); }


protected:
	virtual XnStatus CreateStreamImpl(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialSet);

	XnStatus CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder);
	XnStatus CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStream);
	void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder);

	XnStatus WaitForPrimaryStream(XN_EVENT_HANDLE hNewDataEvent, XnStreamDataSet* pSet);

private:
	XnStatus InitSensor(const XnDeviceConfig* pDeviceConfig);
	XnStatus ParseConnectionString(const XnChar* csConnectionString, XnChar* csSensorID, XnUInt32* pnBoardID);
	XnStatus ValidateSensorID(XnChar* csSensorID);
	XnStatus ReadFromStreamImpl(XnDeviceStream* pStream, XnStreamData* pStreamOutput);
	XnStatus SetMirrorForModule(XnDeviceModule* pModule, XnUInt64 nValue);
	XnStatus FindSensorStream(const XnChar* StreamName, IXnSensorStream** ppStream);
	XnStatus CheckIfReadingAllowed();
	XnStatus InitReading();
	XnBool HasSynchedFrameArrived(const XnChar* strDepthStream, const XnChar* strImageStream);
	XnStatus OnFrameSyncPropertyChanged();


	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	XnStatus GetFirmwareParam(XnInnerParamData* pParam);
	XnStatus GetCmosBlankingUnits(XnCmosBlankingUnits* pBlanking);
	XnStatus GetCmosBlankingTime(XnCmosBlankingTime* pBlanking);
	XnStatus GetFirmwareMode(XnParamCurrentMode* pnMode);
	XnStatus GetLastRawFrame(const XnChar* strStream, XnUChar* pBuffer, XnUInt32 nDataSize);
	XnStatus GetFixedParams(XnDynamicSizeBuffer* pBuffer);


	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetInterface(XnSensorUsbInterface nInterface);
	XnStatus SetNumberOfBuffers(XnUInt32 nCount);
	XnStatus SetReadEndpoint1(XnBool bRead);
	XnStatus SetReadEndpoint2(XnBool bRead);
	XnStatus SetReadEndpoint3(XnBool bRead);
	XnStatus SetReadData(XnBool bRead);
	XnStatus SetFirmwareParam(const XnInnerParamData* pParam);
	XnStatus SetCmosBlankingUnits(const XnCmosBlankingUnits* pBlanking);
	XnStatus SetCmosBlankingTime(const XnCmosBlankingTime* pBlanking);
	XnStatus Reset(XnParamResetType nType);
	XnStatus SetFirmwareMode(XnParamCurrentMode nMode);

	//---------------------------------------------------------------------------
	// Callbacks
	//---------------------------------------------------------------------------
	static XnStatus XN_CALLBACK_TYPE SetInterfaceCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetNumberOfBuffersCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetReadEndpoint1Callback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetReadEndpoint2Callback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetReadEndpoint3Callback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetReadDataCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareParamCallback(XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCmosBlankingUnitsCallback(XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCmosBlankingTimeCallback(XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ResetCallback(XnIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareModeCallback(XnIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetFixedParamsCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE FrameSyncPropertyChangedCallback(const XnProperty* pSender, void* pCookie);
	static XnBool XN_CALLBACK_TYPE HasSynchedFrameArrived(void* pCookie);
	static XnBool XN_CALLBACK_TYPE USBEventCallback(XnUSBEventType USBEventType, XnChar* cpDevPath, void* pCallbackData);
	static XnStatus XN_CALLBACK_TYPE GetFirmwareParamCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCmosBlankingUnitsCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCmosBlankingTimeCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetFirmwareModeCallback(const XnIntProperty* pSender, XnUInt64* pnValue, void* pCookie);


	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnActualIntProperty m_ErrorState;
	XnActualIntProperty m_ResetSensorOnStartup;
	XnActualIntProperty m_Interface;
	XnActualIntProperty m_NumberOfBuffers;
	XnActualIntProperty m_ReadFromEP1;
	XnActualIntProperty m_ReadFromEP2;
	XnActualIntProperty m_ReadFromEP3;
	XnActualIntProperty m_ReadData;
	XnActualIntProperty m_FrameSync;
	XnActualIntProperty m_CloseStreamsOnShutdown;
	XnGeneralProperty m_FirmwareParam;
	XnGeneralProperty m_CmosBlankingUnits;
	XnGeneralProperty m_CmosBlankingTime;
	XnIntProperty m_Reset;
	XnIntProperty m_FirmwareMode;
	XnVersions m_VersionData;
	XnActualGeneralProperty m_Version;
	XnGeneralProperty m_FixedParam;
	XnSensor* m_pThis;
	XnActualGeneralProperty m_InstancePointer;
	XnActualStringProperty m_ID;
	XnActualStringProperty m_USBPath;
	XnActualStringProperty m_DeviceName;
	XnActualStringProperty m_VendorSpecificData;

	XnSensorFirmware m_Firmware;
	XnDevicePrivateData m_DevicePrivateData;
	XnSensorFixedParams m_FixedParams;
	XnSensorFPS m_FPS;
	XnCmosInfo m_CmosInfo;
	XnSensorIO m_SensorIO;

	XnSensorObjects m_Objects;


	XnDump m_FrameSyncDump;
	XnBool m_bInitialized;

	XnIntPropertySynchronizer m_PropSynchronizer;

	XnChar m_strGlobalConfigFile[XN_FILE_MAX_PATH];
};

#endif //__XN_SENSOR_H__