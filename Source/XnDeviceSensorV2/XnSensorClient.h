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

	XN_SOCKET_HANDLE m_hSocket;
	XN_EVENT_HANDLE m_hReplyEvent;
	XN_THREAD_HANDLE m_hListenThread;
	volatile XnBool m_bShouldRun;
	XnDataPacker* m_pOutgoingPacker;
	XnSensorClientServerReply m_LastReply;
	XnBool m_bConnected;

	XnSensorClient* m_pThis;
	XnActualGeneralProperty m_InstancePointer;
	XnActualIntProperty m_ErrorState;

	XN_CRITICAL_SECTION_HANDLE m_hLock;
	XnChar m_strConfigDir[XN_FILE_MAX_PATH];
};

#endif //__XN_SENSOR_CLIENT_H__
