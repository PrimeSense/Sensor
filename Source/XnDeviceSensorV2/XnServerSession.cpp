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
#include "XnServerSession.h"
#include "XnSensorClientServer.h"
#include <XnDDK/XnStreamDataInternal.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnServerSession::XnServerSession(XnSensorsManager* pSensorsManager, XnUInt32 nID, XN_SOCKET_HANDLE hSocket, XnServerLogger* pLogger) :
	m_pSensorsManager(pSensorsManager),
	m_nID(nID),
	m_hSocket(hSocket),
	m_hThread(NULL),
	m_hCommLock(NULL),
	m_hStreamsLock(NULL),
	m_ioStream(hSocket),
	m_privateIncomingPacker(&m_ioStream, XN_SENSOR_SERVER_CONFIG_PACKER_SIZE),
	m_privateOutgoingPacker(&m_ioStream, XN_SENSOR_SERVER_CONFIG_PACKER_SIZE),
	m_pStreamDataSet(NULL),
	m_bShouldRun(TRUE),
	m_bHasEnded(FALSE),
	m_pSensor(NULL),
	m_pLogger(pLogger),
	m_hProprtyChangeCallback(NULL)
{
	SessionStream stream;
	strcpy(stream.strStreamName, XN_MODULE_NAME_DEVICE);
	strcpy(stream.strClientStreamName, XN_MODULE_NAME_DEVICE);
	stream.bIsOpen = FALSE;
	m_streamsHash.Set(XN_MODULE_NAME_DEVICE, stream);
}

XnServerSession::~XnServerSession()
{
	Free();	
}

XnStatus XnServerSession::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_privateIncomingPacker.Init();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_privateOutgoingPacker.Init();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnStreamDataSetCreate(&m_pStreamDataSet);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hCommLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hStreamsLock);
	XN_IS_STATUS_OK(nRetVal);

	// start thread
	nRetVal = xnOSCreateThread(ServeThreadCallback, this, &m_hThread);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnServerSession::Free()
{
	if (m_hThread != NULL)
	{
		xnOSWaitAndTerminateThread(&m_hThread, 2000);
		m_hThread = NULL;
	}

	if (m_hStreamsLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hStreamsLock);
		m_hStreamsLock = NULL;
	}

	if (m_hCommLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hCommLock);
		m_hCommLock = NULL;
	}

	if (m_pStreamDataSet != NULL)
	{
		XnStreamDataSetDestroy(&m_pStreamDataSet);
		m_pStreamDataSet = NULL;
	}

	if (m_hSocket != NULL)
	{
		xnOSCloseSocket(m_hSocket);
		m_hSocket = NULL;
	}

	m_privateIncomingPacker.Free();
	m_privateOutgoingPacker.Free();
}

XnStatus XnServerSession::SendReply(XnSensorServerCustomMessages Type, XnStatus nRC, XnUInt32 nDataSize /* = 0 */, void* pAdditionalData /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUChar message[XN_SENSOR_SERVER_MAX_REPLY_SIZE];
	XnSensorClientServerReply* pReply = (XnSensorClientServerReply*)message;
	pReply->nRetVal = nRC;
	pReply->Type = Type;
	pReply->nDataSize = nDataSize;
	xnOSMemCopy(pReply->pData, pAdditionalData, nDataSize);
	XnUChar* pEnd = pReply->pData + nDataSize;

	m_pLogger->DumpMessage("Reply", nDataSize, m_nID);

	// lock this so that messages don't mix up
	{
		XnAutoCSLocker lock(m_hCommLock);
		nRetVal = m_privateOutgoingPacker.WriteCustomData(Type, message, pEnd - message);
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::SendInitialState()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_PROPERTY_SET_CREATE_ON_STACK(props);

	// get it
	nRetVal = m_pSensor->GetAllProperties(&props, TRUE);
	XN_IS_STATUS_OK(nRetVal);

	// and send it
	m_pLogger->DumpMessage("InitialState", 0, m_nID);

	{
		XnAutoCSLocker lock(m_hCommLock);
		nRetVal = m_privateOutgoingPacker.WritePropertySet(&props);
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::FindStreamByServerName(const XnChar* strName, SessionStream** ppStream)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	for (SessionStreamsHash::Iterator it = m_streamsHash.begin(); it != m_streamsHash.end(); ++it)
	{
		SessionStream* pStream = &it.Value();
		if (strcmp(pStream->strStreamName, strName) == 0)
		{
			*ppStream = pStream;
			return XN_STATUS_OK;
		}
	}
	
	*ppStream = NULL;
	return (XN_STATUS_NO_MATCH);
}

XnStatus XnServerSession::HandleOpenSensor()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strConnectionString[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nDataSize = XN_DEVICE_MAX_STRING_LENGTH;
	nRetVal = m_privateIncomingPacker.ReadCustomData(XN_SENSOR_SERVER_MESSAGE_OPEN_SENSOR, strConnectionString, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to open sensor %s", m_nID, strConnectionString);

	XnStatus nActionResult = OpenSensorImpl(strConnectionString);

	if (nActionResult == XN_STATUS_OK)
	{
		// sensor is open. send client its initial state
		nActionResult = SendInitialState();
	}

	// if an error occurred, send it to the client
	if (nActionResult != XN_STATUS_OK)
	{
		nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::OpenSensorImpl(const XnChar* strConnectionString)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_pSensorsManager->GetSensor(strConnectionString, &m_pSensor);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->PropChangedEvent().Register(PropertyChangedCallback, this, &m_hProprtyChangeCallback);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::CloseSensorImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_pSensor == NULL)
	{
		return XN_STATUS_OK;
	}

	// unregister from events
	m_pSensor->PropChangedEvent().Unregister(m_hProprtyChangeCallback);
	m_hProprtyChangeCallback = NULL;

	// release all streams
	XnAutoCSLocker locker(m_hStreamsLock);
	SessionStreamsHash::Iterator it = m_streamsHash.begin();
	while (it != m_streamsHash.end())
	{
		SessionStreamsHash::Iterator curr = it;
		++it;
		const XnChar* strName = curr.Key();
		if (strcmp(strName, XN_MODULE_NAME_DEVICE) != 0)
		{
			RemoveStreamImpl(strName);
		}
		else
		{
			// just remove it from the map
			m_streamsHash.Remove(curr);
		}
	}

	// release sensor
	if (m_pSensor != NULL)
	{
		m_pSensorsManager->ReleaseSensor(m_pSensor);
		m_pSensor = NULL;
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleSetIntProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt64 nValue;
	nRetVal = m_privateIncomingPacker.ReadProperty(strModule, strProp, &nValue);
	XN_IS_STATUS_OK(nRetVal);

	XnStatus nActionResult = SetIntPropertyImpl(strModule, strProp, nValue);
	
	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::SetIntPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", m_nID, strModule, strProp);

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->SetIntProperty(pStream->strStreamName, strProp, nValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleSetRealProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnDouble dValue;
	nRetVal = m_privateIncomingPacker.ReadProperty(strModule, strProp, &dValue);
	XN_IS_STATUS_OK(nRetVal);

	XnStatus nActionResult = SetRealPropertyImpl(strModule, strProp, dValue);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::SetRealPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", m_nID, strModule, strProp);

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->SetRealProperty(pStream->strStreamName, strProp, dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleSetStringProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = m_privateIncomingPacker.ReadProperty(strModule, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);

	XnStatus nActionResult = SetStringPropertyImpl(strModule, strProp, strValue);
	
	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::SetStringPropertyImpl(const XnChar* strModule, const XnChar* strProp, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", m_nID, strModule, strProp);

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->SetStringProperty(pStream->strStreamName, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleSetGeneralProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnGeneralBuffer gbValue;
	nRetVal = m_privateIncomingPacker.ReadProperty(strModule, strProp, &gbValue);
	XN_IS_STATUS_OK(nRetVal);

	XnStatus nActionResult = SetGeneralPropertyImpl(strModule, strProp, gbValue);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::SetGeneralPropertyImpl(const XnChar* strModule, const XnChar* strProp, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to set %s.%s", m_nID, strModule, strProp);

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->SetGeneralProperty(pStream->strStreamName, strProp, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleGetIntProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnSensorServerMessageGetPropertyRequest request;
	XnUInt32 nDataSize = sizeof(request);
	nRetVal = m_privateIncomingPacker.ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY, &request, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);
	if (nDataSize != sizeof(request))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	// get
	XnUInt64 nValue;
	XnStatus nActionResult = GetIntPropertyImpl(request.strModuleName, request.strPropertyName, &nValue);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY, nActionResult, sizeof(nValue), &nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::GetIntPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnUInt64* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->GetIntProperty(pStream->strStreamName, strProp, pnValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleGetRealProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnSensorServerMessageGetPropertyRequest request;
	XnUInt32 nDataSize = sizeof(request);
	nRetVal = m_privateIncomingPacker.ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY, &request, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);
	if (nDataSize != sizeof(request))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	// get
	XnDouble dValue;
	XnStatus nActionResult = GetRealPropertyImpl(request.strModuleName, request.strPropertyName, &dValue);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY, nActionResult, sizeof(dValue), &dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::GetRealPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnDouble* pdValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->GetRealProperty(pStream->strStreamName, strProp, pdValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleGetStringProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnSensorServerMessageGetPropertyRequest request;
	XnUInt32 nDataSize = sizeof(request);
	nRetVal = m_privateIncomingPacker.ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY, &request, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);
	if (nDataSize != sizeof(request))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	// get
	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	XnStatus nActionResult = GetStringPropertyImpl(request.strModuleName, request.strPropertyName, strValue);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY, nActionResult, sizeof(strValue), strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::GetStringPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->GetStringProperty(pStream->strStreamName, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleGetGeneralProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnUChar bufValue[XN_SENSOR_SERVER_MAX_REPLY_SIZE];
	XnSensorServerMessageGetPropertyRequest* pRequest = (XnSensorServerMessageGetPropertyRequest*)bufValue;
	XnUChar* pData = bufValue + sizeof(XnSensorServerMessageGetPropertyRequest);
	XnUInt32 nDataSize = XN_SENSOR_SERVER_MAX_REPLY_SIZE;
	nRetVal = m_privateIncomingPacker.ReadCustomData(XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY, bufValue, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	if (nDataSize < sizeof(XnSensorServerMessageGetPropertyRequest))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	// get
	XnGeneralBuffer gbValue = XnGeneralBufferPack(pData, pRequest->nSize);
	XnStatus nActionResult = GetGeneralPropertyImpl(pRequest->strModuleName, pRequest->strPropertyName, gbValue);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY, nActionResult, pRequest->nSize, pData);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::GetGeneralPropertyImpl(const XnChar* strModule, const XnChar* strProp, XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strModule, pStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->GetGeneralProperty(pStream->strStreamName, strProp, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleConfigFromINIFile()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnSensorServerMessageIniFile message;
	XnUInt32 nDataSize = sizeof(message);
	nRetVal = m_privateIncomingPacker.ReadCustomData(XN_SENSOR_SERVER_MESSAGE_INI_FILE, (XnUChar*)&message, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	if (nDataSize != sizeof(message))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid size!");
	}

	// load
	XnStatus nActionResult = ConfigFromINIFileImpl(message.strFileName, message.strSectionName);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::ConfigFromINIFileImpl(const XnChar* strFileName, const XnChar* strSectionName)
{
	return m_pSensor->LoadConfigFromFile(strFileName, strSectionName);
}

XnStatus XnServerSession::HandleBatchConfig()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XN_PROPERTY_SET_CREATE_ON_STACK(props);

	nRetVal = m_privateIncomingPacker.ReadPropertySet(&props);
	XN_IS_STATUS_OK(nRetVal);

	XnStatus nActionResult = BatchConfigImpl(&props);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::BatchConfigImpl(const XnPropertySet* pProps)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested a batch config", m_nID);

	XN_PROPERTY_SET_CREATE_ON_STACK(serverProps);
	for (XnPropertySetData::Iterator it = pProps->pData->begin(); it != pProps->pData->end(); ++it)
	{
		SessionStream* pStream;
		nRetVal = m_streamsHash.Get(it.Key(), pStream);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnPropertySetCloneModule(pProps, &serverProps, it.Key(), pStream->strStreamName);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = m_pSensor->BatchConfig(&serverProps);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleNewStream()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XN_PROPERTY_SET_CREATE_ON_STACK(props);

	XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = m_privateIncomingPacker.ReadNewStream(strType, strName, &props);
	XN_IS_STATUS_OK(nRetVal);

	XnPropertySet* pInitialValues = &props;
	if (props.pData->begin() == props.pData->end())
	{
		pInitialValues = NULL;
	}

	XnStatus nActionResult = NewStreamImpl(strType, strName, pInitialValues);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::NewStreamImpl(const XnChar* strType, const XnChar* strName, const XnPropertySet* pInitialValues)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to create stream '%s' (%s)", m_nID, strName, strType);

	nRetVal = m_pSensor->GetStream(strType, pInitialValues);
	XN_IS_STATUS_OK(nRetVal);

	// send client the new stream data
	XN_PROPERTY_SET_CREATE_ON_STACK(streamProps);
	XN_PROPERTY_SET_CREATE_ON_STACK(clientStreamProps);

	// take properties
	nRetVal = m_pSensor->GetAllProperties(&streamProps, FALSE, strType);
	XN_IS_STATUS_OK(nRetVal);

	// copy relevant ones
	nRetVal = XnPropertySetCloneModule(&streamProps, &clientStreamProps, strType, strName);
	XN_IS_STATUS_OK(nRetVal);

	// now change the state property. It should be OFF, and not the real value (each client has
	// its own stream state).
	nRetVal = XnPropertySetRemoveProperty(&clientStreamProps, strName, XN_STREAM_PROPERTY_STATE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(&clientStreamProps, strName, XN_STREAM_PROPERTY_STATE, FALSE);
	XN_IS_STATUS_OK(nRetVal);

	m_pLogger->DumpMessage("NewStream", 0, m_nID, strName);

	{
		XnAutoCSLocker lock(m_hCommLock);
		nRetVal = m_privateOutgoingPacker.WriteNewStream(strType, strName, &clientStreamProps);
	}
	XN_IS_STATUS_OK(nRetVal);

	// now add it to client data
	nRetVal = AddSessionModule(strName, strType);
	XN_IS_STATUS_OK(nRetVal);

	// create client stream data
	XnStreamData* pStreamData = NULL;
	nRetVal = m_pSensor->CreateStreamData(strType, &pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	// and add it to set
	nRetVal = XnStreamDataSetAdd(m_pStreamDataSet, pStreamData);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleRemoveStream()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = m_privateIncomingPacker.ReadStreamRemoved(strName);
	XN_IS_STATUS_OK(nRetVal);

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to remove stream %s", m_nID, strName);
	XnStatus nActionResult = RemoveStreamImpl(strName);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::RemoveStreamImpl(const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	// first close it (so that ref count will be consistent)
	if (pStream->bIsOpen)
	{
		nRetVal = CloseStreamImpl(strName);
		XN_IS_STATUS_OK(nRetVal);
		pStream->bIsOpen = FALSE;
	}

	// release that stream
	nRetVal = m_pSensor->ReleaseStream(pStream->strStreamName);
	XN_IS_STATUS_OK(nRetVal);

	// if client had a buffer of this stream, release it
	XnStreamData* pStreamData = NULL;
	nRetVal = XnStreamDataSetGet(m_pStreamDataSet, pStream->strStreamName, &pStreamData);
	if (nRetVal == XN_STATUS_OK)
	{
		if (pStreamData->pInternal->pLockedBuffer != NULL)
		{
			m_pSensor->ReleaseFrameBuffer(pStream->strStreamName, pStreamData->pInternal->pLockedBuffer);
		}

		// free data
		XnStreamDataSetRemove(m_pStreamDataSet, pStreamData);
		XnStreamDataDestroy(&pStreamData);
	}

	// now remove it from client
	nRetVal = RemoveSessionModule(strName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleOpenStream()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read it
	XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nDataSize = XN_DEVICE_MAX_STRING_LENGTH;
	XnUInt32 nType = XN_SENSOR_SERVER_MESSAGE_OPEN_STREAM;
	nRetVal = m_privateIncomingPacker.ReadCustomData(nType, strStreamName, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	XnStatus nActionResult = OpenStreamImpl(strStreamName);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::OpenStreamImpl(const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to open stream %s", m_nID, strName);
	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	if (!pStream->bIsOpen)
	{
		nRetVal = m_pSensor->OpenStream(pStream->strStreamName, StreamNewDataCallback, pStream, &pStream->hNewDataCallback);
		XN_IS_STATUS_OK(nRetVal);
		pStream->bIsOpen = TRUE;
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleCloseStream()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nDataSize = XN_DEVICE_MAX_STRING_LENGTH;
	XnUInt32 nType = XN_SENSOR_SERVER_MESSAGE_CLOSE_STREAM;
	nRetVal = m_privateIncomingPacker.ReadCustomData(nType, strStreamName, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	XnStatus nActionResult = CloseStreamImpl(strStreamName);

	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::CloseStreamImpl(const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Client %u requested to close stream %s", m_nID, strName);
	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	if (pStream->bIsOpen)
	{
		nRetVal = m_pSensor->CloseStream(pStream->strStreamName, pStream->hNewDataCallback);
		XN_IS_STATUS_OK(nRetVal);
		pStream->bIsOpen = FALSE;
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleReadStream()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read it
	XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nDataSize = XN_DEVICE_MAX_STRING_LENGTH;
	nRetVal = m_privateIncomingPacker.ReadCustomData(XN_SENSOR_SERVER_MESSAGE_READ_STREAM, strStreamName, &nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorServerReadReply reply;
	XnStatus nActionResult = ReadStreamImpl(strStreamName, &reply);
	if (nActionResult == XN_STATUS_OK)
	{
		m_pLogger->DumpMessage("Data", sizeof(reply), 0, strStreamName);
		nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_READ_STREAM, XN_STATUS_OK, sizeof(reply), &reply);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, nActionResult);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::ReadStreamImpl(const XnChar* strName, XnSensorServerReadReply* pReply)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(strName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	XnStreamData* pStreamData;
	nRetVal = XnStreamDataSetGet(m_pStreamDataSet, pStream->strStreamName, &pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->ReadStream(pStreamData, &pReply->nOffset);
	XN_IS_STATUS_OK(nRetVal);

	pReply->nDataSize = pStreamData->nDataSize;
	pReply->nFrameID = pStreamData->nFrameID;
	pReply->nTimestamp = pStreamData->nTimestamp;

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleCloseSession()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Received BYE from client %u", m_nID);

	XnStatus nActionResult = CloseSessionImpl();

	// client shouldn't care if close succeeded or not. always send OK.
	nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_BYE, XN_STATUS_OK);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to send BYE reply to client %u: %s", m_nID, xnGetStatusString(nRetVal));
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::CloseSessionImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// stop processing messages
	m_bShouldRun = FALSE;

	// close sensor
	nRetVal = CloseSensorImpl();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::HandleSingleRequest()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnPackedDataType nType;
	nRetVal = m_privateIncomingPacker.ReadNextObject(&nType);
	XN_IS_STATUS_OK(nRetVal);

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];

	switch (nType)
	{
	case XN_SENSOR_SERVER_MESSAGE_OPEN_SENSOR:
		{
			nRetVal = HandleOpenSensor();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_INT_PROPERTY:
		{
			nRetVal = HandleSetIntProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_REAL_PROPERTY:
		{
			nRetVal = HandleSetRealProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_STRING_PROPERTY:
		{
			nRetVal = HandleSetStringProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_GENERAL_PROPERTY:
		{
			nRetVal = HandleSetGeneralProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY:
		{
			nRetVal = HandleGetIntProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY:
		{
			nRetVal = HandleGetRealProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY:
		{
			nRetVal = HandleGetStringProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY:
		{
			nRetVal = HandleGetGeneralProperty();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_INI_FILE:
		{
			nRetVal = HandleConfigFromINIFile();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_PROPERTY_SET:
		{
			nRetVal = HandleBatchConfig();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_NEW_STREAM:
		{
			nRetVal = HandleNewStream();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PACKED_STREAM_REMOVED:
		{
			nRetVal = HandleRemoveStream();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_READ_STREAM:
		{
			nRetVal = HandleReadStream();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_OPEN_STREAM:
		{
			nRetVal = HandleOpenStream();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_CLOSE_STREAM:
		{
			nRetVal = HandleCloseStream();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}

	case XN_SENSOR_SERVER_MESSAGE_BYE:
		{
			nRetVal = HandleCloseSession();
			XN_IS_STATUS_OK(nRetVal);
			break;
		}

	default:
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Unknown client request: %d", nType);
		nRetVal = SendReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND, XN_STATUS_ERROR);
		XN_IS_STATUS_OK(nRetVal);

		return XN_STATUS_ERROR;
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::ServeThread()
{
	XnStatus nRetVal = XN_STATUS_OK;

	while (m_bShouldRun)
	{
		if (!m_ioStream.IsConnected())
		{
			xnLogWarning(XN_MASK_SENSOR_SERVER, "Client %u socket was closed. Closing session...", m_nID);
			CloseSessionImpl();
			break;
		}

		nRetVal = HandleSingleRequest();
		if ((nRetVal != XN_STATUS_OK) &&
			(nRetVal != XN_STATUS_OS_NETWORK_CONNECTION_CLOSED) && 
			(nRetVal != XN_STATUS_OS_NETWORK_TIMEOUT))
		{
			xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed processing client request: %s", xnGetStatusString(nRetVal));
		}
	}

	// We cannot RemoveClient from this thread (as part of it is to close the thread). Instead, signal that 
	// client has stopped, and let server main thread wait for exit.
	m_bHasEnded = TRUE;

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::AddSessionModule(const XnChar* clientName, const XnChar* serverName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	SessionStream stream;
	strcpy(stream.strClientStreamName, clientName);
	strcpy(stream.strStreamName, serverName);
	stream.bIsOpen = FALSE;
	stream.pSession = this;

	nRetVal = m_streamsHash.Set(clientName, stream);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::RemoveSessionModule(const XnChar* clientName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	SessionStream* pStream;
	nRetVal = m_streamsHash.Get(clientName, pStream);
	if (nRetVal == XN_STATUS_OK)
	{
		m_streamsHash.Remove(clientName);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSession::OnPropertyChanged(const XnProperty* pProp)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnAutoCSLocker streamLocker(m_hStreamsLock);

	SessionStream* pStream = NULL;
	nRetVal = FindStreamByServerName(pProp->GetModule(), &pStream);
	XN_IS_STATUS_OK(nRetVal);

	// send message
	m_pLogger->DumpMessage("PropChange", 0, m_nID, pProp->GetName());

	XnAutoCSLocker commLocker(m_hCommLock);
	switch (pProp->GetType())
	{
	case XN_PROPERTY_TYPE_INTEGER:
		{
			XnActualIntProperty* pActualProp = (XnActualIntProperty*)pProp;
			nRetVal = m_privateOutgoingPacker.WriteProperty(pStream->strClientStreamName, pProp->GetName(), pActualProp->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PROPERTY_TYPE_REAL:
		{
			XnActualRealProperty* pActualProp = (XnActualRealProperty*)pProp;
			nRetVal = m_privateOutgoingPacker.WriteProperty(pStream->strClientStreamName, pProp->GetName(), pActualProp->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PROPERTY_TYPE_STRING:
		{
			XnActualStringProperty* pActualProp = (XnActualStringProperty*)pProp;
			nRetVal = m_privateOutgoingPacker.WriteProperty(pStream->strClientStreamName, pProp->GetName(), pActualProp->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PROPERTY_TYPE_GENERAL:
		{
			XnActualGeneralProperty* pActualProp = (XnActualGeneralProperty*)pProp;
			nRetVal = m_privateOutgoingPacker.WriteProperty(pStream->strClientStreamName, pProp->GetName(), pActualProp->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Unknown property type: %d", pProp->GetType());
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSession::OnNewData(SessionStream* pStream, XnUInt64 nTimestamp, XnUInt32 nFrameID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// send to client
	XnSensorServerNewStreamData message;
	xnOSMemSet(&message, 0, sizeof(message));
	message.nTimestamp = nTimestamp;
	message.nFrameID = nFrameID;
	strcpy(message.strStreamName, pStream->strClientStreamName);
	m_pLogger->DumpMessage("NewData", sizeof(XnSensorServerNewStreamData), 0, pStream->strClientStreamName);

	XnAutoCSLocker locker(m_hCommLock);
	nRetVal = m_privateOutgoingPacker.WriteCustomData(XN_SENSOR_SERVER_MESSAGE_NEW_STREAM_DATA, &message, sizeof(XnSensorServerNewStreamData));
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed sending new data event to client %d", m_nID);
	}
	
	return (XN_STATUS_OK);
}

void XN_CALLBACK_TYPE XnServerSession::PropertyChangedCallback(const XnProperty* pProp, void* pCookie)
{
	XnServerSession* pThis = (XnServerSession*)pCookie;
	pThis->OnPropertyChanged(pProp);
}

void XN_CALLBACK_TYPE XnServerSession::StreamNewDataCallback(const XnChar* strName, XnUInt64 nTimestamp, XnUInt32 nFrameID, void* pCookie)
{
	SessionStream* pStream = (SessionStream*)pCookie;
	pStream->pSession->OnNewData(pStream, nTimestamp, nFrameID);
}

XN_THREAD_PROC XnServerSession::ServeThreadCallback(XN_THREAD_PARAM pThreadParam)
{
	XnServerSession* pThis = (XnServerSession*)pThreadParam;
	XnStatus nRetVal = pThis->ServeThread();
	XN_THREAD_PROC_RETURN(nRetVal);
}

