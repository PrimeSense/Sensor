/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorClientStream.h"
#include "XnSensorClient.h"
#include <XnDDK/XnStreamDataInternal.h>

//---------------------------------------------------------------------------
// Client Stream
//---------------------------------------------------------------------------
XnSensorClientStream::XnSensorClientStream(XnSensorClient* pClient, const XnChar* strType, const XnChar* strName) :
	XnStreamReaderStream(strType, strName),
	m_pClient(pClient),
	m_hSharedMemory(NULL),
	m_pSharedMemory(NULL)
{}

XnSensorClientStream::~XnSensorClientStream()
{
	XnSensorClientStream::Free();
}

XnStatus XnSensorClientStream::OpenSharedMemory()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_hSharedMemory != NULL)
	{
		xnOSCloseSharedMemory(m_hSharedMemory);
		m_hSharedMemory = NULL;
		m_pSharedMemory = NULL;
	}

	XnChar strSharedMemoryName[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = GetProperty(XN_STREAM_PROPERTY_SHARED_BUFFER_NAME, strSharedMemoryName);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSOpenSharedMemoryEx(strSharedMemoryName, XN_OS_FILE_READ, m_pClient->IsServerFromOtherUserAllowed(), &m_hSharedMemory);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSSharedMemoryGetAddress(m_hSharedMemory, (void**)&m_pSharedMemory);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClientStream::Open()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_pClient->m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_OPEN_STREAM, GetName(), (XnUInt32)strlen(GetName()) + 1);
	XN_IS_STATUS_OK(nRetVal);

	// wait for reply
	nRetVal = m_pClient->WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnStreamReaderStream::Open();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClientStream::Close()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read data from server
	nRetVal = m_pClient->m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_CLOSE_STREAM, GetName(), (XnUInt32)strlen(GetName()) + 1);
	XN_IS_STATUS_OK(nRetVal);

	// wait for reply
	nRetVal = m_pClient->WaitForReply(XN_SENSOR_SERVER_MESSAGE_GENERAL_OP_RESPOND);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnStreamReaderStream::Close();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClientStream::Free()
{
	if (m_hSharedMemory != NULL)
	{
		xnOSCloseSharedMemory(m_hSharedMemory);
		m_hSharedMemory = NULL;
		m_pSharedMemory = NULL;
	}
	
	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Client Frame Stream
//---------------------------------------------------------------------------
XnSensorClientFrameStream::XnSensorClientFrameStream(XnSensorClient* pClient, const XnChar* strType, const XnChar* strName) :
	XnSensorClientStream(pClient, strType, strName),
	m_nFrameID(0)
{}

XnSensorClientFrameStream::~XnSensorClientFrameStream()
{
}

XnStatus XnSensorClientFrameStream::ReadImpl(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read data from server
	nRetVal = m_pClient->m_pOutgoingPacker->WriteCustomData(XN_SENSOR_SERVER_MESSAGE_READ_STREAM, pStreamOutput->StreamName, (XnUInt32)strlen(pStreamOutput->StreamName) + 1);
	XN_IS_STATUS_OK(nRetVal);

	// wait for reply
	nRetVal = m_pClient->WaitForReply(XN_SENSOR_SERVER_MESSAGE_READ_STREAM);
	XN_IS_STATUS_OK(nRetVal);

	++m_nFrameID;

	XnSensorServerReadReply* pReply = (XnSensorServerReadReply*)m_pClient->m_LastReply.pData;
	pStreamOutput->nFrameID = m_nFrameID;
	pStreamOutput->nTimestamp = pReply->nTimestamp;
	pStreamOutput->pData = (m_pSharedMemory + pReply->nOffset);
	pStreamOutput->nDataSize = pReply->nDataSize;

	return (XN_STATUS_OK);
}

XnStatus XnSensorClientFrameStream::CreateStreamData(XnStreamData** ppStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnStreamData* pStreamData;

	// we create a StreamData object with no buffer allocated. The buffer will just be 
	// a pointer to the triple buffer
	nRetVal = XnStreamDataCreateNoBuffer(&pStreamData, GetName());
	XN_IS_STATUS_OK(nRetVal);

	pStreamData->pData = m_pSharedMemory;

	*ppStreamData = pStreamData;
	
	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Client Depth Stream
//---------------------------------------------------------------------------
XnSensorClientDepthStream::XnSensorClientDepthStream(XnSensorClient* pClient, const XnChar* strType, const XnChar* strName) :
	XnSensorClientFrameStream(pClient, strType, strName)
{}

XnSensorClientDepthStream::~XnSensorClientDepthStream()
{}


XnStatus XnSensorClientDepthStream::ReadImpl(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorClientFrameStream::ReadImpl(pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	m_pLastFrameShiftsMapOffset = (XnUInt16*)(((XnUInt8*)pStreamOutput->pData) + pStreamOutput->nDataSize);

	return (XN_STATUS_OK);
}

XnStatus XnSensorClientDepthStream::GetProperty(const XnChar* strName, const XnGeneralBuffer& gbValue) const
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (strcmp(strName, XN_STREAM_PROPERTY_SHIFTS_MAP) == 0)
	{
		// shifts-map is at the end of the depth map
		if (gbValue.nDataSize != sizeof(m_pLastFrameShiftsMapOffset))	
		{
			return XN_STATUS_DEVICE_PROPERTY_SIZE_DONT_MATCH;
		}

		const XnUInt16** ppShiftsMap = (const XnUInt16**)gbValue.pData;
		*ppShiftsMap = m_pLastFrameShiftsMapOffset;
	}
	else
	{
		nRetVal = XnSensorClientFrameStream::GetProperty(strName, gbValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Client Audio Stream
//---------------------------------------------------------------------------
XnSensorClientAudioStream::XnSensorClientAudioStream(XnSensorClient* pClient, const XnChar* strType, const XnChar* strName) :
	XnSensorClientStream(pClient, strType, strName),
	m_pHeader(NULL),
	m_pBuffer(NULL),
	m_nLastReadIndex(0),
	m_hLock(NULL),
	m_nFrameID(0),
	m_pTimestamps(NULL)
{}

XnSensorClientAudioStream::~XnSensorClientAudioStream()
{}

XnStatus XnSensorClientAudioStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorClientStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hLock);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClientAudioStream::Free()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_hLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hLock);
		m_hLock = NULL;
	}

	nRetVal = XnSensorClientStream::Free();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorClientAudioStream::OpenSharedMemory()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorClientStream::OpenSharedMemory();
	XN_IS_STATUS_OK(nRetVal);

	m_pHeader = (XnAudioSharedBuffer*)m_pSharedMemory;
	m_pBuffer = m_pSharedMemory + m_pHeader->nBufferOffset;
	m_pTimestamps = (XnUInt64*)(m_pSharedMemory + m_pHeader->nTimestampsListOffset);
	
	return (XN_STATUS_OK);
}

void XnSensorClientAudioStream::NewDataAvailable(XnUInt64 /*nTimestamp*/, XnUInt32 /*nFrameID*/)
{
	// if a read is in progress, wait for it to complete
	XnAutoCSLocker locker(m_hLock);

	// check if we still have new data
	if (m_pHeader->nWritePacketIndex != m_nLastReadIndex)
	{
		XnSensorClientStream::NewDataAvailable(m_pTimestamps[m_nLastReadIndex], 0);
	}
}

XnStatus XnSensorClientAudioStream::ReadImpl(XnStreamData* pStreamOutput)
{
	pStreamOutput->nDataSize = 0;

	// take last write index (note: this is taken from shared memory)
	XnAutoCSLocker locker(m_hLock);

	XnUInt32 nWriteIndex = m_pHeader->nWritePacketIndex;

	// check how many buffers we have
	XnInt32 nAvailbalePackets = nWriteIndex - m_nLastReadIndex;
	if (nAvailbalePackets < 0)
		nAvailbalePackets += m_pHeader->nPacketCount;

	// make sure we have enough space
	while (GetRequiredDataSize() < (XnUInt32)nAvailbalePackets * m_pHeader->nPacketSize)
	{
		m_nLastReadIndex = (m_nLastReadIndex + 1) % m_pHeader->nPacketCount;
		nAvailbalePackets--;
	}

	// take timestamp
	pStreamOutput->nTimestamp = m_pTimestamps[m_nLastReadIndex];

	// now copy data from last read position to this one
	XnUChar* pAudioBuf = (XnUChar*)pStreamOutput->pData;

	// copy
	while (m_nLastReadIndex != nWriteIndex)
	{
		XN_ASSERT(pStreamOutput->nDataSize < GetRequiredDataSize());

		XnUChar* pPacketData = m_pBuffer + (m_nLastReadIndex * m_pHeader->nPacketSize);
		xnOSMemCopy(pAudioBuf, pPacketData, m_pHeader->nPacketSize);
		pAudioBuf += m_pHeader->nPacketSize;
		pStreamOutput->nDataSize += m_pHeader->nPacketSize;

		m_nLastReadIndex = (m_nLastReadIndex + 1) % m_pHeader->nPacketCount;
	}

	m_nFrameID++;
	pStreamOutput->nFrameID = m_nFrameID;

	return (XN_STATUS_OK);
}

