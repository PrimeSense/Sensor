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
#include "XnDeviceFileReader.h"
#include <XnLog.h>
#include "XnDeviceFile.h"
#include <XnDDK/XnActualRealProperty.h>
#include <XnDDK/XnStreamReaderStream.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnLastStreamData
{
	XnUInt32 nPosition;
	XnUInt32 nFrameID;
	XnUInt64 nTimestamp;
} XnLastStreamData;

XN_DECLARE_STRINGS_HASH(XnLastStreamData, XnLastStreamDataHash);

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDeviceFileReader::XnDeviceFileReader() :
	XnStreamReaderDevice(XN_DEVICE_NAME, XN_DEVICE_FILE_MAX_INTERNAL_BUFFER),
	m_FrameDelay(XN_MODULE_PROPERTY_FRAME_DELAY, FALSE),
	m_pBCData(NULL),
	m_nFileVersion(-1),
	m_nReferenceTime(0),
	m_nReferenceTimestamp(0),
	m_bFileHasData(FALSE),
	m_bStreamsCollectionChanged(FALSE),
	m_pThis(this),
	m_InstancePointer(XN_FILE_PROPERTY_INSTANCE_POINTER, &m_pThis, sizeof(m_pThis), NULL)
{
	m_FrameDelay.UpdateSetCallbackToDefault();
}

XnDeviceFileReader::~XnDeviceFileReader()
{

}

XnStatus XnDeviceFileReader::InitImpl(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnStreamReaderDevice::InitImpl(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	// register to events
	nRetVal = OnStreamCollectionChangedEvent().Register(StreamCollectionChangedCallback, this);
	XN_IS_STATUS_OK(nRetVal);

	// TODO: remove this
	ReadWriteModeProperty().UnsafeUpdateValue(XN_DEVICE_MODE_READ);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnStreamReaderDevice::CreateDeviceModule(ppModuleHolder);
	XN_IS_STATUS_OK(nRetVal);

	XnDeviceModule* pModule = (*ppModuleHolder)->GetModule();

	// add sensor properties
	XnProperty* pProps[] = { &m_FrameDelay, &m_InstancePointer };

	nRetVal = pModule->AddProperties(pProps, sizeof(pProps)/sizeof(XnProperty*));
	if (nRetVal != XN_STATUS_OK)
	{
		DestroyModule(*ppModuleHolder);
		*ppModuleHolder = NULL;
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::CreateIOStreamImpl(const XnChar* strConnectionString, XnIOStream*& pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// open file
	XN_VALIDATE_NEW_AND_INIT(pStream, XnIOFileStream, strConnectionString, XN_OS_FILE_READ);

	// read version
	nRetVal = ReadFileVersion();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pStream);
		pStream = NULL;
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

void XnDeviceFileReader::DestroyIOStreamImpl(XnIOStream* pStream)
{
	XN_DELETE(pStream);
}

XnStatus XnDeviceFileReader::ReadFileVersion()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read magic from file
	XnChar csFileMagic[XN_DEVICE_FILE_MAGIC_LEN];
	nRetVal = GetIOStream()->ReadData((XnUChar*)csFileMagic, XN_DEVICE_FILE_MAGIC_LEN);
	XN_IS_STATUS_OK(nRetVal);

	if (strncmp(csFileMagic, XN_DEVICE_FILE_MAGIC_V4, XN_DEVICE_FILE_MAGIC_LEN) == 0)
	{
		m_nFileVersion = 4;
	}
	else if (strncmp(csFileMagic, XN_DEVICE_FILE_MAGIC_V3, XN_DEVICE_FILE_MAGIC_LEN) == 0)
	{
		m_nFileVersion = 3;
	}
	else if (strncmp(csFileMagic, XN_DEVICE_FILE_MAGIC_V2, XN_DEVICE_FILE_MAGIC_LEN) == 0)
	{
		m_nFileVersion = 2;
	}
	else if (strncmp(csFileMagic, XN_DEVICE_FILE_MAGIC_V1, XN_DEVICE_FILE_MAGIC_LEN) == 0)
	{
		m_nFileVersion = 1;
	}
	else
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_DEVICE_FILE_CORRUPTED, XN_MASK_FILE, "Invalid file magic!");
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::ReadInitialState(XnPropertySet *pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_nFileVersion < 4)
	{
		if (m_pBCData == NULL)
		{
			nRetVal = BCInit();
			XN_IS_STATUS_OK(nRetVal);
		}

		return BCReadInitialState(pSet);
	}

	// first read first object - modules properties - using base
	nRetVal = XnStreamReaderDevice::ReadInitialState(pSet);
	XN_IS_STATUS_OK(nRetVal);

	// now continue reading until we get to first data
	XnPackedDataType nType;
	XnBool bStateEnd = FALSE;
	XnUInt32 nPositionBefore;

	while (!bStateEnd)
	{
		nRetVal = GetIOStream()->Tell(&nPositionBefore);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetDataPacker()->ReadNextObject(&nType);
		XN_IS_STATUS_OK(nRetVal);

		switch (nType)
		{
		case XN_PACKED_NEW_STREAM:
			{
				XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];
				XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];
				XN_PROPERTY_SET_CREATE_ON_STACK(props);
				nRetVal = GetDataPacker()->ReadNewStream(strType, strName, &props);
				XN_IS_STATUS_OK(nRetVal);

				XnActualPropertiesHash* pStreamProps;
				nRetVal = XnPropertySetDataDetachModule(props.pData, strName, &pStreamProps);
				XN_IS_STATUS_OK(nRetVal);

				nRetVal = XnPropertySetDataAttachModule(pSet->pData, strName, pStreamProps);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PACKED_INT_PROPERTY:
			{
				XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
				XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
				XnUInt64 nValue;
				nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, &nValue);
				XN_IS_STATUS_OK(nRetVal);

				XnActualPropertiesHash* pModule;
				nRetVal = pSet->pData->Get(strModule, pModule);
				XN_IS_STATUS_OK(nRetVal);

				XnProperty* pProp;
				nRetVal = pModule->Get(strProp, pProp);
				XN_IS_STATUS_OK(nRetVal);

				XnActualIntProperty* pIntProp = (XnActualIntProperty*)pProp;
				nRetVal = pIntProp->UnsafeUpdateValue(nValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PACKED_REAL_PROPERTY:
			{
				XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
				XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
				XnDouble dValue;
				nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, &dValue);
				XN_IS_STATUS_OK(nRetVal);

				XnActualPropertiesHash* pModule;
				nRetVal = pSet->pData->Get(strModule, pModule);
				XN_IS_STATUS_OK(nRetVal);

				XnProperty* pProp;
				nRetVal = pModule->Get(strProp, pProp);
				XN_IS_STATUS_OK(nRetVal);

				XnActualRealProperty* pRealProp = (XnActualRealProperty*)pProp;
				nRetVal = pRealProp->UnsafeUpdateValue(dValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PACKED_STRING_PROPERTY:
			{
				XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
				XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
				XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
				nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, strValue);
				XN_IS_STATUS_OK(nRetVal);

				XnActualPropertiesHash* pModule;
				nRetVal = pSet->pData->Get(strModule, pModule);
				XN_IS_STATUS_OK(nRetVal);

				XnProperty* pProp;
				nRetVal = pModule->Get(strProp, pProp);
				XN_IS_STATUS_OK(nRetVal);

				XnActualStringProperty* pStringProp = (XnActualStringProperty*)pProp;
				nRetVal = pStringProp->UnsafeUpdateValue(strValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PACKED_GENERAL_PROPERTY:
			{
				XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
				XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
				XnGeneralBuffer gbValue;
				nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, &gbValue);
				XN_IS_STATUS_OK(nRetVal);

				XnActualPropertiesHash* pModule;
				nRetVal = pSet->pData->Get(strModule, pModule);
				XN_IS_STATUS_OK(nRetVal);

				XnProperty* pProp;
				nRetVal = pModule->Get(strProp, pProp);
				XN_IS_STATUS_OK(nRetVal);

				XnActualGeneralProperty* pIntProp = (XnActualGeneralProperty*)pProp;
				nRetVal = pIntProp->UnsafeUpdateValue(gbValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		default:
			// reached end of initial state. go back to beginning of this object
			nRetVal = GetIOStream()->Seek(nPositionBefore);
			XN_IS_STATUS_OK(nRetVal);

			// stop reading
			bStateEnd = TRUE;
		}
	} // objects loop
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::HandleStreamRemoved(const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check for specific case: all streams are removed and then end-of-file is reached.
	// in this case, we don't really want to destroy streams, just wrap around.
	XnStringsHash StreamsToRemove;
	nRetVal = StreamsToRemove.Set(strName, NULL);
	XN_IS_STATUS_OK(nRetVal);

	XnPackedDataType nType = XN_PACKED_STREAM_REMOVED;
	XnUInt32 nPositionBefore;

	while (TRUE)
	{
		nRetVal = GetIOStream()->Tell(&nPositionBefore);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetDataPacker()->ReadNextObject(&nType);
		XN_IS_STATUS_OK(nRetVal);

		if (nType == XN_PACKED_STREAM_REMOVED)
		{
			XnChar strTempName[XN_DEVICE_MAX_STRING_LENGTH];
			nRetVal = GetDataPacker()->ReadStreamRemoved(strTempName);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = StreamsToRemove.Set(strTempName, NULL);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			break;
		}
	}

	if (nType != XN_PACKED_END)
	{
		// Not the case we were looking for. Remove those streams.
		for (XnStringsHash::Iterator it = StreamsToRemove.begin(); it != StreamsToRemove.end(); ++it)
		{
			nRetVal = XnStreamReaderDevice::HandleStreamRemoved(it.Key());
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	// in any case, the last object we read wasn't handled yet (end-of-stream or another event), so
	// seek back, so it will be handled.
	nRetVal = GetIOStream()->Seek(nPositionBefore);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::HandleIntProperty(const XnChar* strModule, const XnChar* strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// ignore some properties
	if ((strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 && strcmp(strName, XN_MODULE_PROPERTY_READ_WRITE_MODE) == 0) ||
		(strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 && strcmp(strName, XN_MODULE_PROPERTY_PRIMARY_STREAM) == 0) ||
		(strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 && strcmp(strName, XN_MODULE_PROPERTY_FRAME_DELAY) == 0) ||
		(strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 && strcmp(strName, XN_MODULE_PROPERTY_DEVICE_NAME) == 0))
	{
		return (XN_STATUS_OK);
	}
	else
	{
		nRetVal = XnStreamReaderDevice::HandleIntProperty(strModule, strName, nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::HandleStreamData(XnStreamData* pDataProps, XnCompressionFormats nCompression, XnUInt32 nCompressedSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt32 nPosition;
	nRetVal = GetIOStream()->Tell(&nPosition);
	XN_IS_STATUS_OK(nRetVal);

	XnUIntHash::Iterator it = m_PositionsToIgnore.end();
	if (XN_STATUS_OK == m_PositionsToIgnore.Find(nPosition, it))
	{
		// ignore this one. Just update the frame ID
		XnStreamDeviceStreamHolder* pHolder;
		nRetVal = FindStream(pDataProps->StreamName, &pHolder);
		XN_IS_STATUS_OK(nRetVal);

		XnStreamReaderStream* pStream = (XnStreamReaderStream*)pHolder->GetStream();
		pStream->NewDataAvailable(pDataProps->nTimestamp, pDataProps->nFrameID);

		// and remove it from list
		nRetVal = m_PositionsToIgnore.Remove(it);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// normal case. handle it
		nRetVal = XnStreamReaderDevice::HandleStreamData(pDataProps, nCompression, nCompressedSize);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::Rewind()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// go back to start of stream
	nRetVal = GetIOStream()->Seek(XN_DEVICE_FILE_MAGIC_LEN);
	XN_IS_STATUS_OK(nRetVal);

	// read initial state
	XN_PROPERTY_SET_CREATE_ON_STACK(state);
	nRetVal = ReadInitialState(&state);
	XN_IS_STATUS_OK(nRetVal);

	// first handle current streams. remove or reset them
	XnDeviceModuleHolderList streams;
	nRetVal = GetStreamsList(streams);
	XN_IS_STATUS_OK(nRetVal);

	for (XnDeviceModuleHolderList::Iterator it = streams.begin(); it != streams.end(); ++it)
	{
		XnDeviceModuleHolder* pHolder = *it;

		if (m_bStreamsCollectionChanged)
		{
			// we need to destroy all streams, and recreate them later
			nRetVal = DestroyStream(pHolder->GetModule()->GetName());
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			// just reset frame ID
			XnStreamReaderStream* pStream = (XnStreamReaderStream*)pHolder->GetModule();
			pStream->Reset();
		}
	}

	// if we need, recreate streams
	if (m_bStreamsCollectionChanged)
	{
		nRetVal = CreateStreams(&state);
		XN_IS_STATUS_OK(nRetVal);
	}

	// now set state.
	for (XnPropertySetData::Iterator it = state.pData->begin(); it != state.pData->end(); ++it)
	{
		const XnChar* strName = it.Key();
		XnActualPropertiesHash* pHash = it.Value();

		// fix it first
		if (strcmp(strName, XN_MODULE_NAME_DEVICE) == 0)
		{
			pHash->Remove(XN_MODULE_PROPERTY_READ_WRITE_MODE);
			pHash->Remove(XN_MODULE_PROPERTY_PRIMARY_STREAM);
		}

		XnDeviceModule* pModule;
		nRetVal = FindModule(strName, &pModule);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = pModule->UnsafeBatchConfig(*pHash);
		XN_IS_STATUS_OK(nRetVal);
	}

	ResetLastTimestampAndFrame();
	m_nReferenceTimestamp = 0;
	m_nReferenceTime = 0;
	m_bStreamsCollectionChanged = FALSE;

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::HandleEndOfStream()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (!m_bFileHasData)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_DEVICE_FILE_CORRUPTED, XN_MASK_FILE, "File does not contain any data...");
	}

	nRetVal = Rewind();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnDeviceFileReader::FrameDelay(XnUInt64 nTimestamp)
{
	if (m_FrameDelay.GetValue() != TRUE)
		return;

	if (!IsHighResTimestamps())
		nTimestamp *= 1000;

	// first time
	if (m_nReferenceTime == 0)
	{
		xnOSQueryTimer(m_FrameDelayTimer, &m_nReferenceTime);
		m_nReferenceTimestamp = nTimestamp;
		return;
	}

	// delay
	XnUInt64 nNow;
	xnOSQueryTimer(m_FrameDelayTimer, &nNow);

	// check how much time has passed in the stream
	XnUInt64 nStreamDiff;
	if (nTimestamp < m_nReferenceTimestamp)
	{
		nStreamDiff = 0;
	}
	else
	{
		nStreamDiff = nTimestamp - m_nReferenceTimestamp;
	}

	// check how much time passed (for user)
	XnUInt64 nClockDiff = nNow - m_nReferenceTime;

	// update reference (so that frame delay will work with Pause / Resume)
	m_nReferenceTime = nNow;
	m_nReferenceTimestamp = nTimestamp;

	// check if we need to wait
	if (nClockDiff < nStreamDiff)
	{
		xnOSSleep(XnUInt32((nStreamDiff - nClockDiff) / 1000));

		// take this time as a reference
		xnOSQueryTimer(m_FrameDelayTimer, &m_nReferenceTime);
	}
}

XnStatus XnDeviceFileReader::WaitForPrimaryStream(XN_EVENT_HANDLE hNewDataEvent, XnStreamDataSet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read until primary stream advanced
	while (!HasPrimaryStreamAdvanced(pSet))
	{
		XnBool bWrap;
		nRetVal = ReadTillNextData(&bWrap);
		XN_IS_STATUS_OK(nRetVal);
	}

	FrameDelay(GetLastTimestamp());

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::WaitForStream(XN_EVENT_HANDLE hNewDataEvent, XnDeviceStream* pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// play forward until we have new data in this stream
	while (!pStream->IsNewDataAvailable())
	{
		XnBool bWrap;
		nRetVal = ReadTillNextData(&bWrap);
		XN_IS_STATUS_OK(nRetVal);
	}

	FrameDelay(pStream->GetLastTimestamp());

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::ReadTillNextData(XnBool* pbWrapOccurred)
{
	XnStatus nRetVal = XN_STATUS_OK;

	*pbWrapOccurred = FALSE;

	if (m_nFileVersion < 4)
	{
		nRetVal = BCReadFrame(pbWrapOccurred);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		XnPackedDataType nType = XN_PACKED_END;
		while (nType != XN_PACKED_STREAM_DATA)
		{
			nRetVal = ReadNextEventFromStream(&nType);
			XN_IS_STATUS_OK(nRetVal);

			if (nType == XN_PACKED_END)
			{
				*pbWrapOccurred = TRUE;
			}
		}

		m_bFileHasData = TRUE;
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::SeekTo(XnUInt64 nMinTimestamp, XnUInt32 nMinFrameID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// first check if we need to seek forward or backwards (even if we're in the correct location,
	// we need to rewind, so that next read will return this frame again).
	if ((nMinTimestamp != 0 && nMinTimestamp <= GetLastTimestamp()) ||
		(nMinFrameID != 0 && nMinFrameID <= GetLastFrameID()))
	{
		nRetVal = Rewind();
		XN_IS_STATUS_OK(nRetVal);
	}

	XnBool bFoundNewData = FALSE;

	// Keep current position.
	XnUInt32 nStartingPosition;
	nRetVal = GetIOStream()->Tell(&nStartingPosition);
	XN_IS_STATUS_OK(nRetVal);

	// Take primary stream (it determines frame ID and timestamp)
	XnPackedDataType nType = (XnPackedDataType)-1;
	const XnChar* strPrimaryStream = GetPrimaryStream();
	if (strcmp(strPrimaryStream, XN_PRIMARY_STREAM_ANY) == 0 ||
		strcmp(strPrimaryStream, XN_PRIMARY_STREAM_NONE) == 0)
	{
		strPrimaryStream = NULL;
	}

	// start seeking forward until point is reached.
	XnUInt32 nFoundPosition;
	XnLastStreamDataHash StreamsHash;

	while (TRUE)
	{
		XnUInt32 nPositionBefore;
		nRetVal = GetIOStream()->Tell(&nPositionBefore);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetDataPacker()->ReadNextObject(&nType);
		XN_IS_STATUS_OK(nRetVal);

		XnUInt32 nPositionAfter;
		nRetVal = GetIOStream()->Tell(&nPositionAfter);
		XN_IS_STATUS_OK(nRetVal);

		if (nType == XN_PACKED_STREAM_DATA)
		{
			bFoundNewData = TRUE;

			XnStreamData props;
			XnCompressionFormats nCompression;
			XnUInt32 nCompressedSize;
			nRetVal = GetDataPacker()->ReadStreamDataProps(&props, &nCompression, &nCompressedSize);
			XN_IS_STATUS_OK(nRetVal);

			XnLastStreamData data;
			if (XN_STATUS_OK != StreamsHash.Get(props.StreamName, data))
			{
				XnStreamDeviceStreamHolder* pHolder;
				nRetVal = FindStream(props.StreamName, &pHolder);
				XN_IS_STATUS_OK(nRetVal);

				data.nFrameID = pHolder->GetStream()->GetLastFrameID() + 1;
			}
			else
			{
				// if we had previous data from this stream, ignore it
				m_PositionsToIgnore.Set(data.nPosition, 0);

				++data.nFrameID;
			}

			data.nPosition = nPositionAfter;
			data.nTimestamp = props.nTimestamp;
			nRetVal = StreamsHash.Set(props.StreamName, data);
			XN_IS_STATUS_OK(nRetVal);

			// now check if condition is met
			if (strPrimaryStream == NULL ||
				strcmp(strPrimaryStream, props.StreamName) == 0)
			{
				if (data.nFrameID >= nMinFrameID && data.nTimestamp >= nMinTimestamp)
				{
					// we have everything we need
					// keep this position (we'll read up till here).
					nFoundPosition = nPositionAfter;
					break;
				}
			}
		}
		else if (nType == XN_PACKED_END)
		{
			// we'll read up to the last data of each stream
			nFoundPosition = nPositionBefore;
			break;
		}
	}

	// now seek back
	nRetVal = GetIOStream()->Seek(nStartingPosition);
	XN_IS_STATUS_OK(nRetVal);

	if (bFoundNewData)
	{
		// read everything up to position
		XnUInt32 nPositionAfter = nStartingPosition;

		while (nPositionAfter < nFoundPosition)
		{
			nRetVal = ReadNextEventFromStream(&nType);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = GetIOStream()->Tell(&nPositionAfter);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	else
	{
		// just remark the data as new (this is last frame, return it again to user)
		XnDeviceModuleHolderList streams;
		nRetVal = GetStreamsList(streams);
		XN_IS_STATUS_OK(nRetVal);

		for (XnDeviceModuleHolderList::Iterator it = streams.begin(); it != streams.end(); ++it)
		{
			XnStreamReaderStream* pStream = (XnStreamReaderStream*)(*it)->GetModule();
			pStream->ReMarkDataAsNew();
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::Seek(XnUInt64 nTimestamp)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogInfo(XN_MASK_FILE, "Seeking file to timestamp %llu...", nTimestamp);

	if (m_nFileVersion < 4)
	{
		return BCSeek(nTimestamp);
	}

	nRetVal = SeekTo(nTimestamp, 0);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::SeekFrame(XnUInt32 nFrameID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// don't allow seeking to frame 0
	nFrameID = XN_MAX(nFrameID, 1);

	xnLogInfo(XN_MASK_FILE, "Seeking file to frame %u...", nFrameID);

	if (m_nFileVersion < 4)
	{
		return BCSeekFrame(nFrameID);
	}

	nRetVal = SeekTo(0, nFrameID);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileReader::OnStreamCollectionChanged(const XnChar* StreamName, XnStreamsChangeEventType EventType)
{
	m_bStreamsCollectionChanged = TRUE;
	return XN_STATUS_OK;
}

void XnDeviceFileReader::StreamCollectionChangedCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnStreamsChangeEventType EventType, void* pCookie)
{
	XnDeviceFileReader* pThis = (XnDeviceFileReader*)pCookie;
	pThis->OnStreamCollectionChanged(StreamName, EventType);
}

XnStatus XnDeviceFileReader::ReadNextData()
{
	XnBool bDummy;
	return ReadTillNextData(&bDummy);
}
