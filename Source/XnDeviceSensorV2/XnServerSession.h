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
#ifndef __XN_SERVER_SESSION_H__
#define __XN_SERVER_SESSION_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorsManager.h"
#include <XnOS.h>
#include <XnIONetworkStream.h>
#include "XnSensorClientServer.h"
#include "XnServerLogger.h"
#include <XnStringsHash.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XnServerSession
{
public:
	XnServerSession(XnSensorsManager* pSensorsManager, XnUInt32 nID, XN_SOCKET_HANDLE hSocket, XnServerLogger* pLogger);
	~XnServerSession() ;

	XnStatus Init();
	void Free();

	inline XnUInt32 ID() const { return m_nID; }
	inline XnBool HasEnded() const { return m_bHasEnded; }

private:
	// Types
	typedef struct
	{
		XnServerSession* pSession;
		XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
		XnChar strClientStreamName[XN_DEVICE_MAX_STRING_LENGTH];
		XnBool bIsOpen;
		XnCallbackHandle hNewDataCallback;
	} SessionStream;

	XN_DECLARE_STRINGS_HASH(SessionStream, SessionStreamsHash);

	// Functions
	XnStatus SendReply(XnSensorServerCustomMessages Type, XnStatus nRC, XnUInt32 nDataSize = 0, void* pAdditionalData = NULL);
	XnStatus SendInitialState();
	XnStatus FindStreamByServerName(const XnChar* strName, SessionStream** ppStream);

	XnStatus HandleOpenSensor();
	XnStatus OpenSensorImpl(const XnChar* strConnectionString);
	XnStatus CloseSensorImpl();
	XnStatus HandleSetIntProperty();
	XnStatus SetIntPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnUInt64 nValue);
	XnStatus HandleSetRealProperty();
	XnStatus SetRealPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnDouble dValue);
	XnStatus HandleSetStringProperty();
	XnStatus SetStringPropertyImpl(const XnChar* strModule, const XnChar* strProp, const XnChar* strValue);
	XnStatus HandleSetGeneralProperty();
	XnStatus SetGeneralPropertyImpl(const XnChar* strModule, const XnChar* strProp, const XnGeneralBuffer& gbValue);
	XnStatus HandleGetIntProperty();
	XnStatus GetIntPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnUInt64* pnValue);
	XnStatus HandleGetRealProperty();
	XnStatus GetRealPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnDouble* pdValue);
	XnStatus HandleGetStringProperty();
	XnStatus GetStringPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnChar* strValue);
	XnStatus HandleGetGeneralProperty();
	XnStatus GetGeneralPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnGeneralBuffer& gbValue);
	XnStatus HandleConfigFromINIFile();
	XnStatus ConfigFromINIFileImpl(const XnChar* strFileName, const XnChar* strSectionName);
	XnStatus HandleBatchConfig();
	XnStatus BatchConfigImpl(const XnPropertySet* pProps);
	XnStatus HandleNewStream();
	XnStatus NewStreamImpl(const XnChar* strType, const XnChar* strName, const XnPropertySet* pInitialValues);
	XnStatus HandleRemoveStream();
	XnStatus RemoveStreamImpl(const XnChar* strName);
	XnStatus HandleOpenStream();
	XnStatus OpenStreamImpl(const XnChar* strName);
	XnStatus HandleCloseStream();
	XnStatus CloseStreamImpl(const XnChar* strName);
	XnStatus HandleReadStream();
	XnStatus ReadStreamImpl(const XnChar* strName, XnSensorServerReadReply* pReply);
	XnStatus HandleCloseSession();
	XnStatus CloseSessionImpl();

	XnStatus HandleSingleRequest();

	XnStatus AddSessionModule(const XnChar* clientName, const XnChar* serverName);
	XnStatus RemoveSessionModule(const XnChar* clientName);

	XnStatus OnPropertyChanged(const XnProperty* pProp);
	XnStatus OnNewData(SessionStream* pStream, XnUInt64 nTimestamp, XnUInt32 nFrameID);

	XnStatus ServeThread();

	static void XN_CALLBACK_TYPE PropertyChangedCallback(const XnProperty* pProp, void* pCookie);
	static void XN_CALLBACK_TYPE StreamNewDataCallback(const XnChar* strName, XnUInt64 nTimestamp, XnUInt32 nFrameID, void* pCookie);
	static XN_THREAD_PROC ServeThreadCallback(XN_THREAD_PARAM pThreadParam);

	// Members
	XnSensorsManager* m_pSensorsManager;
	XnUInt32 m_nID;
	XN_SOCKET_HANDLE m_hSocket;
	XN_THREAD_HANDLE m_hThread;
	XN_CRITICAL_SECTION_HANDLE m_hCommLock;
	XN_CRITICAL_SECTION_HANDLE m_hStreamsLock;
	XnIONetworkStream m_ioStream;
	XnDataPacker m_privateIncomingPacker;
	XnDataPacker m_privateOutgoingPacker;
	XnStreamDataSet* m_pStreamDataSet;
	XnBool m_bShouldRun;
	XnBool m_bHasEnded;
	XnServerSensorInvoker* m_pSensor;
	SessionStreamsHash m_streamsHash;
	XnServerLogger* m_pLogger;
	XnCallbackHandle m_hProprtyChangeCallback;
};

#endif // __XN_SERVER_SESSION_H__