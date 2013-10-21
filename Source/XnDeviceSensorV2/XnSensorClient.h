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
#ifndef __XN_SENSOR_CLIENT_H__
#define __XN_SENSOR_CLIENT_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnStreamReaderDevice.h>
#include <XnOS.h>
#include "XnSensorClientServer.h"

//---------------------------------------------------------------------------
// XnSensorClient class
//---------------------------------------------------------------------------
class XnSensorClient : public XnStreamReaderDevice
{
public:
	XnSensorClient();
	virtual ~XnSensorClient();

	static XnStatus GetDefinition(XnDeviceDefinition* pDeviceDefinition);
	static XnStatus Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount);

	XnStatus Init(const XnDeviceConfig* pDeviceConfig);
	XnStatus Destroy();
	XnStatus CreateStream(const XnChar* StreamType, const XnChar* StreamName = NULL, const XnPropertySet* pInitialValues = NULL);
	XnStatus DestroyStream(const XnChar* StreamName);
	XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue);
	XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue);
	XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnChar* strValue);
	XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& Value);
	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue);
	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue);
	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* strValue);
	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& Value);
	XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName);
	XnStatus BatchConfig(const XnPropertySet* pChangeSet);
	XnStatus ReadStream(XnStreamData* pStreamOutput);
	XnStatus Read(XnStreamDataSet* pStreamOutputSet);

	void SetConfigDir(const XnChar* strConfigDir);
	XnBool IsServerFromOtherUserAllowed() { return m_bAllowServerFromOtherUser; }

protected:
	XnStatus SendBye();
	virtual XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus CreateIOStreamImpl(const XnChar* strConnectionString, XnIOStream*& pStream);
	virtual void DestroyIOStreamImpl(XnIOStream* pStream);
	virtual XnStatus ReadInitialState(XnPropertySet* pSet);

	XnStatus CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder);
	XnStatus CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder);
	void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder);

	virtual XnStatus HandlePackedObject(XnPackedDataType nObjectType);
	virtual XnStatus HandleNewStream(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialValues);
	virtual XnStatus HandleGeneralProperty(const XnChar* strModule, const XnChar* strName, const XnGeneralBuffer& gbValue);

	XnStatus WaitForReply(XnSensorServerCustomMessages ExpectedMessage);
	
private:
	friend class XnSensorClientStream;
	friend class XnSensorClientFrameStream;

	XnStatus Listen();
	XnStatus StartServerProcess();

	static XN_THREAD_PROC ListenThread(XN_THREAD_PARAM pThreadParam);
	static XnStatus XN_CALLBACK_TYPE GetInstanceCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);

	XN_SOCKET_HANDLE m_hSocket;
	XN_EVENT_HANDLE m_hReplyEvent;
	XN_THREAD_HANDLE m_hListenThread;
	volatile XnBool m_bShouldRun;
	XnDataPacker* m_pOutgoingPacker;
	XnSensorClientServerReply m_LastReply;
	XnBool m_bConnected;

	XnGeneralProperty m_InstancePointer;
	XnActualIntProperty m_ErrorState;

	XN_CRITICAL_SECTION_HANDLE m_hLock;
	XnChar m_strConfigDir[XN_FILE_MAX_PATH];
	XnChar m_strConfigFile[XN_FILE_MAX_PATH];
	XnBool m_bAllowServerFromOtherUser;
};

#endif //__XN_SENSOR_CLIENT_H__
