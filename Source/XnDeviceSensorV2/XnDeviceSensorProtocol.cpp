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
#include "XnDeviceSensorProtocol.h"
#include "XnDeviceSensorIO.h"
#include "Uncomp.h"
#include "XnHostProtocol.h"
#include <XnLog.h>
#include <XnProfiling.h>
#include "XnStreamProcessor.h"
#include "XnSensor.h"
#include <XnOS.h>

FILE* g_fUSBDump;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnDeviceSensorProtocolGetSensorInfo(XnDevicePrivateData* pDevicePrivateData, XnChar* csSensorID)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	XnFixedParams FixedParams;
	nRetVal = XnHostProtocolGetFixedParams(pDevicePrivateData, FixedParams);
	XN_IS_STATUS_OK(nRetVal);

	sprintf(csSensorID, "%x", FixedParams.nSerialNumber);

	// All is good...
	return (XN_STATUS_OK);
}

XnBool XN_CALLBACK_TYPE XnDeviceSensorProtocolUsbEpCb(XnUChar* pBuffer, XnUInt32 nBufferSize, void* pCallbackData)
{
	XN_PROFILING_START_MT_SECTION("XnDeviceSensorProtocolUsbEpCb");

	XnUInt32 nReadBytes;
	XnUInt16 nMagic;

	XnSpecificUsbDevice* pDevice = (XnSpecificUsbDevice*)pCallbackData;
	XnDevicePrivateData* pDevicePrivateData = pDevice->pDevicePrivateData;
	XnUChar* pBufEnd = pBuffer + nBufferSize;

	XnSpecificUsbDeviceState* pCurrState = &pDevice->CurrState;

	while (pBuffer < pBufEnd)
	{
		switch (pCurrState->State)
		{
		case XN_WAITING_FOR_CONFIGURATION:
			if (pDevicePrivateData->bIgnoreDataPackets)
			{
				// ignore this packet
				xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "ignoring %d bytes - device requested to ignore!", nBufferSize);
				pBuffer = pBufEnd;
			}
			else
			{
				pCurrState->State = XN_IGNORING_GARBAGE;
				pCurrState->nMissingBytesInState = pDevice->nIgnoreBytes;
			}
			break;

		case XN_IGNORING_GARBAGE:
			// ignore first bytes on this endpoint. NOTE: due to a bug in the firmware, the first data received
			// on each endpoint is corrupt, causing wrong timestamp calculation, causing future (true) timestamps
			// to be calculated wrongly. By ignoring the first data received on each endpoint we hope to get
			// only valid data.
			nReadBytes = XN_MIN((XnUInt32)(pBufEnd - pBuffer), pCurrState->nMissingBytesInState);

			if (nReadBytes > 0)
			{
				xnLogVerbose(XN_MASK_SENSOR_PROTOCOL, "ignoring %d bytes - ignore garbage phase!", nReadBytes);
				pCurrState->nMissingBytesInState -= nReadBytes;
				pBuffer += nReadBytes;
			}

			if (pCurrState->nMissingBytesInState == 0)
			{
				pCurrState->State = XN_LOOKING_FOR_MAGIC;
				pCurrState->nMissingBytesInState = sizeof(XnUInt16);
			}
			break;

		case XN_LOOKING_FOR_MAGIC:
			nMagic = XN_PREPARE_VAR16_IN_BUFFER(pDevicePrivateData->FWInfo.nFWMagic);

			if (pCurrState->nMissingBytesInState == sizeof(XnUInt8) && // first byte already found
				pBuffer[0] == ((XnUInt8*)&nMagic)[1])	// we have here second byte
			{
				// move to next byte
				pBuffer++;

				// move to next state
				pCurrState->CurrHeader.nMagic = nMagic;
				pCurrState->State = XN_PACKET_HEADER;
				pCurrState->nMissingBytesInState = sizeof(XnSensorProtocolResponseHeader);
				break;
			}

			while (pBuffer < pBufEnd)
			{
				if (nMagic == *(XnUInt16*)(pBuffer))
				{
					pCurrState->CurrHeader.nMagic = nMagic;
					pCurrState->State = XN_PACKET_HEADER;
					pCurrState->nMissingBytesInState = sizeof(XnSensorProtocolResponseHeader);
					break;
				}
				else
					pBuffer++;
			}

			if (pBuffer == pBufEnd &&					// magic wasn't found
				pBuffer[-1] == ((XnUInt8*)&nMagic)[0])	// last byte in buffer is first in magic
			{
				// mark that we found first one
				pCurrState->nMissingBytesInState--;
			}

			break;

		case XN_PACKET_HEADER:
			nReadBytes = XN_MIN((XnUInt32)(pBufEnd - pBuffer), pCurrState->nMissingBytesInState);
			xnOSMemCopy((XnUChar*)&pCurrState->CurrHeader + sizeof(XnSensorProtocolResponseHeader) - pCurrState->nMissingBytesInState, 
				pBuffer, nReadBytes);
			pCurrState->nMissingBytesInState -= nReadBytes;
			pBuffer += nReadBytes;

			if (pCurrState->nMissingBytesInState == 0)
			{
				// we have entire header. Fix it
				pCurrState->CurrHeader.nBufSize = XN_PREPARE_VAR16_IN_BUFFER(pCurrState->CurrHeader.nBufSize);
				pCurrState->CurrHeader.nMagic = XN_PREPARE_VAR16_IN_BUFFER(pCurrState->CurrHeader.nMagic);
				pCurrState->CurrHeader.nPacketID = XN_PREPARE_VAR16_IN_BUFFER(pCurrState->CurrHeader.nPacketID);
				pCurrState->CurrHeader.nTimeStamp = XN_PREPARE_VAR32_IN_BUFFER(pCurrState->CurrHeader.nTimeStamp);
				pCurrState->CurrHeader.nType = XN_PREPARE_VAR16_IN_BUFFER(pCurrState->CurrHeader.nType);
				pCurrState->CurrHeader.nBufSize = xnOSEndianSwapUINT16(pCurrState->CurrHeader.nBufSize);
				pCurrState->CurrHeader.nBufSize -= sizeof(XnSensorProtocolResponseHeader);

				pCurrState->State = XN_PACKET_DATA;
				pCurrState->nMissingBytesInState = pCurrState->CurrHeader.nBufSize;
			}
			break;

		case XN_PACKET_DATA:
			nReadBytes = XN_MIN((XnUInt32)(pBufEnd - pBuffer), pCurrState->nMissingBytesInState);
			pDevicePrivateData->pSensor->GetFirmware()->GetStreams()->ProcessPacketChunk(&pCurrState->CurrHeader, pBuffer, pCurrState->CurrHeader.nBufSize - pCurrState->nMissingBytesInState, nReadBytes);
			pBuffer += nReadBytes;
			pCurrState->nMissingBytesInState -= nReadBytes;

			if (pCurrState->nMissingBytesInState == 0)
			{
				pCurrState->State = XN_LOOKING_FOR_MAGIC;
				pCurrState->nMissingBytesInState = sizeof(XnUInt16);
			}
			break;
		}
	}

	XN_PROFILING_END_SECTION;

	return TRUE;
}

XnStatus XnDeviceSensorProtocolFindStreamOfType(XnDevicePrivateData* pDevicePrivateData, const XnChar* strType, const XnChar** ppStreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	const XnChar* strNames[100];
	XnUInt32 nCount = 100;

	nRetVal = pDevicePrivateData->pSensor->GetStreamNames(strNames, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		XnChar strCurType[XN_DEVICE_MAX_STRING_LENGTH];
		nRetVal = pDevicePrivateData->pSensor->GetProperty(strNames[i], XN_STREAM_PROPERTY_TYPE, strCurType);
		XN_IS_STATUS_OK(nRetVal);

		if (strcmp(strType, strCurType) == 0)
		{
			*ppStreamName = strNames[i];
			return (XN_STATUS_OK);
		}
	}

	*ppStreamName = NULL;
	return (XN_STATUS_NO_MATCH);
}

XnStatus XnDeviceSensorProtocolDumpLastRawFrameImpl(XnDevicePrivateData* pDevicePrivateData, const XnChar* strType, const XnChar* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	const XnChar* strName;
	nRetVal = XnDeviceSensorProtocolFindStreamOfType(pDevicePrivateData, strType, &strName);
	XN_IS_STATUS_OK(nRetVal);

	XnUInt64 nMaxDataSize;
	nRetVal = pDevicePrivateData->pSensor->GetProperty(strName, XN_STREAM_PROPERTY_REQUIRED_DATA_SIZE, &nMaxDataSize);
	XN_IS_STATUS_OK(nRetVal);

	XnDynamicSizeBuffer dsb;
	dsb.nMaxSize = (XnUInt32)nMaxDataSize;
	dsb.pData = xnOSMallocAligned((XnUInt32)nMaxDataSize, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALLOC_PTR(dsb.pData);

	nRetVal = pDevicePrivateData->pSensor->GetProperty(strName, XN_STREAM_PROPERTY_LAST_RAW_FRAME, XN_PACK_GENERAL_BUFFER(dsb));
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFreeAligned(dsb.pData);
		return (nRetVal);
	}

	xnOSSaveFile(strFileName, dsb.pData, dsb.nDataSize);

	xnOSFreeAligned(dsb.pData);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceSensorProtocolDumpLastRawFrame(XnDevicePrivateData* pDevicePrivateData, const XnChar* strType, const XnChar* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	printf("* Dumping %s...\n", strType);

	nRetVal = XnDeviceSensorProtocolDumpLastRawFrameImpl(pDevicePrivateData, strType, strFileName);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("** Failed! %s\n", xnGetStatusString(nRetVal));
	}
	else
	{
		printf ("** Saved %s to %s\n", strType, strFileName);
	}
	
	return (XN_STATUS_OK);
}

