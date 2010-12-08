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
#include "XnAudioProcessor.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnAudioProcessor::XnAudioProcessor(XnSensorAudioStream* pStream, XnSensorStreamHelper* pHelper, XnUInt32 nInputPacketSize) :
	XnWholePacketProcessor(pHelper->GetPrivateData(), pStream->GetType(), nInputPacketSize),
	m_pStream(pStream),
	m_pHelper(pHelper),
	m_AudioInDump(XN_DUMP_CLOSED)
{
	xnDumpInit(&m_AudioInDump, XN_DUMP_AUDIO_IN, NULL, "AudioIn.pcm");
}

XnAudioProcessor::~XnAudioProcessor()
{
	xnDumpClose(&m_AudioInDump);
	GetStream()->NumberOfChannelsProperty().OnChangeEvent().Unregister(m_hNumChannelsCallback);
}

XnStatus XnAudioProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnWholePacketProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetStream()->NumberOfChannelsProperty().OnChangeEvent().Register(DeleteChannelChangedCallback, this, &m_hNumChannelsCallback);
	XN_IS_STATUS_OK(nRetVal);

	CalcDeleteChannel();
	
	return (XN_STATUS_OK);
}

void XnAudioProcessor::ProcessWholePacket(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData)
{
	XnInt32 nAvailableBytes = 0;

	xnOSEnterCriticalSection(&m_pDevicePrivateData->hAudioBufferCriticalSection);

	// take write packet
	XnUChar* pWritePacket = m_pDevicePrivateData->pAudioBuffer + (m_pDevicePrivateData->nAudioWriteIndex * m_pDevicePrivateData->nAudioPacketSize);

	if (m_bDeleteChannel)
	{
		XnUInt16* pSamples = (XnUInt16*)pData;
		XnUInt16* pSamplesEnd = (XnUInt16*)(pData + pHeader->nBufSize);
		XnUInt16* pOutput = (XnUInt16*)pWritePacket;

		while (pSamples < pSamplesEnd)
		{
			*pOutput = *pSamples;

			pOutput++;
			// skip a sample
			pSamples += 2;
		}
	}
	else
	{
		// copy data
		xnOSMemCopy(pWritePacket, pData, pHeader->nBufSize);
	}

	// mark timestamp
	m_pDevicePrivateData->pAudioPacketsTimestamps[m_pDevicePrivateData->nAudioWriteIndex] = GetTimeStamp(pHeader->nTimeStamp);

	if (m_nLastPacketID % 10 == 0)
	{
		XnUInt64 nSysTime;
		xnOSGetTimeStamp(&nSysTime);

		xnDumpWriteString(m_pDevicePrivateData->BandwidthDump, "%llu,%s,%d,%d\n",
			nSysTime, "Audio", -1, m_nBytesReceived);

		m_nBytesReceived = 0;
	}

	// move write index forward
	m_pDevicePrivateData->nAudioWriteIndex = (m_pDevicePrivateData->nAudioWriteIndex + 1) % m_pDevicePrivateData->nAudioBufferNumOfPackets;

	// if write index got to read index (end of buffer), move read index forward (and loose a packet)
	if (m_pDevicePrivateData->nAudioWriteIndex == m_pDevicePrivateData->nAudioReadIndex)
	{
		m_pDevicePrivateData->nAudioReadIndex = (m_pDevicePrivateData->nAudioReadIndex + 1) % m_pDevicePrivateData->nAudioBufferNumOfPackets;
	}

	xnOSLeaveCriticalSection(&m_pDevicePrivateData->hAudioBufferCriticalSection);

	xnDumpWriteBuffer(m_AudioInDump, pData, pHeader->nBufSize);

	if (m_pDevicePrivateData->pAudioCallback != NULL)
	{
		m_pDevicePrivateData->pAudioCallback(m_pDevicePrivateData->pAudioCallbackCookie);
	}
}

void XnAudioProcessor::CalcDeleteChannel()
{
	m_bDeleteChannel = (m_pHelper->GetFirmwareVersion() >= XN_SENSOR_FW_VER_5_2 && GetStream()->GetNumberOfChannels() == 1);
}

XnStatus XnAudioProcessor::DeleteChannelChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnAudioProcessor* pThis = (XnAudioProcessor*)pCookie;
	pThis->CalcDeleteChannel();
	return XN_STATUS_OK;
}
