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
#include "XnSensorClient.h"
#include "XnSensor.h"
#include "XnSensorClientServer.h"
#include <XnIONetworkStream.h>
#include "XnSensorClientStream.h"
#include <XnDDK/XnStreamReaderStreamHolder.h>
#include <XnOSCpp.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
//#define XN_SENSOR_CLIENT_WAIT_FOR_SERVER			10000
#define XN_SENSOR_CLIENT_WAIT_FOR_SERVER			5000
#define XN_SENSOR_CLIENT_READ_TIMEOUT				4000
#define XN_SENSOR_CLIENT_TERMINATE_THREAD_TIMEOUT	(XN_SENSOR_CLIENT_READ_TIMEOUT * 2)
#define XN_SENSOR_CLIENT_CONNECT_RETRIES			2
#define XN_MASK_SENSOR_CLIENT						"SensorClient"

//---------------------------------------------------------------------------
// XnSensorClient class
//---------------------------------------------------------------------------
XnChar XnSensorClient::ms_strDir[XN_FILE_MAX_PATH];

XnStatus XnSensorClient::TakeServerLocation()
{
	return xnOSGetCurrentDir(ms_strDir, XN_FILE_MAX_PATH);
}

XnSensorClient::XnSensorClient() :
	XnStreamReaderDevice(XN_DEVICE_NAME, XN_SENSOR_SERVER_MAX_MESSAGE_SIZE),
	m_hSocket(NULL),
	m_hReplyEvent(NULL),
	m_hListenThread(NULL),
	m_pOutgoingPacker(NULL),
	m_bShouldRun(TRUE),
	m_bConnected(TRUE),
	m_pThis(this),
	m_InstancePointer(XN_SENSOR_PROPERTY_INSTANCE_POINTER, &m_pThis, sizeof(m_pThis), NULL),
	m_ErrorState(XN_MODULE_PROPERTY_ERROR_STATE, XN_STATUS_OK),
	m_hLock(NULL)
{
	strcpy(m_strConfigDir, ".");
}

XnSensorClient::~XnSensorClient()
{
}

void XnSensorClient::SetConfigDir(const XnChar* strConfigDir)
{
	strcpy(m_strConfigDir, strConfigDir);
}

XnStatus XnSensorClient::GetDefinition(XnDeviceDefinition* pDeviceDefinition)
{
	return XnSensor::GetDefinition(pDeviceDefinition);
}

XnStatus XnSensorClient::Enumerate(XnConnectionString *aConnectionStrings, XnUInt32 *pnCount)
{
	return XnSensor::Enumerate(aConnectionStrings, pnCount);
}

XnStatus XnSensorClient::Init(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnStreamReaderDevice::Init(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	m_pThis = this;
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::InitImpl(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_MUTEX_HANDLE hServerRunningMutex = NULL;
	XnOSEvent serverRunningEvent;

	nRetVal = serverRunningEvent.Open(XN_SENSOR_SERVER_RUNNING_EVENT_NAME);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = serverRunningEvent.Create(XN_SENSOR_SERVER_RUNNING_EVENT_NAME, TRUE);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_CLIENT, "Failed to create server running event: %s", xnGetStatusString(nRetVal));
			return nRetVal;
		}
	}
	
	nRetVal = xnOSCreateNamedMutex(&hServerRunningMutex, XN_SENSOR_SERVER_RUNNING_MUTEX_NAME);
	XN_IS_STATUS_OK(nRetVal);
	
	nRetVal = xnOSLockMutex(hServerRunningMutex, XN_SENSOR_SERVER_RUNNING_MUTEX_TIMEOUT);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSCloseMutex(&hServerRunningMutex);
		return nRetVal;
	}

	XnBool bServerRunning = (serverRunningEvent.Wait(1) == XN_STATUS_OK);
	nRetVal = xnOSUnLockMutex(hServerRunningMutex);
	xnOSCloseMutex(&hServerRunningMutex);
	XN_IS_STATUS_OK(nRetVal);

	if (!bServerRunning)
	{
		nRetVal = StartServerProcess();
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_CLIENT, "Failed to start server process: %s", xnGetStatusString(nRetVal));
			return nRetVal;
		}
	}

	nRetVal = serverRunningEvent.Wait(XN_SENSOR_CLIENT_WAIT_FOR_SERVER);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_SENSOR_CLIENT, "Failed to wait for server to start: %s", xnGetStatusString(nRetVal));
		return nRetVal;
	}

	// init network
	nRetVal = xnOSInitNetwork();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hLock);
	XN_IS_STATUS_OK(nRetVal);

	// now init
	nRetVal = XnStreamReaderDevice::InitImpl(pDeviceConfig);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSCloseCriticalSection(&m_hLock);
		return nRetVal;
	}

	m_bConnected = TRUE;

	nRetVal = xnOSCreateEvent(&m_hReplyEvent, FALSE);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSCloseCriticalSection(&m_hLock);
		return nRetVal;
	}

	nRetVal = xnOSCreateThread(ListenThread, this, &m_hListenThread);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSCloseEvent(&m_hReplyEvent);
		xnOSCloseCriticalSection(&m_hLock);
		return nRetVal;
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::ReadInitialState(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// first message should be either the initial state or an error message (if server failed to open
	// sensor)
	XnPackedDataType nType;
	nRetVal = GetDataPacker()->ReadNextObject(&nType);
	XN_IS_STATUS_OK(nRetVal);

	if (nType == XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND)
	{
		// check the error code
		XnUInt32 nDataSize = sizeof(m_LastReply);
		nRetVal = GetDataPacker()->ReadCustomData(nType, &m_LastReply, &nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		XN_LOG_WARNING_RETURN(m_LastReply.nRetVal, XN_MASK_SENSOR_SERVER, "Server returned an error: %s", xnGetStatusString(m_LastReply.nRetVal));
	}
	else if (nType == XN_PACKED_PROPERTY_SET)
	{
		nRetVal = GetDataPacker()->ReadPropertySet(pSet);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROTOCOL_UNKNOWN_ERROR, XN_MASK_DDK, "Unexpected message: %d (should start with a GENERAL_OP_RESPOND or PROPERTY_SET)", nType);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::Destroy()
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (m_hSocket != NULL)
	{
		nRetVal = SendBye();
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogWarning(XN_MASK_SENSOR_CLIENT, "Failed to send BYE to the server - %s", xnGetStatusString(nRetVal));
			//But we keep going - we must destroy our object.
		}
		
		//Signal to the listener thread that it should stop running
		m_bShouldRun = FALSE;
		m_bConnected = FALSE;
	}

	if (m_hListenThread != NULL)
	{
		xnOSWaitAndTerminateThread(&m_hListenThread, XN_SENSOR_CLIENT_TERMINATE_THREAD_TIMEOUT);
		m_hListenThread = NULL;
	}

	// now destroy it all
	XnStreamReaderDevice::Destroy();

	if (m_hReplyEvent != NULL)
	{
		xnOSCloseEvent(&m_hReplyEvent);
		m_hReplyEvent = NULL;
	}

	XN_DELETE(m_pOutgoingPacker);

	if (m_hLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hLock);
		m_hLock = NULL;
	}

	return XN_STATUS_OK;
}

XnStatus XnSensorClient::CreateStream(const XnChar* StreamType, const XnChar* StreamName /* = NULL */, const XnPropertySet* pInitialValues /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	if (pInitialValues == NULL)
	{
		pInitialValues = &props;
	}
	
	xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Creating stream %s (of type %s)", StreamName, StreamType);

	nRetVal = m_pOutgoingPacker->WriteNewStream(StreamType, StreamName, pInitialValues);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::DestroyStream(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Destroying stream %s", StreamName);

	// this might be called after connection was closed
	if (m_bConnected)
	{
		nRetVal = m_pOutgoingPacker->WriteStreamRemoved(StreamName);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
		XN_IS_STATUS_OK(nRetVal);
	}

	XnStreamReaderDevice::DestroyStream(StreamName);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check if we have a local copy
	nRetVal = XnStreamReaderDevice::GetProperty(ModuleName, PropertyName, pnValue);
	if (nRetVal == XN_STATUS_DEVICE_PROPERTY_DONT_EXIST)
	{
		xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Getting property %s.%s from server...", ModuleName, PropertyName);

		// get from server (virtual property?)
		XnSensorServerMessageGetPropertyRequest request;
		strcpy(request.strModuleName, ModuleName);
		strcpy(request.strPropertyName, PropertyName);
		nRetVal = m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY, &request, sizeof(request));
		XN_IS_STATUS_OK(nRetVal);

		// wait for reply
		nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY);
		XN_IS_STATUS_OK(nRetVal);

		*pnValue = *(XnUInt64*)m_LastReply.pData;
	}
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if we have a local copy
	nRetVal = XnStreamReaderDevice::GetProperty(ModuleName, PropertyName, pdValue);
	if (nRetVal == XN_STATUS_DEVICE_PROPERTY_DONT_EXIST)
	{
		// get from server (virtual property?)
		xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Getting property %s.%s from server...", ModuleName, PropertyName);
		XnSensorServerMessageGetPropertyRequest request;
		strcpy(request.strModuleName, ModuleName);
		strcpy(request.strPropertyName, PropertyName);
		nRetVal = m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY, &request, sizeof(request));
		XN_IS_STATUS_OK(nRetVal);

		// wait for reply
		nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY);
		XN_IS_STATUS_OK(nRetVal);

		*pdValue = *(XnDouble*)m_LastReply.pData;
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if we have a local copy
	nRetVal = XnStreamReaderDevice::GetProperty(ModuleName, PropertyName, strValue);
	if (nRetVal == XN_STATUS_DEVICE_PROPERTY_DONT_EXIST)
	{
		// get from server (virtual property?)
		xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Getting property %s.%s from server...", ModuleName, PropertyName);
		XnSensorServerMessageGetPropertyRequest request;
		strcpy(request.strModuleName, ModuleName);
		strcpy(request.strPropertyName, PropertyName);
		nRetVal = m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY, &request, sizeof(request));
		XN_IS_STATUS_OK(nRetVal);

		// wait for reply
		nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY);
		XN_IS_STATUS_OK(nRetVal);

		strcpy(strValue, (const XnChar*)m_LastReply.pData);
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& Value)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if we have a local copy
	nRetVal = XnStreamReaderDevice::GetProperty(ModuleName, PropertyName, Value);
	if (nRetVal == XN_STATUS_DEVICE_PROPERTY_DONT_EXIST)
	{
		// get from server (virtual property?)
		xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Getting property %s.%s from server...", ModuleName, PropertyName);
		XnUInt32 nBufSize = sizeof(XnSensorServerMessageGetPropertyRequest) + Value.nDataSize;
		XnUChar bufValue[XN_SENSOR_SERVER_MAX_REPLY_SIZE];
		XnUChar* pBuf = bufValue;

		XnSensorServerMessageGetPropertyRequest* pRequest = (XnSensorServerMessageGetPropertyRequest*)pBuf;
		XnUChar* pData = pBuf + sizeof(XnSensorServerMessageGetPropertyRequest);

		strcpy(pRequest->strModuleName, ModuleName);
		strcpy(pRequest->strPropertyName, PropertyName);
		pRequest->nSize = Value.nDataSize;

		// copy data
		xnOSMemCopy(pData, Value.pData, Value.nDataSize);

		nRetVal = m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY, pBuf, nBufSize);
		XN_IS_STATUS_OK(nRetVal);

		// wait for reply
		nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY);
		XN_IS_STATUS_OK(nRetVal);

		xnOSMemCopy(Value.pData, m_LastReply.pData, m_LastReply.nDataSize);
	}
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// there are some properties we don't change on the server (they affect only this client)
	if (strcmp(ModuleName, XN_MODULE_NAME_DEVICE) == 0 && strcmp(PropertyName, XN_MODULE_PROPERTY_PRIMARY_STREAM) == 0)
	{
		nRetVal = XnStreamReaderDevice::SetProperty(ModuleName, PropertyName, nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// set it on the server
		xnLogVerbose(XN_MASK_SENSOR_SERVER, "Setting %s.%s to %llu...", ModuleName, PropertyName, nValue);

		nRetVal = m_pOutgoingPacker->WriteProperty(ModuleName, PropertyName, nValue);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Setting %s.%s to %f...", ModuleName, PropertyName, dValue);

	nRetVal = m_pOutgoingPacker->WriteProperty(ModuleName, PropertyName, dValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Setting %s.%s to %s...", ModuleName, PropertyName, strValue);

	nRetVal = m_pOutgoingPacker->WriteProperty(ModuleName, PropertyName, strValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& Value)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Setting %s.%s...", ModuleName, PropertyName);

	nRetVal = m_pOutgoingPacker->WriteProperty(ModuleName, PropertyName, Value);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::BatchConfig(const XnPropertySet* pChangeSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Batch configuring server...");

	nRetVal = m_pOutgoingPacker->WritePropertySet(pChangeSet);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::ReadStream(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (!m_bConnected)
	{
		return (XN_STATUS_DEVICE_SERVER_DISCONNECTED);
	}

	nRetVal = XnStreamReaderDevice::ReadStream(pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::Read(XnStreamDataSet* pStreamOutputSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_bConnected)
	{
		return (XN_STATUS_DEVICE_SERVER_DISCONNECTED);
	}

	nRetVal = XnStreamReaderDevice::Read(pStreamOutputSet);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::SendBye()
{
	xnLogVerbose(XN_MASK_SENSOR_CLIENT, "Sending Bye");
	XnStatus nRetVal = m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_BYE, NULL, 0);
	XN_IS_STATUS_OK(nRetVal);

	// wait for reply
	nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_BYE);
	XN_IS_STATUS_OK(nRetVal);
	
	return XN_STATUS_OK;
}

XnStatus XnSensorClient::HandlePackedObject(XnPackedDataType nObjectType)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt32 nDataSize = sizeof(m_LastReply);

	switch (nObjectType)
	{
	case XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND:
	case XN_SENSOR_SERVER_MESSAGE_GET_INT_PROPERTY:
	case XN_SENSOR_SERVER_MESSAGE_GET_REAL_PROPERTY:
	case XN_SENSOR_SERVER_MESSAGE_GET_STRING_PROPERTY:
	case XN_SENSOR_SERVER_MESSAGE_GET_GENERAL_PROPERTY:
	case XN_SENSOR_SERVER_MESSAGE_READ_STREAM:
	case XN_SENSOR_SERVER_MESSAGE_BYE:
		{
			nRetVal = GetDataPacker()->ReadCustomData(nObjectType, &m_LastReply, &nDataSize);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = xnOSSetEvent(m_hReplyEvent);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_SENSOR_SERVER_MESSAGE_NEW_STREAM_DATA:
		{
			XnSensorServerNewStreamData message;
			XnUInt32 nDataSize = sizeof(message);
			nRetVal = GetDataPacker()->ReadCustomData(nObjectType, &message, &nDataSize);
			XN_IS_STATUS_OK(nRetVal);

			// find the stream
			XnStreamDeviceStreamHolder* pHolder = NULL;
			nRetVal = FindStream(message.strStreamName, &pHolder);
			XN_IS_STATUS_OK(nRetVal);

			pHolder->GetStream()->NewDataAvailable(message.nTimestamp, message.nFrameID);
			break;
		}
	default:
		{
			nRetVal = XnStreamReaderDevice::HandlePackedObject(nObjectType);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::HandleNewStream(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialValues)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnStreamReaderDevice::HandleNewStream(strType, strName, pInitialValues);
	XN_IS_STATUS_OK(nRetVal);

	// open shared memory
	XnStreamDeviceStreamHolder* pHolder = NULL;
	nRetVal = FindStream(strName, &pHolder);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorClientStream* pStream = (XnSensorClientStream*)pHolder->GetStream();
	nRetVal = pStream->OpenSharedMemory();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::HandleGeneralProperty(const XnChar* strModule, const XnChar* strName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// ignore some properties
	if (strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 && strcmp(strName, XN_SENSOR_PROPERTY_INSTANCE_POINTER) == 0)
	{
		return (XN_STATUS_OK);
	}
	else
	{
		nRetVal = XnStreamReaderDevice::HandleGeneralProperty(strModule, strName, gbValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::WaitForReply(XnSensorServerCustomMessages ExpectedMessage)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// wait for event
	nRetVal = xnOSWaitEvent(m_hReplyEvent, XN_SENSOR_REPLY_TIMEOUT);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_LOG_WARNING_RETURN(nRetVal, XN_MASK_SENSOR_SERVER, "Timeout when waiting for reply from sensor server!");
	}

	// reset it
	nRetVal = xnOSResetEvent(m_hReplyEvent);
	XN_IS_STATUS_OK(nRetVal);

	// check error code
	if (m_LastReply.nRetVal != XN_STATUS_OK)
	{
		XN_LOG_WARNING_RETURN(m_LastReply.nRetVal, XN_MASK_SENSOR_SERVER, "Server returned an error: %s", xnGetStatusString(m_LastReply.nRetVal));
	}

	if (m_LastReply.Type != ExpectedMessage)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "Sensor server protocol error - invalid reply type!");
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnSensorServerMessageIniFile message;
	strncpy(message.strFileName, csINIFilePath, XN_FILE_MAX_PATH);
	strncpy(message.strSectionName, csSectionName, XN_DEVICE_MAX_STRING_LENGTH);

	nRetVal = m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_INI_FILE, &message, sizeof(message));
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::CreateIOStreamImpl(const XnChar *strConnectionString, XnIOStream *&pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnOSCreateSocket(XN_OS_TCP_SOCKET, XN_SENSOR_SERVER_IP_ADDRESS, XN_SENSOR_SERVER_PORT, &m_hSocket);
	XN_IS_STATUS_OK(nRetVal);

	// connect to server
	XnUInt64 nStart;
	xnOSGetTimeStamp(&nStart);

	nRetVal = XN_STATUS_OS_NETWORK_TIMEOUT;
	for (XnUInt32 nRetries = 0; (nRetries < XN_SENSOR_CLIENT_CONNECT_RETRIES) && (nRetVal != XN_STATUS_OK); nRetries++)
	{
		nRetVal = xnOSConnectSocket(m_hSocket, XN_SENSOR_CLIENT_WAIT_FOR_SERVER);
	}

	if (nRetVal == XN_STATUS_OS_NETWORK_TIMEOUT)
	{
		xnLogError(XN_MASK_SENSOR_CLIENT, "Got timeout waiting for server");
		return nRetVal;
	}
	else if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_SENSOR_CLIENT, "Got an error trying to connect to server socket: %s", xnGetStatusString(nRetVal));
		return nRetVal;
	}

	XnIONetworkStream *pNetworkStream = XN_NEW(XnIONetworkStream, m_hSocket);
	if (pNetworkStream == NULL)
	{
		xnOSCloseSocket(m_hSocket);
		return XN_STATUS_ALLOC_FAILED;
	}
	pNetworkStream->SetReadTimeout(XN_SENSOR_CLIENT_READ_TIMEOUT);
	pStream = pNetworkStream;

	// create outgoing data packer (incoming is created by base class)
	m_pOutgoingPacker = XN_NEW(XnDataPacker, pNetworkStream, XN_SENSOR_SERVER_CONFIG_PACKER_SIZE);
	if (m_pOutgoingPacker == NULL)
	{
		XN_DELETE(pNetworkStream);
		xnOSCloseSocket(m_hSocket);
		return XN_STATUS_ALLOC_FAILED;
	}

	nRetVal = m_pOutgoingPacker->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNetworkStream);
		XN_DELETE(m_pOutgoingPacker);
		xnOSCloseSocket(m_hSocket);
		return nRetVal;
	}

	// send server a request to open the sensor
	nRetVal = m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_OPEN_SENSOR, strConnectionString, strlen(strConnectionString) + 1);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNetworkStream);
		XN_DELETE(m_pOutgoingPacker);
		xnOSCloseSocket(m_hSocket);
		return nRetVal;
	}

	return (XN_STATUS_OK);
}

void XnSensorClient::DestroyIOStreamImpl(XnIOStream* pStream)
{
	XN_DELETE(pStream);
	if (m_hSocket != NULL)
	{
		xnOSCloseSocket(m_hSocket);
		m_hSocket = NULL;
	}
}

XnStatus XnSensorClient::CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnDeviceBase::CreateDeviceModule(ppModuleHolder);
	XN_IS_STATUS_OK(nRetVal);

	// add sensor properties
	XnDeviceModule* pModule = (*ppModuleHolder)->GetModule();
	XnProperty* pProps[] = { &m_InstancePointer, &m_ErrorState };

	nRetVal = pModule->AddProperties(pProps, sizeof(pProps)/sizeof(XnProperty*));
	if (nRetVal != XN_STATUS_OK)
	{
		DestroyModule(*ppModuleHolder);
		*ppModuleHolder = NULL;
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorClient::CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnSensorClientStream* pStream;
	if (strcmp(StreamType, XN_STREAM_TYPE_AUDIO) == 0)
	{
		XN_VALIDATE_NEW(pStream, XnSensorClientAudioStream, this, StreamType, StreamName);
	}
	else
	{
		XN_VALIDATE_NEW(pStream, XnSensorClientFrameStream, this, StreamType, StreamName);
	}

	XnStreamReaderStreamHolder* pHolder = XN_NEW(XnStreamReaderStreamHolder, pStream);
	if (pHolder == NULL)
	{
		XN_DELETE(pStream);
		return XN_STATUS_ALLOC_FAILED;
	}

	*ppStreamHolder = pHolder;

	return (XN_STATUS_OK);
}

void XnSensorClient::DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder)
{
	XN_DELETE(pStreamHolder->GetModule());
	XN_DELETE(pStreamHolder);
}

XnStatus XnSensorClient::Listen()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	while (m_bShouldRun)
	{
		nRetVal = ReadNextEventFromStream();
		if (nRetVal == XN_STATUS_OS_NETWORK_TIMEOUT)
		{
			continue;
		}	
		else if ((nRetVal == XN_STATUS_OS_NETWORK_CONNECTION_CLOSED) && !m_bShouldRun)
		{
			xnLogInfo(XN_MASK_SENSOR_CLIENT, "Client connection was closed gracefully");
		}
		else if (nRetVal != XN_STATUS_OK)
		{
			XnIONetworkStream* pStream = (XnIONetworkStream*)GetIOStream();
			if (!pStream->IsConnected())
			{
				m_bConnected = FALSE;
				xnLogError(XN_MASK_SENSOR_CLIENT, "Server has disconnected!");
				break;
			}
			else
			{
				xnLogWarning(XN_MASK_SENSOR_CLIENT, "Sensor client failed to handle event: %s", xnGetStatusString(nRetVal));
			}
		}
	}
	
	return (XN_STATUS_OK);
}

XN_THREAD_PROC XnSensorClient::ListenThread(XN_THREAD_PARAM pThreadParam)
{
	XnSensorClient* pThis = (XnSensorClient*)pThreadParam;
	XnStatus nRetVal = pThis->Listen();
	XN_THREAD_PROC_RETURN(nRetVal);
}

XnStatus XnSensorClient::StartServerProcess()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	const XnChar* strServerDir;
	
#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	strServerDir = ms_strDir;
#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_MACOSX)
	strServerDir = "/usr/bin";
#endif

	// we'll start it in it's directory
	XnChar strCurrDir[XN_FILE_MAX_PATH];
	nRetVal = xnOSGetCurrentDir(strCurrDir, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSSetCurrentDir(strServerDir);
	XN_IS_STATUS_OK(nRetVal);

	XnChar strProcessName[XN_FILE_MAX_PATH];
	sprintf(strProcessName, "%s%sXnSensorServer", strServerDir, XN_FILE_DIR_SEP);

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	XN_VALIDATE_STR_APPEND(strProcessName, ".exe", XN_FILE_MAX_PATH, nRetVal);
#endif

	const XnChar* strArguments[] = { m_strConfigDir };

	XN_PROCESS_ID procID;
	nRetVal = xnOSCreateProcess(strProcessName, 1, strArguments, &procID);

	// in any case, return to working dir
	xnOSSetCurrentDir(strCurrDir);

	// now check for errors
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

