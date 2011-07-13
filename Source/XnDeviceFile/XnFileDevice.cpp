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
#include "XnFileDevice.h"
#include <XnDDK/XnDataPacker.h>
#include <XnCodecIDs.h>
#include <XnPropNames.h>
#include "XnDeviceFileReaderBC.h"
#include <math.h>
#include <XnFormats/XnJpegCodec.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_FILE_MAX_UNCOMPRESSED_DATA_SIZE	(1600 * 1200 * sizeof(XnRGB24Pixel))

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
// XnFileDevice class
//---------------------------------------------------------------------------
XnFileDevice::XnFileDevice(xn::Context& context, const XnChar* strInstanceName) :
	m_context(context),
	m_pInputStream(NULL),	
	m_pDataPacker(NULL),
	m_pNotifications(NULL),
	m_pNotificationsCookie(NULL),
	m_bHighresTimestamps(FALSE),
	m_pStreamData(NULL),
	m_bFileHasData(FALSE),
	m_bNodeCollectionChanged(FALSE),
	m_nCurrTimestamp(0),
	m_pBCData(NULL),
	m_bRepeat(TRUE),
	m_bEOF(FALSE),
	m_strName(strInstanceName),
	m_hSelf(NULL)
{
	xnOSMemSet(&m_ShiftToDepth, 0, sizeof(m_ShiftToDepth));
}

XnFileDevice::~XnFileDevice()
{
	Free();
}

XnStatus XnFileDevice::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnStreamDataCreate(&m_pStreamData, "", XN_FILE_MAX_UNCOMPRESSED_DATA_SIZE);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

void XnFileDevice::Free()
{
	for (XnNodeInfoMap::Iterator it = m_nodeInfoMap.begin(); it != m_nodeInfoMap.end(); ++it)
	{
		XnNodeInfo& nodeInfo = it.Value();
		XN_DELETE(nodeInfo.pXnCodec);
		if (nodeInfo.codec.IsValid())
		{
			xnRemoveNeededNode(GetSelfNodeHandle(), nodeInfo.codec);
			nodeInfo.codec.Release();
		}

	}
	m_nodeInfoMap.Clear();

	if (m_ShiftToDepth.bIsInitialized)
	{
		XnShiftToDepthFree(&m_ShiftToDepth);
	}

	if (m_pInputStream != NULL)
	{
		XN_DELETE(m_pInputStream);
		m_pInputStream = NULL;
	}

	if (m_pDataPacker != NULL)
	{
		m_pDataPacker->Free();
		XN_DELETE(m_pDataPacker);
		m_pDataPacker = NULL;
	}

	if (m_pStreamData != NULL)
	{
		XnStreamDataDestroy(&m_pStreamData);
		m_pStreamData = NULL;
	}
}

XnStatus XnFileDevice::ReadFileVersion()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read magic from file
	XnChar csFileMagic[XN_DEVICE_FILE_MAGIC_LEN];
	nRetVal = m_pInputStream->ReadData((XnUChar*)csFileMagic, XN_DEVICE_FILE_MAGIC_LEN);
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
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_FILE_CORRUPTED, XN_MASK_FILE, "Invalid file magic!");
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadInitialState(XnPropertySet* pSet)
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

	// read an object from data packer
	XnPackedDataType nType;
	nRetVal = m_pDataPacker->ReadNextObject(&nType);
	XN_IS_STATUS_OK(nRetVal);

	if (nType != XN_PACKED_PROPERTY_SET)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_FILE_CORRUPTED, XN_MASK_DDK, "Stream does not start with a property set!");
	}

	nRetVal = m_pDataPacker->ReadPropertySet(pSet);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::SetInitialState(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Fix state (remove some properties that we don't wish to reflect in reader device)
	XnActualPropertiesHash* pDeviceModule = NULL;
	if (XN_STATUS_OK == pSet->pData->Get(XN_MODULE_NAME_DEVICE, pDeviceModule))
	{
		pDeviceModule->Remove(XN_MODULE_PROPERTY_READ_WRITE_MODE);
		pDeviceModule->Remove(XN_MODULE_PROPERTY_PRIMARY_STREAM);

		// check for timestamps resolution
		XnActualIntProperty* pIntProp;
		if (XN_STATUS_OK == pDeviceModule->Get(XN_MODULE_PROPERTY_HIGH_RES_TIMESTAMPS, (XnProperty*&)pIntProp))
		{
			m_bHighresTimestamps = (pIntProp->GetValue() == TRUE);
		}
	}

	// TODO: create DEVICE node

	// now create the rest of the modules and streams (DEVICE was already created)
	XnPropertySetData* pPropSetData = pSet->pData;
	for (XnPropertySetData::ConstIterator it = pPropSetData->begin(); it != pPropSetData->end(); ++it)
	{
		// ignore module DEVICE
		if (strcmp(XN_MODULE_NAME_DEVICE, it.Key()) == 0)
		{
			continue;
		}

		// check if this is a stream
		XnActualPropertiesHash::ConstIterator itProp = it.Value()->end();
		if (XN_STATUS_OK == it.Value()->Find(XN_STREAM_PROPERTY_TYPE, itProp))
		{
			XnActualStringProperty* pTypeProp = (XnActualStringProperty*)itProp.Value();
			nRetVal = HandleNewStream(pTypeProp->GetValue(), it.Key(), it.Value());
			XN_IS_STATUS_OK(nRetVal);
		}
	} // modules loop

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::SetInputStream(void *pStreamCookie, XnPlayerInputStreamInterface *pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_NEW(m_pInputStream, XnInputStream, pStream, pStreamCookie);
	
	nRetVal = m_pInputStream->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(m_pInputStream);
		m_pInputStream = NULL;
		return (nRetVal);
	}

	// read format version
	nRetVal = ReadFileVersion();
	XN_IS_STATUS_OK(nRetVal);

	m_pDataPacker = XN_NEW(XnDataPacker, m_pInputStream, XN_DEVICE_FILE_MAX_INTERNAL_BUFFER);
	if (m_pDataPacker == NULL)
	{
		XN_DELETE(m_pInputStream);
		return (XN_STATUS_ALLOC_FAILED);
	}

	nRetVal = m_pDataPacker->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(m_pDataPacker);
		XN_DELETE(m_pInputStream);
		return (nRetVal);
	}

	// read initial state
	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	nRetVal = ReadInitialState(&props);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SetInitialState(&props);
	XN_IS_STATUS_OK(nRetVal);

	// now read till first data
	XnBool bWrap;
	nRetVal = ReadTillNextData(&bWrap);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::SetNodeNotifications(void *pNodeNotificationsCookie, XnNodeNotifications *pNodeNotifications)
{
	m_pNotifications = pNodeNotifications;
	m_pNotificationsCookie = pNodeNotificationsCookie;
	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::SetRepeat(XnBool bRepeat)
{
	m_bRepeat = bRepeat;
	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::SeekTo(XnUInt64 nMinTimestamp, const XnChar* strNodeName, XnUInt32 nMinFrameID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// first check if we need to seek forward or backwards (even if we're in the correct location,
	// we need to rewind, so that next read will return this frame again).
	XnNodeInfo* pNodeInfo = NULL;
	if (strNodeName != NULL)
	{
		nRetVal = m_nodeInfoMap.Get(strNodeName, pNodeInfo);
		XN_IS_STATUS_OK(nRetVal);

		if (nMinFrameID <= pNodeInfo->nCurrFrameID)
		{
			nRetVal = Rewind();
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	else
	{
		if (nMinTimestamp <= m_nCurrTimestamp)
		{
			nRetVal = Rewind();
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	XnBool bFoundNewData = FALSE;

	// Keep current position.
	XnUInt32 nStartingPosition;
	nRetVal = m_pInputStream->Tell(&nStartingPosition);
	XN_IS_STATUS_OK(nRetVal);

	// start seeking forward until point is reached.
	XnUInt32 nFoundPosition;
	XnPackedDataType nType = (XnPackedDataType)-1;
	XnLastStreamDataHash StreamsHash;

	while (TRUE)
	{
		XnUInt32 nPositionBefore;
		nRetVal = m_pInputStream->Tell(&nPositionBefore);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pDataPacker->ReadNextObject(&nType);
		XN_IS_STATUS_OK(nRetVal);

		XnUInt32 nPositionAfter;
		nRetVal = m_pInputStream->Tell(&nPositionAfter);
		XN_IS_STATUS_OK(nRetVal);

		if (nType == XN_PACKED_STREAM_DATA)
		{
			bFoundNewData = TRUE;

			XnStreamData props;
			XnCompressionFormats nCompression;
			XnUInt32 nCompressedSize;
			nRetVal = m_pDataPacker->ReadStreamDataProps(&props, &nCompression, &nCompressedSize);
			XN_IS_STATUS_OK(nRetVal);

			XnLastStreamData data;
			if (XN_STATUS_OK != StreamsHash.Get(props.StreamName, data))
			{
				XnNodeInfo* pNodeInfo;
				nRetVal = m_nodeInfoMap.Get(props.StreamName, pNodeInfo);
				XN_IS_STATUS_OK(nRetVal);

				data.nFrameID = pNodeInfo->nCurrFrameID + 1;
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
			if (strNodeName != NULL)
			{
				if (strcmp(strNodeName, props.StreamName) == 0 &&
					data.nFrameID >= nMinFrameID)
				{
					// keep this position (we'll read up till here).
					nFoundPosition = nPositionAfter;
					break;
				}
			}
			else if (data.nTimestamp >= nMinTimestamp)
			{
				// keep this position (we'll read up till here).
				nFoundPosition = nPositionAfter;
				break;
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
	nRetVal = m_pInputStream->Seek(nStartingPosition);
	XN_IS_STATUS_OK(nRetVal);

	if (bFoundNewData)
	{
		// read everything up to position
		XnUInt32 nPositionAfter = nStartingPosition;

		while (nPositionAfter < nFoundPosition)
		{
			nRetVal = ReadNextEventFromStream(&nType);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = m_pInputStream->Tell(&nPositionAfter);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	else
	{
/*		// just remark the data as new (this is last frame, return it again to user)
		XnDeviceModuleHolderList streams;
		nRetVal = GetStreamsList(streams);
		XN_IS_STATUS_OK(nRetVal);

		for (XnDeviceModuleHolderList::Iterator it = streams.begin(); it != streams.end(); ++it)
		{
			XnStreamReaderStream* pStream = (XnStreamReaderStream*)(*it)->GetModule();
			pStream->ReMarkDataAsNew();
		}
*/	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::SeekToTimeStamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt64 nTimestamp = 0;
	if (origin == XN_PLAYER_SEEK_CUR)
	{
		nTimestamp = m_nCurrTimestamp + nTimeOffset;
	}
	else if (origin == XN_PLAYER_SEEK_SET)
	{
		nTimestamp = nTimeOffset;
	}
	else
	{
		// TODO: find max timestamp and add offset
		return XN_STATUS_NOT_IMPLEMENTED;
	}

	xnLogInfo(XN_MASK_FILE, "Seeking file to timestamp %llu...", nTimestamp);

	if (m_nFileVersion < 4)
	{
		return BCSeek(nTimestamp);
	}
	else
	{
		return SeekTo(nTimestamp, NULL, 0);
	}
}

XnStatus XnFileDevice::SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnNodeInfo* pNodeInfo;
	nRetVal = m_nodeInfoMap.Get(strNodeName, pNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	XnInt32 nFrameID = 0;

	switch (origin)
	{
	case XN_PLAYER_SEEK_CUR:
		nFrameID = pNodeInfo->nCurrFrameID + nFrameOffset;
		break;
	case XN_PLAYER_SEEK_SET:
		nFrameID = nFrameOffset;
		break;
	case XN_PLAYER_SEEK_END:
		// TODO: handle
		return XN_STATUS_NOT_IMPLEMENTED;
	}

	// don't allow seeking to frame 0
	nFrameID = XN_MAX(nFrameID, 1);

	xnLogInfo(XN_MASK_FILE, "Seeking file to frameID %u of node %s...", nFrameID, strNodeName);

	if (m_nFileVersion < 4)
	{
		return BCSeekFrame(nFrameID);
	}
	else
	{
		nRetVal = SeekTo(0, strNodeName, nFrameID);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::TellTimestamp(XnUInt64& nTimestamp)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nTimestamp = m_nCurrTimestamp;
	
	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::TellFrame(const XnChar* strNodeName, XnUInt32 &nFrameOffset)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnNodeInfo* pNodeInfo;
	nRetVal = m_nodeInfoMap.Get(strNodeName, pNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	nFrameOffset = pNodeInfo->nCurrFrameID;

	return (XN_STATUS_OK);
}


XnStatus XnFileDevice::GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames)
{
	XnStatus nRetVal = XN_STATUS_OK;
	xn::ProductionNode node;
	nRetVal = m_context.GetProductionNodeByName(strNodeName, node);
	XN_IS_STATUS_OK(nRetVal);
	XnUInt64 nFrames64 = 0;
	nRetVal = node.GetIntProperty(XN_STREAM_PROPERTY_NUMBER_OF_FRAMES, nFrames64);
	XN_IS_STATUS_OK(nRetVal);
	nFrames = (XnUInt32)nFrames64;

	return XN_STATUS_OK;
}

XnBool XnFileDevice::IsEOF()
{
	return m_bEOF;
}

XnStatus XnFileDevice::RegisterToEndOfFileReached(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_eofEvent.Register(handler, pCookie, &hCallback);
}

void XnFileDevice::UnregisterFromEndOfFileReached(XnCallbackHandle hCallback)
{
	m_eofEvent.Unregister(hCallback);
}

XnStatus XnFileDevice::ReadNext()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnBool bWrap;
	nRetVal = ReadTillNextData(&bWrap);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::HandlePackedObject(XnPackedDataType nObjectType)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nObjectType)
	{
	case XN_PACKED_NEW_STREAM:
		nRetVal = ReadNewStream();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_STREAM_REMOVED:
		nRetVal = ReadStreamRemoved();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_INT_PROPERTY:
		nRetVal = ReadIntProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_REAL_PROPERTY:
		nRetVal = ReadRealProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_STRING_PROPERTY:
		nRetVal = ReadStringProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_GENERAL_PROPERTY:
		nRetVal = ReadGeneralProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_STREAM_DATA:
		nRetVal = ReadStreamData();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_END:
		nRetVal = HandleEndOfStream();
		XN_IS_STATUS_OK(nRetVal);
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_FILE_CORRUPTED, XN_MASK_DDK, "Unexpected packed type: %d", nObjectType);
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadNewStream()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];

	// create property set
	XnPropertySet* pPropertySet = NULL;
	nRetVal = XnPropertySetCreate(&pPropertySet);
	XN_IS_STATUS_OK(nRetVal);

	// read from stream
	nRetVal = m_pDataPacker->ReadNewStream(strType, strName, pPropertySet);

	if (nRetVal == XN_STATUS_OK)
	{
		// create it
		nRetVal = HandleNewStream(strType, strName, pPropertySet->pData->begin().Value());
	}

	XnPropertySetDestroy(&pPropertySet);

	return (nRetVal);
}

XnProductionNodeType GetNodeType(const XnChar* strType)
{
	if (strcmp(strType, XN_STREAM_TYPE_DEPTH) == 0)
	{
		return XN_NODE_TYPE_DEPTH;
	}
	else if (strcmp(strType, XN_STREAM_TYPE_IMAGE) == 0)
	{
		return XN_NODE_TYPE_IMAGE;
	}
	else if (strcmp(strType, XN_STREAM_TYPE_IR) == 0)
	{
		return XN_NODE_TYPE_IR;
	}
	else if (strcmp(strType, XN_STREAM_TYPE_AUDIO) == 0)
	{
		return XN_NODE_TYPE_AUDIO;
	}
	else
	{
		XN_ASSERT(FALSE);
		return (XnProductionNodeType)-1;
	}
}

XnStatus XnFileDevice::HandleNewStream(const XnChar *strType, const XnChar *strName, const XnActualPropertiesHash *pInitialValues)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if we need to ignore that (stream was not removed upon Rewind).
	XnNodeInfoMap::Iterator it = m_ignoreNewNodes.end();
	if (m_ignoreNewNodes.Find(strName, it) == XN_STATUS_OK)
	{
		// ignore
		return (XN_STATUS_OK);
	}

	XnProductionNodeType type = GetNodeType(strType);
	if (type == -1)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_FILE, "Invalid node type: %s", strType);
	}

	// find compression type
	XnActualIntProperty* pComp;
	nRetVal = pInitialValues->Get(XN_STREAM_PROPERTY_COMPRESSION, (XnProperty*&)pComp);
	XN_IS_STATUS_OK(nRetVal);

	XnCodecID codecID = XnCodec::GetCodecIDFromCompressionFormat((XnCompressionFormats)pComp->GetValue());
	if (codecID == XN_CODEC_NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_FILE, "Invalid compression type: %llu", pComp->GetValue());
	}

	// notify we have a new node
	nRetVal = m_pNotifications->OnNodeAdded(m_pNotificationsCookie, strName, type, codecID);
	XN_IS_STATUS_OK(nRetVal);

	// we support the mirror capability
	nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strName, XN_CAPABILITY_MIRROR, 1);
	XN_IS_STATUS_OK(nRetVal);

	// we support the extended serialization capability
	nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strName, XN_CAPABILITY_EXTENDED_SERIALIZATION, 1);
	XN_IS_STATUS_OK(nRetVal);

	// now write state
	for (XnActualPropertiesHash::ConstIterator it = pInitialValues->begin(); it != pInitialValues->end(); ++it)
	{
		XnProperty* pProp = it.Value();

		switch (pProp->GetType())
		{
		case XN_PROPERTY_TYPE_INTEGER:
			{
				XnActualIntProperty* pIntProp = (XnActualIntProperty*)pProp;
				nRetVal = HandleIntProperty(strName, pProp->GetName(), pIntProp->GetValue());
			}
			break;
		case XN_PROPERTY_TYPE_REAL:
			{
				XnActualRealProperty* pRealProp = (XnActualRealProperty*)pProp;
				nRetVal = HandleRealProperty(strName, pProp->GetName(), pRealProp->GetValue());
			}
			break;
		case XN_PROPERTY_TYPE_STRING:
			{
				XnActualStringProperty* pStrProp = (XnActualStringProperty*)pProp;
				nRetVal = HandleStringProperty(strName, pProp->GetName(), pStrProp->GetValue());
			}
			break;
		case XN_PROPERTY_TYPE_GENERAL:
			{
				XnActualGeneralProperty* pGenProp = (XnActualGeneralProperty*)pProp;
				nRetVal = HandleGeneralProperty(strName, pProp->GetName(), pGenProp->GetValue());
			}
			break;
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_FILE, "Unknown property type: %d", pProp->GetType());
		}

		XN_IS_STATUS_OK(nRetVal);
	}

	// at this stage, a node should exist with this name
	xn::ProductionNode node;
	nRetVal = m_context.GetProductionNodeByName(strName, node);
	XN_IS_STATUS_OK(nRetVal);

	// S2D & RW
	if (type == XN_NODE_TYPE_DEPTH)
	{
		nRetVal = UpdateS2DTables(xn::DepthGenerator(node));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = UpdateRWData(xn::DepthGenerator(node));
		XN_IS_STATUS_OK(nRetVal);
	}

	// notify end-of-state
	nRetVal = m_pNotifications->OnNodeStateReady(m_pNotificationsCookie, strName);
	XN_IS_STATUS_OK(nRetVal);

	// add it to the map
	XnNodeInfo nodeInfo = {0};
	nRetVal = m_nodeInfoMap.Set(strName, nodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	// create codec
	nRetVal = CreateCodec(node);
	XN_IS_STATUS_OK(nRetVal);

	// check IR compatibility
	nRetVal = CheckIRCompatibility(node);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::CreateCodec(xn::ProductionNode& node)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnNodeInfo* pNodeInfo = NULL;
	if (m_nodeInfoMap.Get(node.GetName(), pNodeInfo) == XN_STATUS_OK)
	{
		XnUInt64 nValue;
		nRetVal = node.GetIntProperty(XN_STREAM_PROPERTY_COMPRESSION, nValue);
		XN_IS_STATUS_OK(nRetVal);

		// create new one
		XnCodecID codecID = XnCodec::GetCodecIDFromCompressionFormat((XnCompressionFormats)nValue);
		if (codecID == XN_CODEC_NULL)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_FILE, "Invalid compression type: %llu", nValue);
		}

		if (pNodeInfo->pXnCodec == NULL || pNodeInfo->pXnCodec->GetCompressionFormat() != nValue)
		{
			// release old codec
			XN_DELETE(pNodeInfo->pXnCodec);
			if (pNodeInfo->codec.IsValid())
			{
				xnRemoveNeededNode(GetSelfNodeHandle(), pNodeInfo->codec);
				pNodeInfo->codec.Release();
			}

			// special case: IR recorded with JPEG. This mode is no longer allowed by OpenNI (JPEG
			// can now only be used for image). We'll have to handle it ourselves.
			if (node.GetInfo().GetDescription().Type == XN_NODE_TYPE_IR &&
				codecID == XN_CODEC_JPEG)
			{
				xn::IRGenerator irGen(node);
				XnMapOutputMode outputMode;
				nRetVal = irGen.GetMapOutputMode(outputMode);
				XN_IS_STATUS_OK(nRetVal);

				XN_VALIDATE_NEW_AND_INIT(pNodeInfo->pXnCodec, XnJpegCodec, TRUE, outputMode.nXRes, outputMode.nYRes);
			}
			else
			{
				// normal case
				nRetVal = m_context.CreateCodec(codecID, node, pNodeInfo->codec);
				XN_IS_STATUS_OK(nRetVal);

				// we need to make the codec a needed node, so that if xnForceShutdown() is called, we will be
				// destroyed *before* it does (as we hold a reference to it).
				nRetVal = xnAddNeededNode(GetSelfNodeHandle(), pNodeInfo->codec);
				XN_IS_STATUS_OK(nRetVal);

				XN_VALIDATE_NEW(pNodeInfo->pXnCodec, XnNiCodec, pNodeInfo->codec);
			}
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::CheckIRCompatibility(xn::ProductionNode& node)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnNodeInfo* pNodeInfo = NULL;
	if (node.GetInfo().GetDescription().Type == XN_NODE_TYPE_IR &&
		m_nodeInfoMap.Get(node.GetName(), pNodeInfo) == XN_STATUS_OK)
	{
		XnUInt64 nValue;
		nRetVal = node.GetIntProperty(XN_STREAM_PROPERTY_OUTPUT_FORMAT, nValue);
		XN_IS_STATUS_OK(nRetVal);

		pNodeInfo->bIRisRGB = (nValue == XN_OUTPUT_FORMAT_RGB24);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadStreamRemoved()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read stream name
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];

	nRetVal = m_pDataPacker->ReadStreamRemoved(strName);
	XN_IS_STATUS_OK(nRetVal);

	// remove it
	nRetVal = HandleStreamRemoved(strName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::HandleStreamRemoved(const XnChar* strName)
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
		nRetVal = m_pInputStream->Tell(&nPositionBefore);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pDataPacker->ReadNextObject(&nType);
		XN_IS_STATUS_OK(nRetVal);

		if (nType == XN_PACKED_STREAM_REMOVED)
		{
			XnChar strTempName[XN_DEVICE_MAX_STRING_LENGTH];
			nRetVal = m_pDataPacker->ReadStreamRemoved(strTempName);
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
			nRetVal = m_pNotifications->OnNodeRemoved(m_pNotificationsCookie, it.Key());
			XN_IS_STATUS_OK(nRetVal);

			XnNodeInfo* pNodeInfo;
			m_nodeInfoMap.Get(it.Key(), pNodeInfo);
			XN_DELETE(pNodeInfo->pXnCodec);
			m_nodeInfoMap.Remove(it.Key());
			m_ignoreNewNodes.Remove(it.Key());
		}

		m_bNodeCollectionChanged = TRUE;
	}

	// in any case, the last object we read wasn't handled yet (end-of-stream or another event), so
	// seek back, so it will be handled.
	nRetVal = m_pInputStream->Seek(nPositionBefore);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadIntProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt64 nValue;

	// read change data
	nRetVal = m_pDataPacker->ReadProperty(strModule, strProp, &nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleIntProperty(strModule, strProp, nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::HandleIntProperty(const XnChar *strModule, const XnChar *strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// ignore some properties
	if (strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 && strcmp(strName, XN_MODULE_PROPERTY_PRIMARY_STREAM) == 0)
	{
		return (XN_STATUS_OK);
	}

	// OpenNI props
	if (strcmp(strName, XN_STREAM_PROPERTY_STATE) == 0)
	{
		nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_IS_GENERATING, nValue);
	}
	else if (strcmp(strName, XN_MODULE_PROPERTY_MIRROR) == 0)
	{
		nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_MIRROR, nValue);
	}
	else if (strcmp(strName, XN_STREAM_PROPERTY_X_RES) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_Y_RES) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_FPS) == 0)
	{
		xn::MapGenerator node;
		nRetVal = m_context.GetProductionNodeByName(strModule, node);
		XN_IS_STATUS_OK(nRetVal);

		XnMapOutputMode mode;
		nRetVal = node.GetMapOutputMode(mode);
		XN_IS_STATUS_OK(nRetVal);

		if (strcmp(strName, XN_STREAM_PROPERTY_X_RES) == 0)
		{
			mode.nXRes = nValue;
		}
		else if (strcmp(strName, XN_STREAM_PROPERTY_Y_RES) == 0)
		{
			mode.nYRes = nValue;
		}
		else if (strcmp(strName, XN_STREAM_PROPERTY_FPS) == 0)
		{
			mode.nFPS = nValue;
		}

		// change supported modes to this one
		nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_SUPPORTED_MAP_OUTPUT_MODES_COUNT, 1);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, strModule, XN_PROP_SUPPORTED_MAP_OUTPUT_MODES, sizeof(XnMapOutputMode), &mode);
		XN_IS_STATUS_OK(nRetVal);

		// and set actual mode
		nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, strModule, XN_PROP_MAP_OUTPUT_MODE, sizeof(mode), &mode);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (strcmp(strName, XN_STREAM_PROPERTY_OUTPUT_FORMAT) == 0)
	{
		switch (nValue)
		{
		case XN_OUTPUT_FORMAT_SHIFT_VALUES:
		case XN_OUTPUT_FORMAT_DEPTH_VALUES:
		case XN_OUTPUT_FORMAT_GRAYSCALE16:
		case XN_OUTPUT_FORMAT_PCM:
			break;
		case XN_OUTPUT_FORMAT_GRAYSCALE8:
			nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_PIXEL_FORMAT, XN_PIXEL_FORMAT_GRAYSCALE_8_BIT);
			break;
		case XN_OUTPUT_FORMAT_RGB24:
			nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_PIXEL_FORMAT, XN_PIXEL_FORMAT_RGB24);
			break;
		case XN_OUTPUT_FORMAT_YUV422:
			nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_PIXEL_FORMAT, XN_PIXEL_FORMAT_YUV422);
			break;
		default:
			XN_ASSERT(FALSE);
			return XN_STATUS_ERROR;
		}

		XN_IS_STATUS_OK(nRetVal);

		// also set property (we need it for IR compatibility)
		nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, strName, nValue);
	}
	else if (strcmp(strName, XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH) == 0)
	{
		nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_DEVICE_MAX_DEPTH, nValue);
	}
	else if (strcmp(strName, XN_STREAM_PROPERTY_SAMPLE_RATE) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS) == 0)
	{
		xn::AudioGenerator node;
		nRetVal = m_context.GetProductionNodeByName(strModule, node);
		XN_IS_STATUS_OK(nRetVal);

		XnWaveOutputMode mode;
		nRetVal = node.GetWaveOutputMode(mode);
		XN_IS_STATUS_OK(nRetVal);

		if (strcmp(strName, XN_STREAM_PROPERTY_SAMPLE_RATE) == 0)
		{
			mode.nSampleRate = nValue;
		}
		else if (strcmp(strName, XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS) == 0)
		{
			mode.nChannels = nValue;
		}

		// change supported modes to this one
		nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES_COUNT, 1);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, strModule, XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES, sizeof(XnWaveOutputMode), &mode);
		XN_IS_STATUS_OK(nRetVal);

		// and set actual mode
		nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, strModule, XN_PROP_WAVE_OUTPUT_MODE, sizeof(mode), &mode);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = m_pNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, strModule, strName, nValue);
	}

	XN_IS_STATUS_OK(nRetVal);

	/********************/
	// Now for some additional logic...
	/********************/

	xn::ProductionNode node;
	nRetVal = m_context.GetProductionNodeByName(strModule, node);
	XN_IS_STATUS_OK(nRetVal);

	// replace codec?
	if (strcmp(strName, XN_STREAM_PROPERTY_COMPRESSION) == 0)
	{
		nRetVal = CreateCodec(node);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (strcmp(strName, XN_STREAM_PROPERTY_OUTPUT_FORMAT) == 0)
	{
		nRetVal = CheckIRCompatibility(node);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 &&
		strcmp(strName, XN_MODULE_PROPERTY_HIGH_RES_TIMESTAMPS) == 0)
	{
		// check timestamps resolution
		m_bHighresTimestamps = (nValue == TRUE);
	}
	else if (strcmp(strName, XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_MAX_SHIFT) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_CONST_SHIFT) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_PARAM_COEFF) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_SHIFT_SCALE) == 0)
	{
		// only after node is ready
		xn::DepthGenerator depth(node);
		XnNodeInfo* pNodeInfo;
		if (m_nodeInfoMap.Get(strModule, pNodeInfo) == XN_STATUS_OK &&
			m_context.GetProductionNodeByName(strModule, depth) == XN_STATUS_OK)
		{
			nRetVal = UpdateS2DTables(depth);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	if (strcmp(strName, XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_X_RES) == 0)
	{
		// only after node is ready
		XnNodeInfo* pNodeInfo;
		if (m_nodeInfoMap.Get(strModule, pNodeInfo) == XN_STATUS_OK)
		{
			// check this is a depth node
			if (node.GetInfo().GetDescription().Type == XN_NODE_TYPE_DEPTH)
			{
				xn::DepthGenerator depth(node);
				nRetVal = UpdateRWData(depth);
				XN_IS_STATUS_OK(nRetVal);
			}
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadRealProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnDouble dValue;

	// read change data
	nRetVal = m_pDataPacker->ReadProperty(strModule, strProp, &dValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleRealProperty(strModule, strProp, dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::HandleRealProperty(const XnChar *strModule, const XnChar *strName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_pNotifications->OnNodeRealPropChanged(m_pNotificationsCookie, strModule, strName, dValue);
	XN_IS_STATUS_OK(nRetVal);

	XnNodeInfo* pNodeInfo;

	if (strcmp(strName, XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE) == 0 ||
		strcmp(strName, XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE) == 0)
	{
		// only after node is ready
		xn::DepthGenerator depth;
		if (m_nodeInfoMap.Get(strModule, pNodeInfo) == XN_STATUS_OK &&
			m_context.GetProductionNodeByName(strModule, depth) == XN_STATUS_OK)
		{
			nRetVal = UpdateS2DTables(depth);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	if (strcmp(strName, XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE) == 0)
	{
		// only after node is ready
		xn::DepthGenerator depth;
		if (m_nodeInfoMap.Get(strModule, pNodeInfo) == XN_STATUS_OK &&
			m_context.GetProductionNodeByName(strModule, depth) == XN_STATUS_OK)
		{
			nRetVal = UpdateRWData(depth);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadStringProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];

	// read change data
	nRetVal = m_pDataPacker->ReadProperty(strModule, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleStringProperty(strModule, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::HandleStringProperty(const XnChar *strModule, const XnChar *strName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_pNotifications->OnNodeStringPropChanged(m_pNotificationsCookie, strModule, strName, strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadGeneralProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnGeneralBuffer gbValue;

	// read change data
	nRetVal = m_pDataPacker->ReadProperty(strModule, strProp, &gbValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleGeneralProperty(strModule, strProp, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::HandleGeneralProperty(const XnChar* strModule, const XnChar* strName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// OpenNI props
	if (strcmp(strName, XN_STREAM_PROPERTY_CROPPING) == 0)
	{
		nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, strModule, XN_PROP_CROPPING, sizeof(XnCropping), gbValue.pData);
	}
	else
	{
		nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, strModule, strName, gbValue.nDataSize, gbValue.pData);
	}

	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadStreamData()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnCompressionFormats nCompression;
	XnUInt32 nCompressedSize;
	nRetVal = m_pDataPacker->ReadStreamDataProps(m_pStreamData, &nCompression, &nCompressedSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleStreamData(m_pStreamData, nCompression, nCompressedSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void TransformRGB24ToGrayscale16(XnUInt8* pBuffer, XnUInt32* pnBufferSize)
{
	XnRGB24Pixel* pInput = (XnRGB24Pixel*)pBuffer;
	XnRGB24Pixel* pBufferEnd = (XnRGB24Pixel*)(pBuffer + *pnBufferSize);
	XnUInt16* pOutput = (XnUInt16*)pBuffer;

	while (pInput < pBufferEnd)
	{
		*pOutput = ((XnUInt16)pInput->nRed) << 2;
		pInput++;
		pOutput++;
	}

	*pnBufferSize = (XnUInt8*)pOutput - pBuffer;
}

XnStatus XnFileDevice::HandleStreamData(XnStreamData* pDataProps, XnCompressionFormats nCompression, XnUInt32 nCompressedSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt32 nPosition;
	nRetVal = m_pInputStream->Tell(&nPosition);
	XN_IS_STATUS_OK(nRetVal);

	XnUIntHash::Iterator it = m_PositionsToIgnore.end();
	if (XN_STATUS_OK == m_PositionsToIgnore.Find(nPosition, it))
	{
		// ignore this one. Just update the frame ID
		XnNodeInfo* pNodeInfo;
		nRetVal = m_nodeInfoMap.Get(pDataProps->StreamName, pNodeInfo);
		XN_IS_STATUS_OK(nRetVal);

		pNodeInfo->nCurrFrameID++;

		// and remove it from list
		nRetVal = m_PositionsToIgnore.Remove(it);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// take the codec
		XnNodeInfo* pNodeInfo;
		nRetVal = m_nodeInfoMap.Get(pDataProps->StreamName, pNodeInfo);
		XN_IS_STATUS_OK(nRetVal);

		// now uncompress data
		nRetVal = m_pDataPacker->ReadStreamData(pDataProps, pNodeInfo->pXnCodec);
		XN_IS_STATUS_OK(nRetVal);

		if (!m_bHighresTimestamps)
		{
			pDataProps->nTimestamp *= 1000;
		}

		if (pNodeInfo->bIRisRGB)
		{
			// covert RGB IR to regular IR
			TransformRGB24ToGrayscale16((XnUInt8*)pDataProps->pData, &pDataProps->nDataSize);
		}

		// update curr timestamp
		if (pDataProps->nTimestamp > m_nCurrTimestamp)
		{
			m_nCurrTimestamp = pDataProps->nTimestamp;
		}

		// update frame ID
		++pNodeInfo->nCurrFrameID;

		// and notify about new data
		nRetVal = m_pNotifications->OnNodeNewData(m_pNotificationsCookie, pDataProps->StreamName, pDataProps->nTimestamp, pNodeInfo->nCurrFrameID, pDataProps->pData, pDataProps->nDataSize);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::HandleEndOfStream()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_bFileHasData)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_FILE, "File does not contain any data...");
	}

	nRetVal = m_eofEvent.Raise();
	XN_IS_STATUS_OK(nRetVal);

	if (m_bRepeat)
	{
		nRetVal = Rewind();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		m_bEOF = TRUE;
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadNextEventFromStream(XnPackedDataType* pnObjectType /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnPackedDataType nObjectType;

	nRetVal = m_pDataPacker->ReadNextObject(&nObjectType);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandlePackedObject(nObjectType);
	XN_IS_STATUS_OK(nRetVal);

	if (pnObjectType != NULL)
	{
		*pnObjectType = nObjectType;
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::ReadTillNextData(XnBool* pbWrapOccurred)
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
		while ((nType != XN_PACKED_STREAM_DATA) && (!m_bEOF))
		{
			nRetVal = ReadNextEventFromStream(&nType);
			XN_IS_STATUS_OK(nRetVal);

			if (nType == XN_PACKED_END)
			{
				*pbWrapOccurred = TRUE;
			}
		}

		if (nType == XN_PACKED_STREAM_DATA)
		{
			m_bFileHasData = TRUE;
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::Rewind()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// go back to start of stream
	nRetVal = m_pInputStream->Seek(XN_DEVICE_FILE_MAGIC_LEN);
	XN_IS_STATUS_OK(nRetVal);

	// read initial state
	XN_PROPERTY_SET_CREATE_ON_STACK(state);
	nRetVal = ReadInitialState(&state);
	XN_IS_STATUS_OK(nRetVal);

	// first handle current streams. remove or reset them
	for (XnNodeInfoMap::Iterator it = m_nodeInfoMap.begin(); it != m_nodeInfoMap.end(); ++it)
	{
		const XnChar* strName = it.Key();

		if (m_bNodeCollectionChanged)
		{
			// we need to destroy all streams, and recreate them later
			nRetVal = m_pNotifications->OnNodeRemoved(m_pNotificationsCookie, strName);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			// just reset frame ID
			it.Value().nCurrFrameID = 0;
			// and mark not to recreate it
			nRetVal = m_ignoreNewNodes.Set(strName, it.Value());
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	// if we need, recreate nodes
	if (m_bNodeCollectionChanged)
	{
		nRetVal = SetInitialState(&state);
		XN_IS_STATUS_OK(nRetVal);
	}

// 	ResetLastTimestampAndFrame();
// 	m_nReferenceTimestamp = 0;
// 	m_nReferenceTime = 0;

	m_bNodeCollectionChanged = FALSE;
	m_nCurrTimestamp = 0;

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::UpdateS2DTables(const xn::DepthGenerator& depth)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt64 nTemp;
	XnDouble dTemp;

	// get config
	XnShiftToDepthConfig config;

	nRetVal = depth.GetIntProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, nTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.nZeroPlaneDistance = nTemp;

	nRetVal = depth.GetRealProperty(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, dTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.fZeroPlanePixelSize = dTemp;

	nRetVal = depth.GetRealProperty(XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE, dTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.fEmitterDCmosDistance = dTemp;

	nRetVal = depth.GetIntProperty(XN_STREAM_PROPERTY_MAX_SHIFT, nTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.nDeviceMaxShiftValue = nTemp;

	config.nDeviceMaxDepthValue = depth.GetDeviceMaxDepth();

	nRetVal = depth.GetIntProperty(XN_STREAM_PROPERTY_CONST_SHIFT, nTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.nConstShift = nTemp;

	nRetVal = depth.GetIntProperty(XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR, nTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.nPixelSizeFactor = nTemp;

	nRetVal = depth.GetIntProperty(XN_STREAM_PROPERTY_PARAM_COEFF, nTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.nParamCoeff = nTemp;

	nRetVal = depth.GetIntProperty(XN_STREAM_PROPERTY_SHIFT_SCALE, nTemp);
	XN_IS_STATUS_OK(nRetVal);
	config.nShiftScale = nTemp;

	config.nDepthMinCutOff = 0;
	config.nDepthMaxCutOff = config.nDeviceMaxDepthValue;

	if (!m_ShiftToDepth.bIsInitialized)
	{
		nRetVal = XnShiftToDepthInit(&m_ShiftToDepth, &config);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = XnShiftToDepthUpdate(&m_ShiftToDepth, &config);
		XN_IS_STATUS_OK(nRetVal);
	}

	// notify
	nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, depth.GetName(), XN_STREAM_PROPERTY_S2D_TABLE, m_ShiftToDepth.nShiftsCount * sizeof(XnDepthPixel), m_ShiftToDepth.pShiftToDepthTable);
	XN_IS_STATUS_OK(nRetVal);
	
	nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, depth.GetName(), XN_STREAM_PROPERTY_D2S_TABLE, m_ShiftToDepth.nDepthsCount * sizeof(XnUInt16), m_ShiftToDepth.pDepthToShiftTable);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::UpdateRWData(const xn::DepthGenerator& depth)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt64 nZPD;
	nRetVal = depth.GetIntProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, nZPD);
	XN_IS_STATUS_OK(nRetVal);

	XnDouble fZPPS;
	nRetVal = depth.GetRealProperty(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, fZPPS);
	XN_IS_STATUS_OK(nRetVal);

	XnFieldOfView FOV;
	FOV.fHFOV = 2*atan(fZPPS*XN_SXGA_X_RES/2/nZPD);
	FOV.fVFOV = 2*atan(fZPPS*XN_VGA_Y_RES*2/2/nZPD);

	// notify
	nRetVal = m_pNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie, 
		depth.GetName(), 
		XN_PROP_FIELD_OF_VIEW, 
		sizeof(FOV),
		&FOV);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnNodeHandle XnFileDevice::GetSelfNodeHandle()
{
	if (m_hSelf == NULL)
	{
		xn::Player player;
		XnStatus nRetVal = m_context.GetProductionNodeByName(m_strName, player);
		XN_ASSERT(nRetVal == XN_STATUS_OK);

		// keep only the handle (we don't want to keep a reference to ourself. This will prevent destruction)
		m_hSelf = player;
	}

	return m_hSelf;
}