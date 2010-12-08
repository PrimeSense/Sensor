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





#ifndef __XN_SENSOR_SERVER_H__
#define __XN_SENSOR_SERVER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include "XnSensor.h"
#include <XnDDK/XnDataPacker.h>
#include "XnSensorClientServer.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_CONFIG_FILE_SERVER_SECTION "Server"

//---------------------------------------------------------------------------
// XnSensorServer class
//---------------------------------------------------------------------------
class XnServerStreamsHash;

class XnSensorServer
{
public:
	XnSensorServer();
	~XnSensorServer();

	XnStatus Run(const XnChar* strConfigFile);
	XnBool IsServerRunning();

private:
	struct XnClient; // Forward Declaration

	XnStatus InitServer(const XnChar* strConfigFile);
	XnStatus ServerMainLoop();
	void ShutdownServer();
	XnStatus OpenSensor(const XnChar* strConnectionString);
	XnStatus SendInitialState(XnClient* pClient);
	XnStatus ServeClient(XnClient* pClient);
	XnStatus SendReply(XnClient* pClient, XnSensorServerCustomMessages Type, XnStatus nRC, XnUInt32 nDataSize = 0, void* pAdditionalData = NULL);
	XnStatus HandleSingleRequest(XnClient* pClient);
	XnStatus HandleOpenSensor(XnClient* pClient);
	XnStatus HandleSetIntProperty(XnClient* pClient);
	XnStatus HandleSetRealProperty(XnClient* pClient);
	XnStatus HandleSetStringProperty(XnClient* pClient);
	XnStatus HandleSetGeneralProperty(XnClient* pClient);
	XnStatus HandleGetIntProperty(XnClient* pClient);
	XnStatus HandleGetRealProperty(XnClient* pClient);
	XnStatus HandleGetStringProperty(XnClient* pClient);
	XnStatus HandleGetGeneralProperty(XnClient* pClient);
	XnStatus HandleConfigFromINIFile(XnClient* pClient);
	XnStatus HandleBatchConfig(XnClient* pClient);
	XnStatus HandleNewStream(XnClient* pClient);
	XnStatus HandleRemoveStream(XnClient* pClient);
	XnStatus HandleReadStream(XnClient* pClient);
	XnStatus HandleSetStreamState(XnClient* pClient, XnBool bOpen);

	XnStatus AddClient(XN_SOCKET_HANDLE hClientSocket);
	XnStatus RemoveClientStream(XnClient* pClient, const XnChar* strName);
	XnStatus RemoveClient(XnClient* pClient);
	XnStatus ReadStreams();
	XnStatus ReturnToDefaults();

	XnStatus OnStreamAdded(const XnChar* StreamName);
	XnStatus OnStreamRemoved(const XnChar* StreamName);
	XnStatus OnStreamCollectionChanged(const XnChar* StreamName, XnStreamsChangeEventType EventType);
	XnStatus OnPropertyChanged(const XnProperty* pProp);
	XnStatus OnNewServerEvent(const XnUChar* pData, XnUInt32 nDataSize, XnClient* pClient);
	XnStatus OnNewStreamData(const XnChar* StreamName);

	XnStatus RegisterToProps(XnPropertySet* pProps);

	void DumpMessage(const XnChar* strType, XnUInt32 nSize = 0, XnUInt32 nClientID = 0, const XnChar* strComment = "");

	static XN_THREAD_PROC ClientThread(XN_THREAD_PARAM pThreadParam);
	static XN_THREAD_PROC ReaderThread(XN_THREAD_PARAM pThreadParam);

	static void XN_CALLBACK_TYPE StreamCollectionChangedCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnStreamsChangeEventType EventType, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE PropertyChangedCallback(const XnProperty* pProp, void* pCookie);
	static void XN_CALLBACK_TYPE NewServerEventCallback(const XnUChar* pData, XnUInt32 nDataSize, void* pCookie);
	static void XN_CALLBACK_TYPE NewStreamDataCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE StartNewLogCallback(XnIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	
	XN_DECLARE_LIST(XnClient*, XnClientsList);
	XnStatus OnIntPropertyChangedInternally(const XnChar* strPropName, XnUInt64 nValue, XnBool& bPassPropToClients);
	XnStatus OnRealPropertyChangedInternally(const XnChar* strPropName, XnDouble dValue, XnBool& bPassPropToClients);
	XnStatus OnStringPropertyChangedInternally(const XnChar* strPropName, const XnChar* strValue, XnBool &bPassPropToClients);
	XnStatus OnGeneralPropertyChangedInternally(const XnChar* strPropName, const XnGeneralBuffer &gbValue, XnBool& bPassPropToClients);
	XN_SOCKET_HANDLE m_hListenSocket;
	XN_THREAD_HANDLE m_hReaderThread;
	XN_EVENT_HANDLE m_hNewDataEvent;

	XN_EVENT_HANDLE m_hServerRunningEvent; //This event is set as long as the server is running and servicing requests
	XN_MUTEX_HANDLE m_hServerRunningMutex; //This mutex protects m_hServerRunningEvent
	XN_CRITICAL_SECTION_HANDLE m_hSensorLock;
	XN_CRITICAL_SECTION_HANDLE m_hBroadcastingLock;
	XN_CRITICAL_SECTION_HANDLE m_hClientsCriticalSection;

	XnClientsList m_clients;
	XnBool m_bSensorOpen;
	XnSensor m_sensor;
	XnPropertySetData m_allStreamsProps;
	XnServerStreamsHash* m_pServerStreams;
	XnUInt32 m_nLastClientID;
	XnDump m_serverDump;
	XnStatus m_nErrorState;

	XnActualIntProperty m_noClientTimeout;
	XnIntProperty m_startNewLog;
};

#endif //__XN_SENSOR_SERVER_H__
