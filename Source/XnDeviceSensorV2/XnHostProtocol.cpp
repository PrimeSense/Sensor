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






#include "XnDeviceSensorProtocol.h"
#include "XnHostProtocol.h"
#include <math.h>
#include <XnLog.h>
#include "XnSensorDepthStream.h"
#include "XnSensor.h"
#include <XnPsVersion.h>

// Control Protocol
#include "XnParams.h"

#define XN_RECEIVE_USB_DATA_TIMEOUT 20000
#define XN_USB_HOST_PROTOCOL_TIMEOUT 5000
#define XN_USB_HOST_PROTOCOL_TIMEOUT_KEEP_ALIVE 5000
#define XN_USB_HOST_PROTOCOL_TIMEOUT_GETVERSION 5000
#define XN_USB_HOST_PROTOCOL_TIMEOUT_SETPARAM 5000


#define XN_USB_HOST_PROTOCOL_SEND_RETRIES	5
#define XN_HOST_PROTOCOL_NOT_READY_RETRIES	3

#define XN_PROTOCOL_MAX_PACKET_SIZE_V5_0	512
#define XN_PROTOCOL_MAX_PACKET_SIZE_V0_17	64

#define MAX_PACKET_SIZE 512

XnStatus XnHostProtocolInitFWParams(XnDevicePrivateData* pDevicePrivateData, XnFWVer nFWVer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nFWVer)
	{
	case XN_SENSOR_FW_VER_5_4:
		nRetVal = XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_3);
		XN_IS_STATUS_OK(nRetVal);

		pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets = OPCODE_GET_CMOS_PRESETS;
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		break;

	case XN_SENSOR_FW_VER_5_3:
		nRetVal = XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_2);
		XN_IS_STATUS_OK(nRetVal);

		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		break;

	case XN_SENSOR_FW_VER_5_2:
		nRetVal = XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_1);
		XN_IS_STATUS_OK(nRetVal);

		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		break;

	case XN_SENSOR_FW_VER_5_1:
		nRetVal = XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_0);
		XN_IS_STATUS_OK(nRetVal);

		pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking = OPCODE_GET_CMOS_BLANKING;
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		break;

	case XN_SENSOR_FW_VER_5_0:
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		pDevicePrivateData->FWInfo.nFWMagic = XN_FW_MAGIC_26;
		pDevicePrivateData->FWInfo.nHostMagic = XN_HOST_MAGIC_26;
		pDevicePrivateData->FWInfo.nProtocolHeaderSize = sizeof(XnHostProtocolHeaderV26);
		pDevicePrivateData->FWInfo.nProtocolMaxPacketSize = XN_PROTOCOL_MAX_PACKET_SIZE_V5_0;
		pDevicePrivateData->FWInfo.bAudioSupported = TRUE;

		pDevicePrivateData->FWInfo.bMirrorSupported = TRUE;

		pDevicePrivateData->FWInfo.nOpcodeGetVersion = OPCODE_GET_VERSION;
		pDevicePrivateData->FWInfo.nOpcodeKeepAlive = OPCODE_KEEP_ALIVE;
		pDevicePrivateData->FWInfo.nOpcodeGetParam = OPCODE_GET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeSetParam = OPCODE_SET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeGetFixedParams = OPCODE_GET_FIXED_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeGetMode = OPCODE_GET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeSetMode = OPCODE_SET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams = OPCODE_ALGORITM_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeReset = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking = OPCODE_SET_CMOS_BLANKING;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets = OPCODE_INVALID;


		break;

	case XN_SENSOR_FW_VER_4_0:
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		pDevicePrivateData->FWInfo.nFWMagic = XN_FW_MAGIC_26;
		pDevicePrivateData->FWInfo.nHostMagic = XN_HOST_MAGIC_26;
		pDevicePrivateData->FWInfo.nProtocolHeaderSize = sizeof(XnHostProtocolHeaderV26);
		pDevicePrivateData->FWInfo.nProtocolMaxPacketSize = XN_PROTOCOL_MAX_PACKET_SIZE_V0_17;
		pDevicePrivateData->FWInfo.bAudioSupported = FALSE;
		pDevicePrivateData->FWInfo.bMirrorSupported = FALSE;

		pDevicePrivateData->FWInfo.nOpcodeGetVersion = OPCODE_V400_GET_VERSION;
		pDevicePrivateData->FWInfo.nOpcodeKeepAlive = OPCODE_V400_KEEP_ALIVE;
		pDevicePrivateData->FWInfo.nOpcodeGetParam = OPCODE_V400_GET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeSetParam = OPCODE_V400_SET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeGetFixedParams = OPCODE_V400_GET_FIXED_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeGetMode = OPCODE_V400_GET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeSetMode = OPCODE_V400_SET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams = OPCODE_V400_ALGORITM_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeReset = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets = OPCODE_INVALID;

		break;
	case XN_SENSOR_FW_VER_3_0:
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		pDevicePrivateData->FWInfo.nFWMagic = XN_FW_MAGIC_26;
		pDevicePrivateData->FWInfo.nHostMagic = XN_HOST_MAGIC_26;
		pDevicePrivateData->FWInfo.nProtocolHeaderSize = sizeof(XnHostProtocolHeaderV26);
		pDevicePrivateData->FWInfo.nProtocolMaxPacketSize = XN_PROTOCOL_MAX_PACKET_SIZE_V0_17;
		pDevicePrivateData->FWInfo.bAudioSupported = TRUE;
		pDevicePrivateData->FWInfo.bMirrorSupported = FALSE;

		pDevicePrivateData->FWInfo.nOpcodeGetVersion = OPCODE_V300_GET_VERSION;
		pDevicePrivateData->FWInfo.nOpcodeKeepAlive = OPCODE_V300_KEEP_ALIVE;
		pDevicePrivateData->FWInfo.nOpcodeGetParam = OPCODE_V300_GET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeSetParam = OPCODE_V300_SET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeGetFixedParams = OPCODE_V300_GET_FIXED_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeGetMode = OPCODE_V300_GET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeSetMode = OPCODE_V300_SET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams = OPCODE_V300_ALGORITM_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeReset = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets = OPCODE_INVALID;

		break;
	case XN_SENSOR_FW_VER_1_2:
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		pDevicePrivateData->FWInfo.nFWMagic = XN_FW_MAGIC_26;
		pDevicePrivateData->FWInfo.nHostMagic = XN_HOST_MAGIC_26;
		pDevicePrivateData->FWInfo.nProtocolHeaderSize = sizeof(XnHostProtocolHeaderV26);
		pDevicePrivateData->FWInfo.nProtocolMaxPacketSize = XN_PROTOCOL_MAX_PACKET_SIZE_V0_17;
		pDevicePrivateData->FWInfo.bAudioSupported = FALSE;
		pDevicePrivateData->FWInfo.bMirrorSupported = FALSE;

		pDevicePrivateData->FWInfo.nOpcodeGetVersion = OPCODE_V110_GET_VERSION;
		pDevicePrivateData->FWInfo.nOpcodeKeepAlive = OPCODE_V110_KEEP_ALIVE;
		pDevicePrivateData->FWInfo.nOpcodeGetParam = OPCODE_V110_GET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeSetParam = OPCODE_V110_SET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeGetFixedParams = OPCODE_V110_GET_FIXED_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeGetMode = OPCODE_V110_GET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeSetMode = OPCODE_V110_SET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams = OPCODE_V110_ALGORITHM_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeReset = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets = OPCODE_INVALID;

		break;
	case XN_SENSOR_FW_VER_1_1:
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		pDevicePrivateData->FWInfo.nFWMagic = XN_FW_MAGIC_25;
		pDevicePrivateData->FWInfo.nHostMagic = XN_HOST_MAGIC_25;
		pDevicePrivateData->FWInfo.nProtocolHeaderSize = sizeof(XnHostProtocolHeaderV25);
		pDevicePrivateData->FWInfo.nProtocolMaxPacketSize = XN_PROTOCOL_MAX_PACKET_SIZE_V0_17;
		pDevicePrivateData->FWInfo.bAudioSupported = FALSE;
		pDevicePrivateData->FWInfo.bMirrorSupported = FALSE;

		pDevicePrivateData->FWInfo.nOpcodeGetVersion = OPCODE_V110_GET_VERSION;
		pDevicePrivateData->FWInfo.nOpcodeKeepAlive = OPCODE_V110_KEEP_ALIVE;
		pDevicePrivateData->FWInfo.nOpcodeGetParam = OPCODE_V110_GET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeSetParam = OPCODE_V110_SET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeGetFixedParams = OPCODE_V110_GET_FIXED_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeGetMode = OPCODE_V110_GET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeSetMode = OPCODE_V110_SET_MODE;
		pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams = OPCODE_V110_ALGORITHM_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeReset = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets = OPCODE_INVALID;

		break;
	case XN_SENSOR_FW_VER_0_17:
		pDevicePrivateData->FWInfo.nFWVer = nFWVer;
		pDevicePrivateData->FWInfo.nFWMagic = XN_FW_MAGIC_25;
		pDevicePrivateData->FWInfo.nHostMagic = XN_HOST_MAGIC_25;
		pDevicePrivateData->FWInfo.nProtocolHeaderSize = sizeof(XnHostProtocolHeaderV25);
		pDevicePrivateData->FWInfo.nProtocolMaxPacketSize = XN_PROTOCOL_MAX_PACKET_SIZE_V0_17;
		pDevicePrivateData->FWInfo.bAudioSupported = FALSE;
		pDevicePrivateData->FWInfo.bMirrorSupported = FALSE;

		pDevicePrivateData->FWInfo.nOpcodeGetVersion = OPCODE_V017_GET_VERSION;
		pDevicePrivateData->FWInfo.nOpcodeKeepAlive = OPCODE_V017_KEEP_ALIVE;
		pDevicePrivateData->FWInfo.nOpcodeGetParam = OPCODE_V017_GET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeSetParam = OPCODE_V017_SET_PARAM;
		pDevicePrivateData->FWInfo.nOpcodeGetFixedParams = OPCODE_V017_GET_FIXED_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeGetMode = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeSetMode = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams = OPCODE_V017_ALGORITM_PARAMS;
		pDevicePrivateData->FWInfo.nOpcodeReset = OPCODE_V017_RESET;
		pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking = OPCODE_INVALID;
		pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets = OPCODE_INVALID;

		break;
	default:
		return (XN_STATUS_IO_DEVICE_WRONG_VERSION);
	}

	pDevicePrivateData->FWInfo.nCurrMode = XN_MODE_PS;

	return (XN_STATUS_OK);
}

XnStatus XnHostProtocolInitHeader(XnDevicePrivateData* pDevicePrivateData, void* pBuffer, void* pDataBuf, XnUInt32 nSize, XnUInt16 nOpcode)
{
	static XnUInt16 nId = 0;

	if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_1_2)
	{
		XnHostProtocolHeaderV26* pHeader = (XnHostProtocolHeaderV26*)pBuffer;
		pHeader->nMagic = XN_PREPARE_VAR16_IN_BUFFER(pDevicePrivateData->FWInfo.nHostMagic);
		pHeader->nSize = XN_PREPARE_VAR16_IN_BUFFER(XnUInt16(nSize/sizeof(XnUInt16)));
		pHeader->nOpcode = XN_PREPARE_VAR16_IN_BUFFER(nOpcode);
		pHeader->nId = XN_PREPARE_VAR16_IN_BUFFER(nId++);
	}
	else
	{
		XnHostProtocolHeaderV25* pHeader = (XnHostProtocolHeaderV25*)pBuffer;
		pHeader->nMagic = XN_PREPARE_VAR16_IN_BUFFER(pDevicePrivateData->FWInfo.nHostMagic);
		pHeader->nSize = XN_PREPARE_VAR16_IN_BUFFER(XnUInt16(nSize/sizeof(XnUInt16)));
		pHeader->nOpcode = XN_PREPARE_VAR16_IN_BUFFER(nOpcode);
		pHeader->nId = XN_PREPARE_VAR16_IN_BUFFER(nId++);
		pHeader->nCRC16 = XN_PREPARE_VAR16_IN_BUFFER(0);
	}

	return (XN_STATUS_OK);
}

XnStatus XnHostProtocolUSBSend(XnDevicePrivateData* pDevicePrivateData,
							   XnUChar* pBuffer, XnUInt16 nSize, XnUInt32& nRead, XnUInt32 nTimeOut, XnBool bForceBulk)
{
	XnStatus nRetVal;

	XnUsbControlConnection* pCtrlConnection = &pDevicePrivateData->SensorHandle.ControlConnection;

	XnUInt32 nCounter = XN_USB_HOST_PROTOCOL_SEND_RETRIES;
	while (nCounter-- != 0)
	{
		if (pCtrlConnection->bIsBulk || bForceBulk)
			nRetVal = xnUSBWriteEndPoint(pCtrlConnection->ControlOutConnectionEp, pBuffer, nSize, nTimeOut);
		else
		{
			nRetVal = xnUSBSendControl(pDevicePrivateData->SensorHandle.USBDevice, XN_USB_CONTROL_TYPE_VENDOR, 0, 0, 0, pBuffer, nSize, nTimeOut);
		}

		if (nRetVal != XN_STATUS_USB_TRANSFER_TIMEOUT && nRetVal != XN_STATUS_USB_TRANSFER_STALL)
			break;

		xnOSSleep(100);
	}

	return nRetVal;
}

XnStatus XnHostProtocolUSBReceive(XnDevicePrivateData* pDevicePrivateData,
								  XnUChar* pBuffer, XnUInt nSize, XnUInt32& nRead, XnUInt32 nTimeOut, XnBool bForceBulk, XnUInt32 nFailTimeout)
{
	XnStatus nRetVal;
	XnUInt64 nMaxTime;
	XnUInt64 nCurrTime;

	XnUsbControlConnection* pCtrlConnection = &pDevicePrivateData->SensorHandle.ControlConnection;

	xnOSGetHighResTimeStamp(&nMaxTime);
	nMaxTime += (nTimeOut * 1000);

	while (TRUE)
	{
		xnOSGetHighResTimeStamp(&nCurrTime);
		if (nCurrTime > nMaxTime)
		{
			return (XN_STATUS_USB_TRANSFER_TIMEOUT);
		}

		if (pCtrlConnection->bIsBulk || bForceBulk)
			nRetVal = xnUSBReadEndPoint(pCtrlConnection->ControlInConnectionEp, pBuffer, nSize, &nRead, nTimeOut);
		else
			nRetVal = xnUSBReceiveControl(pDevicePrivateData->SensorHandle.USBDevice, XN_USB_CONTROL_TYPE_VENDOR, 0, 0, 0, pBuffer, nSize, &nRead, nTimeOut);

		if (nRetVal != XN_STATUS_USB_TRANSFER_TIMEOUT && nRetVal != XN_STATUS_USB_TRANSFER_STALL && nRetVal != XN_STATUS_USB_NOT_ENOUGH_DATA)
		{
			break;
		}

		if (nFailTimeout != 0)
		{
			XnUInt64 nNow;
			XnUInt64 nNow2;
			xnOSGetHighResTimeStamp(&nNow);
			xnOSGetHighResTimeStamp(&nNow2);
			while (nNow2 - nNow < nFailTimeout)
			{
				xnOSGetHighResTimeStamp(&nNow2);
			}
		}
		else
		{
			xnOSSleep(pDevicePrivateData->FWInfo.nUSBDelayReceive);
		}
	}

	return nRetVal;
}

XnStatus ValidateReplyV26(XnDevicePrivateData* pDevicePrivateData, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt16 nExpectedOpcode, XnUInt16 nRequestId, XnUInt16& nDataSize, XnUChar** pDataBuf)
{
	XnUInt16 nHeaderOffset = 0;
	XnHostProtocolHeaderV26* pHeader = (XnHostProtocolHeaderV26*)pBuffer;

	pHeader->nMagic = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nMagic);
	
	while (pHeader->nMagic !=  pDevicePrivateData->FWInfo.nFWMagic && nHeaderOffset < nBufferSize-pDevicePrivateData->FWInfo.nProtocolHeaderSize-sizeof(XnHostProtocolReplyHeader))
	{
		nHeaderOffset++;
		pHeader = (XnHostProtocolHeaderV26*)(pBuffer+nHeaderOffset);
		pHeader->nMagic = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nMagic);
	}

	pHeader->nId = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nId);
	pHeader->nOpcode = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nOpcode);
	pHeader->nSize = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nSize);

	if (pHeader->nMagic != pDevicePrivateData->FWInfo.nFWMagic)
	{
		return XN_STATUS_DEVICE_PROTOCOL_BAD_MAGIC;
	}

	if (pHeader->nId != nRequestId)
	{
//		printf("Response ID (%d) not the same as request id (%d)\n", pHeader->nId, nRequestId);
		return XN_STATUS_DEVICE_PROTOCOL_WRONG_ID;
	}

	if (pHeader->nOpcode != nExpectedOpcode)
	{
//		printf("Unexpected opcode %d (expected: %d)\n", pHeader->nOpcode, nExpectedOpcode);
		return XN_STATUS_DEVICE_PROTOCOL_WRONG_OPCODE;
	}
	// CRC?
	// ...

	XnHostProtocolReplyHeader* pReply = (XnHostProtocolReplyHeader*)(pBuffer+nHeaderOffset+pDevicePrivateData->FWInfo.nProtocolHeaderSize);
	pReply->nErrorCode = XN_PREPARE_VAR16_IN_BUFFER(pReply->nErrorCode);
	
	if (pReply->nErrorCode != ACK)
	{
		xnLogWarning(XN_MASK_SENSOR_PROTOCOL, "Received NACK: %d", pReply->nErrorCode);

		switch (pReply->nErrorCode)
		{
		case NACK_INVALID_COMMAND:
			return XN_STATUS_DEVICE_PROTOCOL_INVALID_COMMAND;
		case NACK_BAD_PACKET_CRC:
			return XN_STATUS_DEVICE_PROTOCOL_BAD_PACKET_CRC;
		case NACK_BAD_PACKET_SIZE:
			return XN_STATUS_DEVICE_PROTOCOL_BAD_PACKET_SIZE;
		case NACK_BAD_PARAMS:
			return XN_STATUS_DEVICE_PROTOCOL_BAD_PARAMS;
		case NACK_BAD_COMMAND_SIZE:
			return XN_STATUS_DEVICE_PROTOCOL_BAD_COMMAND_SIZE;
		case NACK_NOT_READY:
			return XN_STATUS_DEVICE_PROTOCOL_NOT_READY;
		case NACK_UNKNOWN_ERROR:
		default:
			return XN_STATUS_DEVICE_PROTOCOL_UNKNOWN_ERROR;
		}
	}
	// Check reply length is reasonable for opcode

	nDataSize = pHeader->nSize - sizeof(XnHostProtocolReplyHeader)/sizeof(XnUInt16);

	if (pDataBuf)
		*pDataBuf = pBuffer + nHeaderOffset+pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnHostProtocolReplyHeader);
	return XN_STATUS_OK;
}

XnStatus ValidateReplyV25(XnDevicePrivateData* pDevicePrivateData, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt16 nExpectedOpcode, XnUInt16 nRequestId, XnUInt16& nDataSize, XnUChar** pDataBuf)
{
	XnUInt16 nHeaderOffset = 0;
	XnHostProtocolHeaderV25* pHeader = (XnHostProtocolHeaderV25*)pBuffer;

	pHeader->nMagic = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nMagic);

	while (pHeader->nMagic !=  pDevicePrivateData->FWInfo.nFWMagic && nHeaderOffset < nBufferSize-pDevicePrivateData->FWInfo.nProtocolHeaderSize-sizeof(XnHostProtocolReplyHeader))
	{
		nHeaderOffset++;
		pHeader = (XnHostProtocolHeaderV25*)(pBuffer+nHeaderOffset);
		pHeader->nMagic = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nMagic);
	}

	pHeader->nCRC16 = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nCRC16);
	pHeader->nId = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nId);
	pHeader->nOpcode = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nOpcode);
	pHeader->nSize = XN_PREPARE_VAR16_IN_BUFFER(pHeader->nSize);

	if (pHeader->nMagic != pDevicePrivateData->FWInfo.nFWMagic)
	{
		return XN_STATUS_DEVICE_PROTOCOL_BAD_MAGIC;
	}

	if (pHeader->nId != nRequestId)
	{
		//		printf("Response ID (%d) not the same as request id (%d)\n", pHeader->nId, nRequestId);
		return XN_STATUS_DEVICE_PROTOCOL_WRONG_ID;
	}

	if (pHeader->nOpcode != nExpectedOpcode)
	{
		//		printf("Unexpected opcode %d (expected: %d)\n", pHeader->nOpcode, nExpectedOpcode);
		return XN_STATUS_DEVICE_PROTOCOL_WRONG_OPCODE;
	}
	// CRC?
	// ...

	XnHostProtocolReplyHeader* pReply = (XnHostProtocolReplyHeader*)(pBuffer+nHeaderOffset+pDevicePrivateData->FWInfo.nProtocolHeaderSize);

	pReply->nErrorCode = XN_PREPARE_VAR16_IN_BUFFER(pReply->nErrorCode);

	if (pReply->nErrorCode != ACK)
	{
		xnLogWarning(XN_MASK_SENSOR_PROTOCOL, "Received NACK: %d", pReply->nErrorCode);

		switch (pReply->nErrorCode)
		{
		case NACK_INVALID_COMMAND:
			return XN_STATUS_DEVICE_PROTOCOL_INVALID_COMMAND;
		case NACK_BAD_PACKET_CRC:
			return XN_STATUS_DEVICE_PROTOCOL_BAD_PACKET_CRC;
		case NACK_BAD_PACKET_SIZE:
			return XN_STATUS_DEVICE_PROTOCOL_BAD_PACKET_SIZE;
		case NACK_BAD_PARAMS:
			return XN_STATUS_DEVICE_PROTOCOL_BAD_PARAMS;
		case NACK_UNKNOWN_ERROR:
		default:
			return XN_STATUS_DEVICE_PROTOCOL_UNKNOWN_ERROR;
		}
	}
	// Check reply length is reasonable for opcode

	nDataSize = pHeader->nSize - sizeof(XnHostProtocolReplyHeader)/sizeof(XnUInt16);

	if (pDataBuf)
		*pDataBuf = pBuffer + nHeaderOffset+pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnHostProtocolReplyHeader);
	return XN_STATUS_OK;
}

XnUInt32 XnHostProtocolGetTimeOut(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nOpcode)
{
	if  (nOpcode == pDevicePrivateData->FWInfo.nOpcodeKeepAlive)
		return XN_USB_HOST_PROTOCOL_TIMEOUT_KEEP_ALIVE;
	else if  (nOpcode == pDevicePrivateData->FWInfo.nOpcodeGetVersion)
		return XN_USB_HOST_PROTOCOL_TIMEOUT_GETVERSION;
	else if  (nOpcode == pDevicePrivateData->FWInfo.nOpcodeSetParam)
		return XN_USB_HOST_PROTOCOL_TIMEOUT_SETPARAM;
	else
		return XN_USB_HOST_PROTOCOL_TIMEOUT;
}

XnUInt32 XnHostProtocolGetSetParamRecvTimeOut(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nParam)
{
	if (nParam == PARAM_IMAGE_FLICKER_DETECTION)
		return pDevicePrivateData->FWInfo.nUSBDelaySetParamFlicker;
	else if (nParam == PARAM_GENERAL_STREAM0_MODE)
		return pDevicePrivateData->FWInfo.nUSBDelaySetParamStream0Mode;
	else if (nParam == PARAM_GENERAL_STREAM1_MODE)
		return pDevicePrivateData->FWInfo.nUSBDelaySetParamStream1Mode;
	else if (nParam == PARAM_GENERAL_STREAM2_MODE)
		return pDevicePrivateData->FWInfo.nUSBDelaySetParamStream2Mode;
	else
		return 0;
}

XnStatus XnHostProtocolGetRequestID(XnDevicePrivateData* pDevicePrivateData, XnUChar* pBuffer, XnUInt16* pnRequestId)
{
	XnUInt16 nRequestId;

	if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_1_2)
	{
		nRequestId = ((XnHostProtocolHeaderV26*)(pBuffer))->nId;
	}
	else
	{
		nRequestId = ((XnHostProtocolHeaderV25*)(pBuffer))->nId;
	}

	*pnRequestId = XN_PREPARE_VAR16_IN_BUFFER(nRequestId);
	return XN_STATUS_OK;
}

XnStatus XnHostProtocolReceiveReply(XnDevicePrivateData* pDevicePrivateData, XnUChar* pBuffer, XnUInt16 nBufferSize, XnUInt32 nTimeOut, XnUInt16 nOpcode, XnUInt16 nRequestId, XnUInt32* pnReadBytes, XnUInt16* pnDataSize, XnUChar** ppRelevantBuffer, XnBool bForceBulk, XnUInt32 nRecvTimeout, XnUInt32 nFailTimeout)
{
	XnStatus rc = XN_STATUS_OK;

	XnUInt64 nStartWaitingTime;
	xnOSGetTimeStamp(&nStartWaitingTime);

	while (TRUE) // loop until timeout expires
	{
		do // loop until right reply ID is received
		{
			// receive reply
			if (nRecvTimeout != 0)
			{
				xnOSSleep(nRecvTimeout);
			}

			rc = XnHostProtocolUSBReceive(pDevicePrivateData, pBuffer, pDevicePrivateData->FWInfo.nProtocolMaxPacketSize, *pnReadBytes, nTimeOut, bForceBulk, nFailTimeout);
			XN_IS_STATUS_OK(rc);

			// Validate the reply
			if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_1_2)
			{
				rc = ValidateReplyV26(pDevicePrivateData, pBuffer, *pnReadBytes, nOpcode, nRequestId, *pnDataSize, ppRelevantBuffer);
			}
			else
			{
				rc = ValidateReplyV25(pDevicePrivateData, pBuffer, *pnReadBytes, nOpcode, nRequestId, *pnDataSize, ppRelevantBuffer);
			}
		} while (rc == XN_STATUS_DEVICE_PROTOCOL_WRONG_ID);

		XnUInt64 nNow;
		xnOSGetTimeStamp(&nNow);

		if (rc != XN_STATUS_OK && rc != XN_STATUS_DEVICE_PROTOCOL_BAD_MAGIC)
		{
			return rc;
		}
		else if (rc == XN_STATUS_DEVICE_PROTOCOL_BAD_MAGIC && (nNow-nStartWaitingTime)>XN_RECEIVE_USB_DATA_TIMEOUT)
		{
			// Timeout expired
			return XN_STATUS_DEVICE_PROTOCOL_BAD_MAGIC;
		}
		else if (rc == XN_STATUS_DEVICE_PROTOCOL_BAD_MAGIC)
		{
			// Timeout not expired yet
			xnOSSleep(10);
		}
		else 
		{
			// OK
			break;
		}
	}

	return rc;
}

XnStatus XnHostProtocolExecute(XnDevicePrivateData* pDevicePrivateData,
							   XnUChar* pBuffer, XnUInt16 nSize, XnUInt16 nOpcode,
							   XnUChar** ppRelevantBuffer, XnUInt16& nDataSize, XnUInt32 nRecvTimeout = 0)
{
	XnStatus rc;
	XnUInt32 nRead;
	XnUInt32 nFailTimeout = 0;

	XnBool bForceBulk = FALSE;

	if (nOpcode == OPCODE_INVALID)
	{
		return (XN_STATUS_DEVICE_PROTOCOL_UNSUPPORTED_OPCODE);
	}

	XnUInt32 nTimeOut = XnHostProtocolGetTimeOut(pDevicePrivateData, nOpcode);

	// store request (in case we need to retry it)
	XnUChar request[MAX_PACKET_SIZE];
	xnOSMemCopy(request, pBuffer, nSize);

	XnUInt16 nRequestId;
	rc = XnHostProtocolGetRequestID(pDevicePrivateData, pBuffer, &nRequestId);
	XN_IS_STATUS_OK(rc);

	XnUInt16 nRetriesLeft = XN_HOST_PROTOCOL_NOT_READY_RETRIES;
	while (nRetriesLeft-- > 0) // loop until device is ready
	{
		rc = xnOSLockMutex(pDevicePrivateData->hExecuteMutex, XN_WAIT_INFINITE);
		XN_IS_STATUS_OK(rc);

		// Sleep before sending the control
		xnOSSleep(pDevicePrivateData->FWInfo.nUSBDelayExecutePreSend);

		// Send request
		rc = XnHostProtocolUSBSend(pDevicePrivateData, request, nSize, nRead, nTimeOut, bForceBulk);
		if (rc != XN_STATUS_OK)
		{
			xnOSUnLockMutex(pDevicePrivateData->hExecuteMutex);
			return rc;
		}

		// Sleep before trying to read the reply
		xnOSSleep(pDevicePrivateData->FWInfo.nUSBDelayExecutePostSend);

		// receive reply
		rc = XnHostProtocolReceiveReply(pDevicePrivateData, pBuffer, nSize, nTimeOut, nOpcode, nRequestId, &nRead, &nDataSize, ppRelevantBuffer, bForceBulk, nRecvTimeout, nFailTimeout);

		if (rc == XN_STATUS_DEVICE_PROTOCOL_NOT_READY || rc == XN_STATUS_OK)
		{
			XnStatus unlockRC = xnOSUnLockMutex(pDevicePrivateData->hExecuteMutex);
			XN_IS_STATUS_OK(unlockRC);
		}
		else
		{
			xnOSUnLockMutex(pDevicePrivateData->hExecuteMutex);
			return rc;
		}

		if (rc == XN_STATUS_OK)
			break;

		xnOSSleep(1000);
		xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Device not ready. %d more retries...", nRetriesLeft);
	}

	XN_IS_STATUS_OK(rc);

	if (ppRelevantBuffer == NULL)
		return XN_STATUS_OK;

	// Get rest of data
	XnInt32 nDataRead = 0;

	XnInt32 nCur = nRead; // Read so far

	nRead -= (pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnHostProtocolReplyHeader)); // Data read so far

	while (nRead < nDataSize*2U)
	{
		XnUInt32 dummy = 0;

		rc = XnHostProtocolUSBReceive(pDevicePrivateData, pBuffer+nCur, pDevicePrivateData->FWInfo.nProtocolMaxPacketSize, dummy, nTimeOut, bForceBulk, 0);
		if (rc != XN_STATUS_OK)
		{
			return rc;
		}

		nCur += dummy;
		nRead += dummy;
	}

	nDataRead = nRead;

	return XN_STATUS_OK;
}


XnInt32 compareVersion(const XnVersions& version, XnInt32 nMajor, XnInt32 nMinor, XnInt32 nBuild)
{
	XnInt32 nResult = version.nMajor - nMajor;

	if (nResult == 0)
	{
		nResult = version.nMinor - nMinor;
	}

	if (nResult == 0)
	{
		nResult = version.nBuild - nBuild;
	}

	return nResult;
}

XnStatus XnHostProtocolGetVersion(XnDevicePrivateData* pDevicePrivateData, XnVersions& Version)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;
	XnUInt16 nDataSize;
	XnVersions *pVersion = NULL;

	xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Getting hardware versions...");

	XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_1);
	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, 0, pDevicePrivateData->FWInfo.nOpcodeGetVersion);
	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData,
										buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize, pDevicePrivateData->FWInfo.nOpcodeGetVersion,
										(XnUChar**)(&pVersion), nDataSize);
	if (rc != XN_STATUS_OK)
	{
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_1_1);
		XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, 0, pDevicePrivateData->FWInfo.nOpcodeGetVersion);
		rc = XnHostProtocolExecute(pDevicePrivateData, 
			buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize, pDevicePrivateData->FWInfo.nOpcodeGetVersion,
			(XnUChar**)(&pVersion), nDataSize);	

		if (rc != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_PROTOCOL, "Get version failed: %s", xnGetStatusString(rc));

			return rc;
		}
	}

	xnOSMemCopy(&Version, pVersion, sizeof(XnVersions));

	Version.nBuild = XN_PREPARE_VAR16_IN_BUFFER(Version.nBuild);
	Version.nChip = XN_PREPARE_VAR32_IN_BUFFER(Version.nChip);
	Version.nFPGA = XN_PREPARE_VAR16_IN_BUFFER(Version.nFPGA);
	Version.nSystemVersion = XN_PREPARE_VAR16_IN_BUFFER(Version.nSystemVersion);

	*((XnUInt16*)&Version) = xnOSEndianSwapUINT16(*((XnUInt16*)pVersion));

	if (compareVersion(Version, 5, 5, 0) >= 0)
	{
		xnLogWarning(XN_MASK_SENSOR_PROTOCOL, "Sensor version %d.%d is newer than latest known. Trying to use 5.4 protocol...", Version.nMajor, Version.nMinor);
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_4);
	}
	else if (Version.nMajor == 5 && Version.nMinor == 4)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_4);
	else if (Version.nMajor == 5 && Version.nMinor == 3)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_3);
	else if (Version.nMajor == 5 && Version.nMinor == 2)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_2);
	else if (Version.nMajor == 5 && Version.nMinor == 1)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_1);
	else if (Version.nMajor == 5 && Version.nMinor == 0)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_5_0);
	else if (Version.nMajor >= 4)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_4_0);
	else if (Version.nMajor == 3 && Version.nMinor == 0)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_3_0);
	else if (Version.nMajor == 1 && Version.nMinor == 2)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_1_2);
	else if (Version.nMajor == 1 && Version.nMinor == 1)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_1_1);
	else if (Version.nMajor == 0)
		XnHostProtocolInitFWParams(pDevicePrivateData, XN_SENSOR_FW_VER_0_17);

	Version.SDK.nMajor = XN_PS_MAJOR_VERSION;
	Version.SDK.nMinor = XN_PS_MINOR_VERSION;
	Version.SDK.nMaintenance = XN_PS_MAINTENANCE_VERSION;
	Version.SDK.nBuild = XN_PS_BUILD_VERSION;

	if (Version.nFPGA == XN_FPGA_VER_FPDB_26)
	{
		pDevicePrivateData->HWInfo.nHWVer = XN_SENSOR_HW_VER_FPDB_10;
	}
	else if (Version.nFPGA == XN_FPGA_VER_FPDB_25)
	{
		pDevicePrivateData->HWInfo.nHWVer = XN_SENSOR_HW_VER_FPDB_10;
	}
	else if (Version.nFPGA == XN_FPGA_VER_CDB)
	{
		pDevicePrivateData->HWInfo.nHWVer = XN_SENSOR_HW_VER_CDB_10;
	}
	else
	{
		pDevicePrivateData->HWInfo.nHWVer = XN_SENSOR_HW_VER_UNKNOWN;
	}

	if (Version.nChip == XN_CHIP_VER_PS1000)
	{
		pDevicePrivateData->ChipInfo.nChipVer = XN_SENSOR_CHIP_VER_PS1000;
	}
	else if (Version.nChip == XN_CHIP_VER_PS1080)
	{
		pDevicePrivateData->ChipInfo.nChipVer = XN_SENSOR_CHIP_VER_PS1080;
	}
	else
	{
		pDevicePrivateData->ChipInfo.nChipVer = XN_SENSOR_CHIP_VER_UNKNOWN;
	}

	if (pDevicePrivateData->FWInfo.nFWVer == XN_SENSOR_FW_VER_4_0)
	{
		pDevicePrivateData->SensorInfo.nSensorVer = XN_SENSOR_VER_4_0;
	}
	else if (pDevicePrivateData->FWInfo.nFWVer == XN_SENSOR_FW_VER_3_0)
	{
		pDevicePrivateData->SensorInfo.nSensorVer = XN_SENSOR_VER_3_0;
		pDevicePrivateData->HWInfo.nHWVer = XN_SENSOR_HW_VER_RD_3;
	}
	else if (pDevicePrivateData->FWInfo.nFWVer == XN_SENSOR_FW_VER_5_0 || pDevicePrivateData->FWInfo.nFWVer == XN_SENSOR_FW_VER_5_1)
	{
		pDevicePrivateData->SensorInfo.nSensorVer = XN_SENSOR_VER_5_0;
		pDevicePrivateData->HWInfo.nHWVer = XN_SENSOR_HW_VER_RD_5;
	}
	else
	{
		pDevicePrivateData->SensorInfo.nSensorVer = XN_SENSOR_VER_2_0;
	}

	Version.FWVer = pDevicePrivateData->FWInfo.nFWVer;
	Version.HWVer = pDevicePrivateData->HWInfo.nHWVer;
	Version.SensorVer = pDevicePrivateData->SensorInfo.nSensorVer;
	Version.ChipVer = pDevicePrivateData->ChipInfo.nChipVer;

	if (Version.nMajor >= 5)
	{
		XnChar cpBuffer[XN_MAX_OS_NAME_LENGTH];
		sprintf(cpBuffer, "%x", Version.nBuild);
		Version.nBuild = atoi(cpBuffer);
	}

	if (!pDevicePrivateData->pSensor->IsLowBandwidth() &&
		compareVersion(Version, 5, 3, 16) >= 0)
	{
		pDevicePrivateData->FWInfo.nUSBDelayReceive = 1;
		pDevicePrivateData->FWInfo.nUSBDelayExecutePreSend = 0;
		pDevicePrivateData->FWInfo.nUSBDelayExecutePostSend = 0;
		pDevicePrivateData->FWInfo.nUSBDelaySoftReset = 1;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamFlicker = 1;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamStream0Mode = 1;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamStream1Mode = 1;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamStream2Mode = 1;
	}
	else
	{
		pDevicePrivateData->FWInfo.nUSBDelayReceive = 100;
		pDevicePrivateData->FWInfo.nUSBDelayExecutePreSend = 1;
		pDevicePrivateData->FWInfo.nUSBDelayExecutePostSend = 10;
		pDevicePrivateData->FWInfo.nUSBDelaySoftReset = 800;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamFlicker = 3000;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamStream0Mode = 1;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamStream1Mode = 300;
		pDevicePrivateData->FWInfo.nUSBDelaySetParamStream2Mode = 1;

		if (compareVersion(Version, 5, 3, 15) == 0)
		{
			pDevicePrivateData->FWInfo.nUSBDelaySetParamFlicker = 300;
		}	
	}

	xnLogInfo(XN_MASK_SENSOR_PROTOCOL, "Hardware versions: FW=%d.%d.%d (%d) HW=%d Chip=%d Sensor=%d SYS=%d", Version.nMajor, Version.nMinor, Version.nBuild, Version.FWVer, Version.HWVer, Version.ChipVer, Version.SensorVer, Version.nSystemVersion);

	return XN_STATUS_OK;
}

XnStatus XnHostProtocolKeepAlive(XnDevicePrivateData* pDevicePrivateData)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

	xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Requesting KeepAlive...");

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, 0, pDevicePrivateData->FWInfo.nOpcodeKeepAlive);

	XnUInt16 nDataSize;

	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData, 
										buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize, pDevicePrivateData->FWInfo.nOpcodeKeepAlive,
										NULL, nDataSize);

	if (rc == XN_STATUS_OK)
		xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Got KeepAlive Reply.");
	else
		xnLogError(XN_MASK_SENSOR_PROTOCOL, "KeepAlive failed: %s", xnGetStatusString(rc));

	return rc;
}


XnStatus XnHostProtocolGetParam(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nParam, XnUInt16& nValue)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

//	xnLogInfo(XN_MASK_SENSOR_PROTOCOL, "Getting Parameter [%d]...", nParam);

	*(XnUInt16*)pDataBuf = XN_PREPARE_VAR16_IN_BUFFER(nParam);

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeGetParam);

	XnUInt16 nDataSize;
	XnUInt16* pValue = NULL;

	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData, 
										buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeGetParam,
										(XnUChar**)(&pValue), nDataSize);
	if (rc != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_SENSOR_PROTOCOL, "Failed getting [%d]: %s", nParam, xnGetStatusString(rc));
		return rc;
	}

//	xnLogInfo(XN_MASK_SENSOR_PROTOCOL, "Param[%d] = %d", nParam, *pValue);
	nValue = XN_PREPARE_VAR16_IN_BUFFER(*pValue);

	return XN_STATUS_OK;
}

XnStatus XnHostProtocolSetParam(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nParam, XnUInt16 nValue)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

//	xnLogInfo(XN_MASK_SENSOR_PROTOCOL, "Setting Parameter [%d] to %d", nParam, nValue);

	*(XnUInt16*)pDataBuf = XN_PREPARE_VAR16_IN_BUFFER(nParam);
	*(((XnUInt16*)pDataBuf)+1) = XN_PREPARE_VAR16_IN_BUFFER(nValue);

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnUInt16)*2, pDevicePrivateData->FWInfo.nOpcodeSetParam);

	XnUInt16 nDataSize;

	XnInt32 nTimesLeft = 5;
	XnStatus rc = XN_STATUS_ERROR;
	while (rc != XN_STATUS_OK && rc != XN_STATUS_DEVICE_PROTOCOL_BAD_PARAMS && 
		rc != XN_STATUS_DEVICE_PROTOCOL_INVALID_COMMAND && nTimesLeft > 0)
	{
		rc = XnHostProtocolExecute(pDevicePrivateData, 
										buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnUInt16)*2, pDevicePrivateData->FWInfo.nOpcodeSetParam,
										NULL, nDataSize, XnHostProtocolGetSetParamRecvTimeOut(pDevicePrivateData, nParam));
		nTimesLeft--;

		if (rc != XN_STATUS_OK)
		{
			xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Retrying to set the param... rc=%d", rc);
		}
	}

	if (rc != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_SENSOR_PROTOCOL, "Failed setting [%d] to [%d]: %s", nParam, nValue, xnGetStatusString(rc));
	}
	else
	{
//		xnLogInfo(XN_MASK_SENSOR_PROTOCOL, "Done.");
	}

	return rc;
}

void XnHostPrototcolAdjustFixedParamsV26(XnFixedParamsV26* pFixedParamsV26, XnFixedParams* pFixedParams)
{
	// the only difference from V2.6 to V3.0 is the 4 last parameters
	xnOSMemCopy(pFixedParams, pFixedParamsV26, sizeof(XnFixedParamsV26));
	pFixedParams->nUseExtPhy = pFixedParamsV26->nUseExtPhy;
	pFixedParams->bProjectorProtectionEnabled = FALSE;
	pFixedParams->nProjectorDACOutputVoltage = FALSE;
	pFixedParams->nTecEmitterDelay = pFixedParamsV26->nTecEmitterDelay;
}

void XnHostPrototcolAdjustFixedParamsV20(XnFixedParamsV20* pFixedParamsV20, XnFixedParams* pFixedParams)
{
	// the only difference from V2.0 to V2.6 is the addition of nUseExtPhy
	XnFixedParamsV26 fixedParamsV26;
	xnOSMemCopy(&fixedParamsV26, pFixedParamsV20, sizeof(XnFixedParamsV20));

	// now adjust from V2.6 to current
	XnHostPrototcolAdjustFixedParamsV26(&fixedParamsV26, pFixedParams);
}

XnStatus XnHostProtocolGetFixedParams(XnDevicePrivateData* pDevicePrivateData, XnFixedParams& FixedParams)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;
	XnUChar* pRelevantBuffer;
	XnUInt16 nFixedParamSize = 0;

	XnChar FixedParamsBuffer[2048] = {0};
	XnChar* pData = FixedParamsBuffer;

	xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Getting the fixed params...");

	if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_3_0)
	{
		nFixedParamSize = sizeof(XnFixedParams);
	}
	else if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_1_1)
	{
		nFixedParamSize = sizeof(XnFixedParamsV26);
	}
	else // v0.17
	{
		nFixedParamSize = sizeof(XnFixedParamsV20);
	}

	xnOSMemSet(&FixedParams, 0, sizeof(XnFixedParams));

	XnInt16 nDataRead = 0;

	XnStatus rc;
	while (nDataRead < nFixedParamSize)
	{
		*(XnUInt16*)pDataBuf = XN_PREPARE_VAR16_IN_BUFFER(XnUInt16(nDataRead/sizeof(XnUInt32)));
		XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeGetFixedParams);
		
		XnUInt16 nDataSize;

		rc = XnHostProtocolExecute(pDevicePrivateData,
									buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeGetFixedParams,
									&pRelevantBuffer, nDataSize);

		if (rc != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_PROTOCOL, "Get fixed params failed: %s", xnGetStatusString(rc));

			return rc;
		}

		XnUInt32 nReadNow = nDataSize*sizeof(XnUInt16);
		if (nReadNow == 0)
		{
			break;
		}

		xnOSMemCopy(pData + nDataRead, pRelevantBuffer, nReadNow);

		nDataRead += nReadNow;
	}

	for (XnUInt32 i = 0; i < nFixedParamSize/sizeof(XnUInt32); i ++)
	{
		XnUInt32 temp = *((XnUInt32*)(&FixedParams)+i);
		*((XnUInt32*)(&FixedParams)+i) = XN_PREPARE_VAR32_IN_BUFFER(temp);
	}

	if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_3_0)
	{
		xnOSMemCopy(&FixedParams, FixedParamsBuffer, sizeof(XnFixedParams));
	}
	else if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_1_1)
	{
		XnFixedParamsV26 fixedParamsV26;
		xnOSMemCopy(&fixedParamsV26, FixedParamsBuffer, nFixedParamSize);
		XnHostPrototcolAdjustFixedParamsV26(&fixedParamsV26, &FixedParams);
	}
	else if (pDevicePrivateData->FWInfo.nFWVer == XN_SENSOR_FW_VER_0_17)
	{
		XnFixedParamsV20 fixedParamsV20;
		xnOSMemCopy(&fixedParamsV20, FixedParamsBuffer, nFixedParamSize);
		XnHostPrototcolAdjustFixedParamsV20(&fixedParamsV20, &FixedParams);
	}

	return XN_STATUS_OK;
}

XnStatus XnHostProtocolGetMode(XnDevicePrivateData* pDevicePrivateData, XnUInt16& nMode)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, 0, pDevicePrivateData->FWInfo.nOpcodeGetMode);

	XnUInt16 nDataSize;
	XnUInt16* pMode = NULL;

	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData, 
										buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize, pDevicePrivateData->FWInfo.nOpcodeGetMode,
										(XnUChar**)(&pMode), nDataSize);
	if (rc != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_SENSOR_PROTOCOL, "Get mode failed: %s", xnGetStatusString(rc));

		return rc;
	}
	nMode = XN_PREPARE_VAR16_IN_BUFFER(*pMode);

	return XN_STATUS_OK;
}

XnStatus XnHostProtocolReset(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nResetType)
{
	XnStatus rc = XN_STATUS_OK;

	if (pDevicePrivateData->FWInfo.nFWVer == XN_SENSOR_FW_VER_0_17)
	{
		XnUChar buffer[MAX_PACKET_SIZE] = {0};
		XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

		*(XnUInt16*)pDataBuf = XN_PREPARE_VAR16_IN_BUFFER(nResetType);

		XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeReset);

		XnUInt16 nDataSize;

		rc = XnHostProtocolExecute(pDevicePrivateData, 
			buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeReset,
			NULL, nDataSize);

		// Power reset can't fail, and device won't have time to send ACK.
		if (nResetType == XN_RESET_TYPE_POWER)
			rc = XN_STATUS_OK;

		return rc;
	}
	else
	{
		XnUInt16 nActualValue;
		switch (nResetType)
		{
		case XN_RESET_TYPE_POWER:
			nActualValue = XN_HOST_PROTOCOL_MODE_REBOOT;
			break;
		case XN_RESET_TYPE_SOFT:
			{
				// also kill streams before (in FW < 5.2)
				if (pDevicePrivateData->FWInfo.nFWVer < XN_SENSOR_FW_VER_5_2)
				{
					XnSensorFirmwareParams* pParams = pDevicePrivateData->pSensor->GetFirmware()->GetParams();
					rc = pParams->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_OFF);
					XN_IS_STATUS_OK(rc);
					rc = pParams->m_Stream1Mode.SetValue(XN_VIDEO_STREAM_OFF);
					XN_IS_STATUS_OK(rc);
					rc = pParams->m_Stream2Mode.SetValue(XN_AUDIO_STREAM_OFF);
					XN_IS_STATUS_OK(rc);
				}
			}

			nActualValue = XN_HOST_PROTOCOL_MODE_SOFT_RESET;
			break;
		case XN_RESET_TYPE_SOFT_FIRST:
			nActualValue = XN_HOST_PROTOCOL_MODE_SOFT_RESET;
			break;
		default:
			return XN_STATUS_DEVICE_UNSUPPORTED_MODE;
		}

		return XnHostProtocolSetMode(pDevicePrivateData, nActualValue);
	}
}

XnStatus XnHostProtocolSetMode(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nMode)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

	*(XnUInt16*)pDataBuf = XN_PREPARE_VAR16_IN_BUFFER(nMode);

	xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Setting mode to %d...", nMode);

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeSetMode);

	XnUInt16 nDataSize;

	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData, 
										buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeSetMode,
										NULL, nDataSize);

	// Patch: always return OK when switching modes (since the firmware is changing there is nobody to ACK the request...)
	return XN_STATUS_OK;
}


#pragma pack (push, 1)
typedef struct XnAlgorithmParamRequest
{
	XnUInt16 nParamID;
	XnUInt16 nFormat;
	XnUInt16 nResolution;
	XnUInt16 nFPS;
	XnUInt16 nOffset;
} XnAlgorithmParamRequest;

typedef struct XnAlgorithmParamRequestV4
{
	XnUInt8 nResolution;
	XnUInt8 nFPS;
	XnUInt8 nFormat;
	XnUInt8 nParamID;
	XnUInt16 nOffset;
} XnAlgorithmParamRequestV4;
#pragma pack (pop)

XnStatus XnHostProtocolAlgorithmParams(XnDevicePrivateData* pDevicePrivateData,
									   XnHostProtocolAlgorithmType eAlgorithmType,
									   void* pAlgorithmInformation, XnUInt16 nAlgInfoSize, XnResolutions nResolution, XnUInt16 nFPS)
{
	XnChar* pData = (XnChar*)pAlgorithmInformation;
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;
	XnUChar* pRelevantBuffer;

	XnInt16 nDataRead = 0;
	XnUInt16 nRequestSize = 0;

	xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Getting algorithm params 0x%x for resolution %d and fps %d....", eAlgorithmType, nResolution, nFPS);

	XnStatus rc;
	while (nDataRead < nAlgInfoSize)
	{
		if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_5_1)
		{
			XnAlgorithmParamRequest* pRequest = (XnAlgorithmParamRequest*)pDataBuf;
			pRequest->nParamID = XN_PREPARE_VAR16_IN_BUFFER(eAlgorithmType);
			pRequest->nFormat = 0;
			pRequest->nResolution = XN_PREPARE_VAR16_IN_BUFFER(nResolution);
			pRequest->nFPS = XN_PREPARE_VAR16_IN_BUFFER(nFPS);
			pRequest->nOffset = XN_PREPARE_VAR16_IN_BUFFER(nDataRead / sizeof(XnUInt16));
			nRequestSize = sizeof(XnAlgorithmParamRequest);
		}
		else
		{
			XnAlgorithmParamRequestV4* pRequest = (XnAlgorithmParamRequestV4*)pDataBuf;
			pRequest->nParamID = eAlgorithmType;
			pRequest->nFormat = 0;
			pRequest->nResolution = nResolution;
			pRequest->nFPS = 0;
			pRequest->nOffset = XN_PREPARE_VAR16_IN_BUFFER(nDataRead / sizeof(XnUInt16));
			nRequestSize = sizeof(XnAlgorithmParamRequestV4);
		}

		XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, nRequestSize, pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams);
		
		XnUInt16 nDataSize;
		rc = XnHostProtocolExecute(pDevicePrivateData, 
									buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+nRequestSize, pDevicePrivateData->FWInfo.nOpcodeAlgorithmParams,
									&pRelevantBuffer, nDataSize);


		if (rc != XN_STATUS_OK)
			return rc;

		XnUInt32 nReadNow = nDataSize*sizeof(XnUInt16);
		if (nReadNow == 0)
		{
			break;
		}

		xnOSMemCopy(pData + nDataRead, pRelevantBuffer, nReadNow);

		nDataRead += nReadNow;
	}

	if (nDataRead != nAlgInfoSize)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_DEVICE_INVALID_RESPONSE_SIZE, XN_MASK_SENSOR_PROTOCOL, "Failed getting algorithm params: expected %u bytes, but got only %u", nAlgInfoSize, nDataRead);
	}

	return XN_STATUS_OK;
}


XnStatus XnHostProtocolSetAudioSampleRate(XnDevicePrivateData* pDevicePrivateData, XnSampleRate nSampleRate)
{
	EA2d_SampleRate nSample;

	switch (nSampleRate)
	{
	case XN_SAMPLE_RATE_8K:
		nSample = A2D_SAMPLE_RATE_8KHZ;
		break;
	case XN_SAMPLE_RATE_11K:
		nSample = A2D_SAMPLE_RATE_11KHZ;
		break;
	case XN_SAMPLE_RATE_12K:
		nSample = A2D_SAMPLE_RATE_12KHZ;
		break;
	case XN_SAMPLE_RATE_16K:
		nSample = A2D_SAMPLE_RATE_16KHZ;
		break;
	case XN_SAMPLE_RATE_22K:
		nSample = A2D_SAMPLE_RATE_22KHZ;
		break;
	case XN_SAMPLE_RATE_24K:
		nSample = A2D_SAMPLE_RATE_24KHZ;
		break;
	case XN_SAMPLE_RATE_32K:
		nSample = A2D_SAMPLE_RATE_32KHZ;
		break;
	case XN_SAMPLE_RATE_44K:
		nSample = A2D_SAMPLE_RATE_44KHZ;
		break;
	case XN_SAMPLE_RATE_48K:
		nSample = A2D_SAMPLE_RATE_48KHZ;
		break;
	default:
		return XN_STATUS_DEVICE_UNSUPPORTED_MODE;
	}

	return XnHostProtocolSetParam(pDevicePrivateData, PARAM_AUDIO_SAMPLE_RATE, nSample);
}

XnStatus XnHostProtocolGetAudioSampleRate(XnDevicePrivateData* pDevicePrivateData, XnSampleRate* pSampleRate)
{
	XnUInt16 nValue;
	XnHostProtocolGetParam(pDevicePrivateData, PARAM_AUDIO_SAMPLE_RATE, nValue);
	XnSampleRate nSample;

	switch (nValue)
	{
	case A2D_SAMPLE_RATE_8KHZ:
		nSample = XN_SAMPLE_RATE_8K;
		break;
	case A2D_SAMPLE_RATE_11KHZ:
		nSample = XN_SAMPLE_RATE_11K;
		break;
	case A2D_SAMPLE_RATE_12KHZ:
		nSample = XN_SAMPLE_RATE_12K;
		break;
	case A2D_SAMPLE_RATE_16KHZ:
		nSample = XN_SAMPLE_RATE_16K;
		break;
	case A2D_SAMPLE_RATE_22KHZ:
		nSample = XN_SAMPLE_RATE_22K;
		break;
	case A2D_SAMPLE_RATE_24KHZ:
		nSample = XN_SAMPLE_RATE_24K;
		break;
	case A2D_SAMPLE_RATE_32KHZ:
		nSample = XN_SAMPLE_RATE_32K;
		break;
	case A2D_SAMPLE_RATE_44KHZ:
		nSample = XN_SAMPLE_RATE_44K;
		break;
	case A2D_SAMPLE_RATE_48KHZ:
		nSample = XN_SAMPLE_RATE_48K;
		break;
	default:
		return XN_STATUS_DEVICE_UNSUPPORTED_MODE;
	}

	*pSampleRate = nSample;

	return (XN_STATUS_OK);
}


XnStatus XnHostProtocolSetMultipleParams(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nNumOfParams, XnInnerParamData* anParams)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

	XnUInt16* pCurData = (XnUInt16*)pDataBuf;
	for (XnUInt16 nIndex = 0; nIndex < nNumOfParams; ++nIndex)
	{
		*pCurData++ = XN_PREPARE_VAR16_IN_BUFFER(anParams[nIndex].nParam);
		*pCurData++ = XN_PREPARE_VAR16_IN_BUFFER(anParams[nIndex].nValue);
	}

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnUInt16)*nNumOfParams*2, pDevicePrivateData->FWInfo.nOpcodeSetParam);

	XnUInt16 nDataSize;

	XnInt32 nTimesLeft = 5;
	XnStatus rc = XN_STATUS_ERROR;
	while (rc != XN_STATUS_OK && rc != XN_STATUS_DEVICE_PROTOCOL_BAD_PARAMS && 
		rc != XN_STATUS_DEVICE_PROTOCOL_INVALID_COMMAND && nTimesLeft > 0)
	{
		rc = XnHostProtocolExecute(pDevicePrivateData, 
			buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnUInt16)*nNumOfParams*2, pDevicePrivateData->FWInfo.nOpcodeSetParam,
			NULL, nDataSize);
		nTimesLeft--;
	}

	if (rc != XN_STATUS_OK)
		xnLogError(XN_MASK_SENSOR_PROTOCOL, "Failed: %s", xnGetStatusString(rc));

	return rc;
}


XnStatus XnHostProtocolSetIRCropping(XnDevicePrivateData* pDevicePrivateData, XnCropping* pCropping)
{
	XnStatus rc = XN_STATUS_OK;

	if (pCropping->bEnabled)
	{
		XnInnerParamData anParams[4];
		XnUInt16 nIndex = 0;

		anParams[nIndex].nParam = PARAM_IR_CROP_SIZE_X;
		anParams[nIndex].nValue = pCropping->nXSize;
		nIndex++;
		anParams[nIndex].nParam = PARAM_IR_CROP_SIZE_Y;
		anParams[nIndex].nValue = pCropping->nYSize;
		nIndex++;
		anParams[nIndex].nParam = PARAM_IR_CROP_OFFSET_X;
		anParams[nIndex].nValue = pCropping->nXOffset;
		nIndex++;
		anParams[nIndex].nParam = PARAM_IR_CROP_OFFSET_Y;
		anParams[nIndex].nValue = pCropping->nYOffset;

		rc = XnHostProtocolSetMultipleParams(pDevicePrivateData, 4, anParams);
		if (rc != XN_STATUS_OK)
			return rc;
	}

	// commit
	rc = XnHostProtocolSetParam(pDevicePrivateData, PARAM_IR_CROP_ENABLE, pCropping->bEnabled);

	return rc;
}


XnStatus XnDeviceSensorGetDepthAGCParams(XnUInt16 nBin, XnUInt16* pnMinParam, XnUInt16* pnMaxParam)
{
	switch (nBin)
	{
	case 0:
		*pnMinParam = PARAM_DEPTH_AGC_BIN0_LOW;
		*pnMaxParam = PARAM_DEPTH_AGC_BIN0_HIGH;
		break;
	case 1:
		*pnMinParam = PARAM_DEPTH_AGC_BIN1_LOW;
		*pnMaxParam = PARAM_DEPTH_AGC_BIN1_HIGH;
		break;
	case 2:
		*pnMinParam = PARAM_DEPTH_AGC_BIN2_LOW;
		*pnMaxParam = PARAM_DEPTH_AGC_BIN2_HIGH;
		break;
	case 3:
		*pnMinParam = PARAM_DEPTH_AGC_BIN3_LOW;
		*pnMaxParam = PARAM_DEPTH_AGC_BIN3_HIGH;
		break;
	default:
		return XN_STATUS_DEVICE_BAD_PARAM;
	}

	return XN_STATUS_OK;
}

XnStatus XnHostProtocolSetDepthAGCBin(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nBin, XnUInt16 nMinShift, XnUInt16 nMaxShift)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt16 nMinParam;
	XnUInt16 nMaxParam;

	nRetVal = XnDeviceSensorGetDepthAGCParams(nBin, &nMinParam, &nMaxParam);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnHostProtocolSetParam(pDevicePrivateData, nMinParam, nMinShift);
	XN_IS_STATUS_OK(nRetVal);
	
	nRetVal = XnHostProtocolSetParam(pDevicePrivateData, nMaxParam, nMaxShift);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnHostProtocolGetDepthAGCBin(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nBin, XnUInt16* pnMinShift, XnUInt16* pnMaxShift)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt16 nMinParam;
	XnUInt16 nMaxParam;

	nRetVal = XnDeviceSensorGetDepthAGCParams(nBin, &nMinParam, &nMaxParam);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnHostProtocolGetParam(pDevicePrivateData, nMinParam, *pnMinShift);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnHostProtocolGetParam(pDevicePrivateData, nMaxParam, *pnMaxShift);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

#pragma pack (push, 1)

typedef struct XnVSyncRequest
{
	XnUInt16 nUnits;
	XnUInt16 nCmosID;
	XnUInt16 nNumberOfFrames;
} XnVSyncRequest;

#pragma pack (pop)

XnStatus XnHostProtocolSetCmosBlanking(XnDevicePrivateData* pDevicePrivateData, XnUInt16 nUnits, XnCMOSType nCMOSID, XnUInt16 nNumberOfFrames)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;
	XnUInt32 nRequestSize;

	if (pDevicePrivateData->FWInfo.nFWVer >= XN_SENSOR_FW_VER_5_1)
	{
		XnVSyncRequest* pRequest = (XnVSyncRequest*)pDataBuf;
		pRequest->nUnits = XN_PREPARE_VAR16_IN_BUFFER(nUnits);
		pRequest->nCmosID = XN_PREPARE_VAR16_IN_BUFFER(nCMOSID);
		pRequest->nNumberOfFrames = XN_PREPARE_VAR16_IN_BUFFER(nNumberOfFrames);
		nRequestSize = sizeof(XnVSyncRequest);
	}
	else
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED, XN_MASK_SENSOR_PROTOCOL, "Set Blanking is not supported by this firmware!");
	}

	xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Chaning CMOS %d Blanking to %hd (NumberOfFrames=%hu)...", nCMOSID, nUnits, nNumberOfFrames);

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, nRequestSize, pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking);

	XnUInt16 nDataSize;
	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData,
		buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize + nRequestSize, pDevicePrivateData->FWInfo.nOpcodeSetCmosBlanking,
		NULL, nDataSize);

	if (rc != XN_STATUS_OK)
	{
		XN_LOG_WARNING_RETURN(rc, XN_MASK_SENSOR_PROTOCOL, "Failed changing CMOS %d Blanking to %hd (NumberOfFrames=%hu): %s", nCMOSID, nUnits, nNumberOfFrames, xnGetStatusString(rc));
	}

	return (XN_STATUS_OK);
}

#pragma pack (push, 1)

typedef struct XnGetCmosBlankingRequest
{
	XnUInt16 nCmosID;
} XnGetCmosBlankingRequest;

typedef struct XnGetCmosBlankingReply
{
	XnUInt32 nUnits;
} XnGetCmosBlankingReply;

#pragma pack (pop)

XnStatus XnHostProtocolGetCmosBlanking(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOSID, XnUInt16* pnLines)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

	XnGetCmosBlankingRequest* pRequest = (XnGetCmosBlankingRequest*)pDataBuf;
	pRequest->nCmosID = nCMOSID;

	xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "Getting Cmos %d VBlanking...", nCMOSID);

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnGetCmosBlankingRequest), pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking);

	XnGetCmosBlankingReply* pReply;
	XnUInt16 nDataSize;
	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData,
		buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize + sizeof(XnGetCmosBlankingRequest), pDevicePrivateData->FWInfo.nOpcodeGetCmosBlanking,
		(XnUChar**)&pReply, nDataSize);

	if (rc != XN_STATUS_OK)
	{
		XN_LOG_WARNING_RETURN(rc, XN_MASK_SENSOR_PROTOCOL, "Failed getting Cmos %d Blanking: %s", nCMOSID, xnGetStatusString(rc));
	}

	xnLogInfo(XN_MASK_SENSOR_PROTOCOL, "Cmos %d VBlanking: %u", nCMOSID, pReply->nUnits);

	*pnLines = pReply->nUnits;

	return (XN_STATUS_OK);
}

XnStatus XnHostProtocolGetCmosPresets(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOSID, XnCmosPreset* aPresets, XnUInt32& nCount)
{
	XnUChar buffer[MAX_PACKET_SIZE] = {0};
	XnUChar* pDataBuf = buffer + pDevicePrivateData->FWInfo.nProtocolHeaderSize;

	xnLogInfo(XN_MASK_SENSOR_PROTOCOL, "Reading CMOS %d supported presets...", nCMOSID);

	*(XnUInt16*)pDataBuf = XN_PREPARE_VAR16_IN_BUFFER(nCMOSID);

	XnHostProtocolInitHeader(pDevicePrivateData, buffer, pDataBuf, sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets);

	XnUInt16 nDataSize;
	XnCmosPreset* pValue = NULL;

	XnStatus rc = XnHostProtocolExecute(pDevicePrivateData, 
		buffer, pDevicePrivateData->FWInfo.nProtocolHeaderSize+sizeof(XnUInt16), pDevicePrivateData->FWInfo.nOpcodeGetCmosPresets,
		(XnUChar**)(&pValue), nDataSize);
	if (rc != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_SENSOR_PROTOCOL, "Failed getting CMOS %d presets: %s", nCMOSID, xnGetStatusString(rc));
		return rc;
	}

	XnUInt32 nReturnedCount = nDataSize * 2 / sizeof(XnCmosPreset);
	if (nReturnedCount > nCount)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	nCount = nReturnedCount;

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		aPresets[i].nFormat = XN_PREPARE_VAR16_IN_BUFFER(pValue[i].nFormat);
		aPresets[i].nResolution = XN_PREPARE_VAR16_IN_BUFFER(pValue[i].nResolution);
		aPresets[i].nFPS = XN_PREPARE_VAR16_IN_BUFFER(pValue[i].nFPS);
	}

	return XN_STATUS_OK;
}
