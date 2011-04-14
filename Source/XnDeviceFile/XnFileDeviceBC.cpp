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




#include "XnFileDevice.h"
#include "XnDeviceFileReaderBC.h"
#include <XnFormats/XnStreamCompression.h>
#include <XnDDK/XnBackwardsCompatibility36.h>
#include <XnDDK/XnPropertySetInternal.h>

//---------------------------------------------------------------------------
// Backwards Compatibility
//---------------------------------------------------------------------------

extern XnStatus XnIOAdjustStreamPropertiesV3(const XnStreamProperties* pStreamPropertiesV3, XnStreamProperties* pStreamProperties);
extern XnStatus XnIOAdjustStreamPropertiesV2 (const XnStreamPropertiesV2* pStreamPropertiesV2, XnStreamProperties* pStreamProperties);
extern XnStatus XnIOAdjustStreamPropertiesV1 (const XnStreamPropertiesV1* pStreamPropertiesV1, XnStreamProperties* pStreamProperties);
extern XnStatus XnIOAdjustStreamFramePropertiesV3(const XnStreamFrameProperties* pStreamFramePropertiesV3, XnStreamFrameProperties* pStreamFrameProperties);
extern XnStatus XnIOAdjustStreamFramePropertiesV2 (const XnStreamFramePropertiesV2* pStreamFramePropertiesV2, XnStreamFrameProperties* pStreamFrameProperties);
extern XnStatus XnIOAdjustStreamFramePropertiesV1 (const XnStreamFramePropertiesV1* pStreamFramePropertiesV1, XnStreamFrameProperties* pStreamFrameProperties);
extern XnStatus XnIOAdjustPackedStreamPropertiesV2 (const XnPackedStreamPropertiesV2* pPackedStreamPropertiesV2, XnPackedStreamProperties* pPackedStreamProperties);
extern XnStatus XnIOAdjustPackedStreamFrameHeaderV2 (const XnPackedStreamFrameHeaderV2* pPackedStreamFrameHeaderV2, XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeader);
extern XnStatus XnIOAdjustPackedStreamPropertiesV1 (const XnPackedStreamPropertiesV1* pPackedStreamPropertiesV1, XnPackedStreamProperties* pPackedStreamProperties);
extern XnStatus XnIOAdjustPackedStreamFrameHeaderV1 (const XnPackedStreamFrameHeaderV1* pPackedStreamFrameHeaderV1, XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeader);
extern XnStatus XnIOAdjustPackedStreamPropertiesV3 (const XnPackedStreamProperties* pPackedStreamPropertiesV3, XnPackedStreamProperties* pPackedStreamProperties);
extern XnStatus XnIOAdjustPackedStreamFrameHeaderV3 (const XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeaderV3, XnPackedStreamFrameHeaderV3* pPackedStreamFrameHeader);
extern XnStatus BCSetDepthProperties(XnPropertySet* pSet, XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties);
extern XnStatus BCSetImageProperties(XnPropertySet* pSet, XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties);
extern XnStatus BCSetAudioProperties(XnPropertySet* pSet, XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties);
extern XnStatus ConvertStreamPropertiesToPropertySet(XnStreamPropertiesV3* pStreamProperties, XnPackedStreamProperties* pPackedStreamProperties, XnPropertySet* pSet);
extern XnStatus XnDeviceFileAdjustFileFrameHeaderV1(const XnDeviceFileFrameHeaderV1* pFileFrameHeaderV1, XnDeviceFileFrameHeaderV3* pFileFrameHeader);
extern XnStatus XnDeviceFileAdjustFileFrameHeaderV2(const XnDeviceFileFrameHeaderV2* pFileFrameHeaderV2, XnDeviceFileFrameHeaderV3* pFileFrameHeader);
extern XnStatus XnDeviceFileAdjustFileFrameHeaderV3(const XnDeviceFileFrameHeaderV3* pFileFrameHeaderV3, XnDeviceFileFrameHeaderV3* pFileFrameHeader);

XnStatus XnFileDevice::BCInit()
{
	XN_VALIDATE_CALLOC(m_pBCData, XnFileBCData, 1);
	return XN_STATUS_OK;
}

XnStatus XnFileDevice::BCCalculatePackedBufferSize()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnStreamPropertiesV3* pStreamProperties = &m_pBCData->StreamProperties;
	XnPackedStreamProperties* pPackedStreamProperties = &m_pBCData->PackedStreamProperties;

	XnUInt32 nBufferSize = 0;

	if (pStreamProperties->DepthFormat != XN_DEPTH_FORMAT_DISABLED)
	{
		if ((pStreamProperties->DepthFormat == XN_DEPTH_FORMAT_RAW12) || 
			(pStreamProperties->DepthFormat == XN_DEPTH_FORMAT_RAW10) ||
			(pStreamProperties->DepthFormat == XN_DEPTH_FORMAT_SHIFTS))
		{
			switch (pPackedStreamProperties->StreamDepthCompressionFormat)
			{
			case XN_COMPRESSED_DEPTH_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_DEPTH_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nDepthBufferSize;
				break;
			case XN_COMPRESSED_DEPTH_FORMAT_16Z:
			case XN_COMPRESSED_DEPTH_FORMAT_16ZEMBTABLE:
				nBufferSize += (XnUInt32)(pStreamProperties->nDepthBufferSize * XN_STREAM_COMPRESSION_DEPTH16Z_WORSE_RATIO);
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_DEPTH_COMPRESSION_FORMAT);
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_DEPTH_FORMAT);
		}
	}

	if (pStreamProperties->ImageFormat != XN_IMAGE_FORMAT_DISABLED)
	{
		if ((pStreamProperties->ImageFormat == XN_IMAGE_FORMAT_RGB24) || (pStreamProperties->ImageFormat == XN_IMAGE_FORMAT_GRAYSCALE8))
		{
			switch (pPackedStreamProperties->StreamImageCompressionFormat)
			{
			case XN_COMPRESSED_IMAGE_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nImageBufferSize;
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_8Z:
				nBufferSize += (XnUInt32)(pStreamProperties->nImageBufferSize * XN_STREAM_COMPRESSION_IMAGE8Z_WORSE_RATIO);
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_JPEG:
				nBufferSize += (XnUInt32)(pStreamProperties->nImageBufferSize * XN_STREAM_COMPRESSION_IMAGEJ_WORSE_RATIO);
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_IMAGE_COMPRESSION_FORMAT);
			}
		}
		else if (pStreamProperties->ImageFormat == XN_IMAGE_FORMAT_YUV422)
		{
			switch (pPackedStreamProperties->StreamImageCompressionFormat)
			{
			case XN_COMPRESSED_IMAGE_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_IMAGE_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nImageBufferSize;
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_IMAGE_COMPRESSION_FORMAT);
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT);
		}
	}

	if (pStreamProperties->MiscFormat != XN_MISC_FORMAT_DISABLED)
	{
		if (pStreamProperties->MiscFormat == XN_MISC_FORMAT_CONFIDENCE_MAP)
		{
			switch (pPackedStreamProperties->StreamMiscCompressionFormat)
			{
			case XN_COMPRESSED_MISC_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_MISC_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nMiscBufferSize;
				break;
			case XN_COMPRESSED_MISC_FORMAT_CONF4:
				nBufferSize += (XnUInt32)(pStreamProperties->nMiscBufferSize * XN_STREAM_COMPRESSION_CONF4_WORSE_RATIO);
				break;
			case XN_COMPRESSED_MISC_FORMAT_CONF4LZ:
				nBufferSize += (XnUInt32)(pStreamProperties->nMiscBufferSize * XN_STREAM_COMPRESSION_CONF4_WORSE_RATIO);
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_MISC_COMPRESSION_FORMAT);		
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_MISC_FORMAT);
		}
	}

	if (pStreamProperties->AudioFormat != XN_AUDIO_FORMAT_DISABLED)
	{
		if (pStreamProperties->AudioFormat == XN_AUDIO_FORMAT_PCM)
		{
			switch (pPackedStreamProperties->StreamAudioCompressionFormat)
			{
			case XN_COMPRESSED_AUDIO_FORMAT_SKIP:
				break;
			case XN_COMPRESSED_AUDIO_FORMAT_UNCOMPRESSED:
				nBufferSize += pStreamProperties->nAudioBufferSize;
				break;
			default:
				return (XN_STATUS_IO_INVALID_STREAM_AUDIO_COMPRESSION_FORMAT);
			}
		}
		else
		{
			return (XN_STATUS_IO_INVALID_STREAM_AUDIO_FORMAT);
		}
	}

	nBufferSize += sizeof(XnPackedStreamFrameHeaderV3);

	return nBufferSize;

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::BCReadInitialState(XnPropertySet* pSet)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	XnDeviceFileHeader DeviceFileHeader;
	XN_STREAM_FLAGS_TYPE nStreamFlags = 0;

	m_pBCData->nFramePos = 1;

	xnOSFreeAligned(m_pBCData->pPackedStreamBuffer);
	m_pBCData->pPackedStreamBuffer = NULL;
	m_pBCData->nPackedStreamBufferSize = 0;

	// read StreamProperties
	if (m_nFileVersion == 3)
	{
		// Current Version
		nRetVal = m_pInputStream->ReadData((XnUChar*)&DeviceFileHeader.nMajorVersion, sizeof(XnUInt16));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pInputStream->ReadData((XnUChar*)&DeviceFileHeader.nMinorVersion, sizeof(XnUInt16));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pInputStream->ReadData((XnUChar*)&DeviceFileHeader.StreamProperties, sizeof(XnStreamPropertiesV3));
		XN_IS_STATUS_OK(nRetVal);

		DeviceFileHeader.nMajorVersion = XN_PREPARE_VAR16_IN_BUFFER(DeviceFileHeader.nMajorVersion);
		DeviceFileHeader.nMinorVersion = XN_PREPARE_VAR16_IN_BUFFER(DeviceFileHeader.nMinorVersion);

		nRetVal = XnIOAdjustStreamPropertiesV3(&DeviceFileHeader.StreamProperties, &DeviceFileHeader.StreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (m_nFileVersion == 2)
	{
		// Version 2
		DeviceFileHeader.nMajorVersion = 0;
		DeviceFileHeader.nMinorVersion = 0;
		XnStreamPropertiesV2 StreamPropertiesV2;

		nRetVal = m_pInputStream->ReadData((XnUChar*)&StreamPropertiesV2, sizeof(XnStreamPropertiesV2));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnIOAdjustStreamPropertiesV2(&StreamPropertiesV2, &DeviceFileHeader.StreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (m_nFileVersion == 1)
	{
		// Version 1
		DeviceFileHeader.nMajorVersion = 0;
		DeviceFileHeader.nMinorVersion = 0;
		XnStreamPropertiesV1 StreamPropertiesV1;

		nRetVal = m_pInputStream->ReadData((XnUChar*)&StreamPropertiesV1, sizeof(XnStreamPropertiesV1));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnIOAdjustStreamPropertiesV1(&StreamPropertiesV1, &DeviceFileHeader.StreamProperties);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// Bad Magic
		return XN_STATUS_IO_INVALID_STREAM_HEADER;		
	}

	// read packed stream properties
	switch (m_nFileVersion)
	{
	case 3:
		{
			nRetVal = m_pInputStream->ReadData((XnUChar*)&DeviceFileHeader.PackedStreamProperties, sizeof(XnPackedStreamProperties));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnIOAdjustPackedStreamPropertiesV3(&DeviceFileHeader.PackedStreamProperties, &DeviceFileHeader.PackedStreamProperties);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 2:
		{
			XnPackedStreamPropertiesV2 PackedStreamPropertiesV2;

			nRetVal = m_pInputStream->ReadData((XnUChar*)&PackedStreamPropertiesV2, sizeof(XnPackedStreamPropertiesV2));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnIOAdjustPackedStreamPropertiesV2(&PackedStreamPropertiesV2, &DeviceFileHeader.PackedStreamProperties);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 1:
		{
			XnPackedStreamPropertiesV1 PackedStreamPropertiesV1;

			nRetVal = m_pInputStream->ReadData((XnUChar*)&PackedStreamPropertiesV1, sizeof(XnPackedStreamPropertiesV1));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnIOAdjustPackedStreamPropertiesV1(&PackedStreamPropertiesV1, &DeviceFileHeader.PackedStreamProperties);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	default:
		return XN_STATUS_IO_INVALID_STREAM_HEADER;
	}

	// Save the stream properties into the private data (but keep the original stream flags)
	nStreamFlags = m_pBCData->StreamProperties.nStreamFlags;
	xnOSMemCopy(&m_pBCData->StreamProperties, &DeviceFileHeader.StreamProperties, sizeof(XnStreamProperties));
	m_pBCData->StreamProperties.nStreamFlags = nStreamFlags;

	if (m_pBCData->StreamProperties.Shift2DepthData.bShift2DepthData)
	{
		m_pBCData->StreamProperties.Shift2DepthData.nMaxDepthValue = 10000;
		m_pBCData->StreamProperties.nDepthMaxValue = 10000;
	}

	// Save the packed stream properties into the private data
	xnOSMemCopy(&m_pBCData->PackedStreamProperties, &DeviceFileHeader.PackedStreamProperties, sizeof(XnPackedStreamProperties));

	XnUInt32 nBufferSize = BCCalculatePackedBufferSize();
	if (nBufferSize != m_pBCData->nPackedStreamBufferSize)
	{
		xnOSFreeAligned(m_pBCData->pPackedStreamBuffer);
		XN_VALIDATE_ALIGNED_CALLOC(m_pBCData->pPackedStreamBuffer, XnUChar, nBufferSize, XN_DEFAULT_MEM_ALIGN);
		m_pBCData->nPackedStreamBufferSize = nBufferSize;
	}

	nRetVal = ConvertStreamPropertiesToPropertySet(&m_pBCData->StreamProperties, &m_pBCData->PackedStreamProperties, pSet);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::BCSeek(XnUInt64 nTimestamp)
{
	return (XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED);
}

XnStatus XnFileDevice::BCSeekFrame(XnUInt32 nFrameID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceFileFrameHeaderV3 FileFrameHeader;
	XnUInt32 nReadBytes = 0;
	XnUInt32 nShouldRead = 0;
	XnInt32 nExpectedFrameID = 1;

	// go back to start of file
	nRetVal = Rewind();
	XN_IS_STATUS_OK(nRetVal);

	// Update the frame position to the new position (treat 0 as 1)
	m_pBCData->nFramePos = XN_MAX(nFrameID, 1);

	// Make sure we aren't trying to reach a frame that's beyond the number of frames
	if (m_pBCData->nFramePos > m_pBCData->StreamProperties.nNumOfFrames)
	{
		// Set the frame position to the last frame
		m_pBCData->nFramePos = m_pBCData->StreamProperties.nNumOfFrames;
	}

	// Set the file position to the first frame data (right after the file header)
	XnUInt32 nOffset = 0;
	switch (m_nFileVersion)
	{
	case 3:
		nOffset = sizeof(XnDeviceFileHeader);
		break;
	case 2:
		nOffset = sizeof(XnDeviceFileHeaderV2);
		break;
	case 1:
		nOffset = sizeof(XnDeviceFileHeaderV1);
		break;
	default:
		return (XN_STATUS_IO_INVALID_STREAM_HEADER);
	}

	nRetVal = m_pInputStream->Seek(nOffset);
	XN_IS_STATUS_OK(nRetVal);

	// Keep reading frames until we reach the wanted frame
	XnUInt32 nCurrFilePos = 1;
	while (nCurrFilePos < m_pBCData->nFramePos)
	{
		// Read the frame header
		switch (m_nFileVersion)
		{
		case 3:
			{
				nShouldRead = nReadBytes = sizeof(XnDeviceFileFrameHeaderV3);
				nRetVal = m_pInputStream->ReadData((XnUChar*)&FileFrameHeader, nReadBytes);
				XN_IS_STATUS_OK(nRetVal);
				nExpectedFrameID = nCurrFilePos;
			}
			break;
		case 2:
			{
				XnDeviceFileFrameHeaderV2 FileFrameHeaderV2;
				nShouldRead = nReadBytes = sizeof(XnDeviceFileFrameHeaderV2);
				nRetVal = m_pInputStream->ReadData((XnUChar*)&FileFrameHeaderV2, nReadBytes);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnDeviceFileAdjustFileFrameHeaderV2(&FileFrameHeaderV2, &FileFrameHeader);
				XN_IS_STATUS_OK(nRetVal);
				nExpectedFrameID = nCurrFilePos - 1;
			}
			break;
		case 1:
			{
				XnDeviceFileFrameHeaderV1 FileFrameHeaderV1;
				nShouldRead = nReadBytes = sizeof(XnDeviceFileFrameHeaderV1);
				nRetVal = m_pInputStream->ReadData((XnUChar*)&FileFrameHeaderV1, nReadBytes);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnDeviceFileAdjustFileFrameHeaderV1(&FileFrameHeaderV1, &FileFrameHeader);
				XN_IS_STATUS_OK(nRetVal);
				nExpectedFrameID = nCurrFilePos - 1;
			}
			break;
		default:
			return XN_STATUS_IO_INVALID_STREAM_HEADER;
		}

		// Make sure we got the right header size
		if (nReadBytes != nShouldRead)
		{
			return (XN_STATUS_IO_INVALID_STREAM_FRAME_HEADER);
		}

		// Skip the frame data
		XnUInt32 nPosition;
		nRetVal = m_pInputStream->Tell(&nPosition);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pInputStream->Seek(FileFrameHeader.nPackedStreamSize + nPosition);
		XN_IS_STATUS_OK(nRetVal);

		// increment streams frame ID
		for (XnNodeInfoMap::Iterator it = m_nodeInfoMap.begin(); it != m_nodeInfoMap.end(); ++it)
		{
			it.Value().nCurrFrameID++;
		}

		// Make sure frame ids are sequential
		if (FileFrameHeader.FrameProperties.nDepthFrameID != nExpectedFrameID)
		{
			return (XN_STATUS_IO_STREAM_NOT_SEQUENTIAL);
		}

		// Update the current file frame position
		nCurrFilePos++;
	}

	// now read last frame (the one we wanted)
	XnBool bWrapOccured;
	nRetVal = BCReadFrame(&bWrapOccured);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::BCReadFrame(XnBool* pbWrapOccured)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	XnDeviceFileFrameHeaderV3 FileFrameHeader;
	XnUInt64 nCurrTime = 0;
	XnUInt64 nDiffTime = 0;
	XnUInt64 nFramesDiffTime = 0;
	XnUInt32 nSleepTime = 0;

	*pbWrapOccured = FALSE;

	// If we've reached the last frame, seek back to the first one
	if (m_pBCData->nFramePos > m_pBCData->StreamProperties.nNumOfFrames)
	{
		nRetVal = HandleEndOfStream();
		XN_IS_STATUS_OK(nRetVal);
		*pbWrapOccured = TRUE;
		if (m_bEOF)
		{
			return XN_STATUS_OK;
		}
	}

	m_bFileHasData = TRUE;

	// Note: Since this is an internal function, the pointers are assumed to be checked by the caller
	// Read the frame header
	switch (m_nFileVersion)
	{
	case 3:
		{
			nRetVal = m_pInputStream->ReadData((XnUChar*)&FileFrameHeader, sizeof(XnDeviceFileFrameHeaderV3));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnDeviceFileAdjustFileFrameHeaderV3(&FileFrameHeader, &FileFrameHeader);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 2:
		{
			XnDeviceFileFrameHeaderV2 FileFrameHeaderV2;

			nRetVal = m_pInputStream->ReadData((XnUChar*)&FileFrameHeaderV2, sizeof(XnDeviceFileFrameHeaderV2));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnDeviceFileAdjustFileFrameHeaderV2(&FileFrameHeaderV2, &FileFrameHeader);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	case 1:
		{
			XnDeviceFileFrameHeaderV1 FileFrameHeaderV1;

			nRetVal = m_pInputStream->ReadData((XnUChar*)&FileFrameHeaderV1, sizeof(XnDeviceFileFrameHeaderV1));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = XnDeviceFileAdjustFileFrameHeaderV1(&FileFrameHeaderV1, &FileFrameHeader);
			XN_IS_STATUS_OK(nRetVal);
		}
		break;
	default:
		return XN_STATUS_IO_INVALID_STREAM_HEADER;
	}

	FileFrameHeader.FrameProperties.nDepthFrameID = m_pBCData->nFramePos;
	FileFrameHeader.FrameProperties.nImageFrameID = m_pBCData->nFramePos;

	// Make sure we aren't going to overflow the packed stream buffer
	if (FileFrameHeader.nPackedStreamSize > m_pBCData->nPackedStreamBufferSize)
	{
		return (XN_STATUS_INPUT_BUFFER_OVERFLOW);
	}

	// Read the frame packed stream data into the packed stream buffer
	nRetVal = m_pInputStream->ReadData(m_pBCData->pPackedStreamBuffer, FileFrameHeader.nPackedStreamSize);
	XN_IS_STATUS_OK(nRetVal);

	// read the frame header
	XnPackedStreamFrameHeaderV3 PackedStreamHeader;
	XnUChar* pPackedBuffer = m_pBCData->pPackedStreamBuffer;

	switch (m_nFileVersion)
	{
	case 0:
	case 3:
		{
			xnOSMemCopy(&PackedStreamHeader, pPackedBuffer, sizeof(XnPackedStreamFrameHeaderV3));
			pPackedBuffer += sizeof(XnPackedStreamFrameHeaderV3);
			nRetVal = XnIOAdjustPackedStreamFrameHeaderV3(&PackedStreamHeader, &PackedStreamHeader);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case 2:
		{
			XnPackedStreamFrameHeaderV2* pPackedStreamHeaderV2 = (XnPackedStreamFrameHeaderV2*)pPackedBuffer;
			pPackedBuffer += sizeof(XnPackedStreamFrameHeaderV2);
			nRetVal = XnIOAdjustPackedStreamFrameHeaderV2(pPackedStreamHeaderV2, &PackedStreamHeader);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case 1:
		{
			XnPackedStreamFrameHeaderV1* pPackedStreamHeaderV1 = (XnPackedStreamFrameHeaderV1*)pPackedBuffer;
			pPackedBuffer += sizeof(XnPackedStreamFrameHeaderV1);
			nRetVal = XnIOAdjustPackedStreamFrameHeaderV1(pPackedStreamHeaderV1, &PackedStreamHeader);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	default:
		return XN_STATUS_IO_INVALID_STREAM_HEADER;
	}

	// Depth
	XnNodeInfo* pNodeInfo;
	if (XN_STATUS_OK == m_nodeInfoMap.Get(XN_STREAM_NAME_DEPTH, pNodeInfo))
	{
		m_pStreamData->nDataSize = m_pBCData->StreamProperties.nDepthBufferSize;

		nRetVal = pNodeInfo->pXnCodec->Decompress(pPackedBuffer, PackedStreamHeader.nCompDepthBufferSize, (XnUChar*)m_pStreamData->pData, &m_pStreamData->nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pNotifications->OnNodeNewData(m_pNotificationsCookie, XN_STREAM_NAME_DEPTH, FileFrameHeader.FrameProperties.nDepthTimeStamp * 1000, m_pBCData->nFramePos, m_pStreamData->pData, m_pBCData->StreamProperties.nDepthBufferSize);
		XN_IS_STATUS_OK(nRetVal);

		pNodeInfo->nCurrFrameID++;

		pPackedBuffer += PackedStreamHeader.nCompDepthBufferSize;
	}

	// Image
	if (XN_STATUS_OK == m_nodeInfoMap.Get(XN_STREAM_NAME_IMAGE, pNodeInfo))
	{
		m_pStreamData->nDataSize = m_pBCData->StreamProperties.nImageBufferSize;

		nRetVal = pNodeInfo->pXnCodec->Decompress(pPackedBuffer, PackedStreamHeader.nCompImageBufferSize, (XnUChar*)m_pStreamData->pData, &m_pStreamData->nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pNotifications->OnNodeNewData(m_pNotificationsCookie, XN_STREAM_NAME_IMAGE, FileFrameHeader.FrameProperties.nImageTimeStamp * 1000, m_pBCData->nFramePos, m_pStreamData->pData, m_pBCData->StreamProperties.nImageBufferSize);
		XN_IS_STATUS_OK(nRetVal);

		pNodeInfo->nCurrFrameID++;

		pPackedBuffer += PackedStreamHeader.nCompImageBufferSize;
	}

	// we do not support MISC
	pPackedBuffer += PackedStreamHeader.nCompMiscBufferSize;

	// Audio
	if (XN_STATUS_OK == m_nodeInfoMap.Get(XN_STREAM_NAME_AUDIO, pNodeInfo))
	{
		m_pStreamData->nDataSize = m_pBCData->StreamProperties.nAudioBufferSize;

		nRetVal = pNodeInfo->pXnCodec->Decompress(pPackedBuffer, PackedStreamHeader.nCompAudioBufferSize, (XnUChar*)m_pStreamData->pData, &m_pStreamData->nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_pNotifications->OnNodeNewData(m_pNotificationsCookie, XN_STREAM_NAME_AUDIO, FileFrameHeader.FrameProperties.nAudioTimeStamp * 1000, m_pBCData->nFramePos, m_pStreamData->pData, m_pStreamData->nDataSize);
		XN_IS_STATUS_OK(nRetVal);

		pNodeInfo->nCurrFrameID++;

		pPackedBuffer += PackedStreamHeader.nCompAudioBufferSize;
	}

	// Increase the file frame position
	m_pBCData->nFramePos++;

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnFileDevice::BCDestroy()
{
	if (m_pBCData != NULL)
	{
		xnOSFreeAligned(m_pBCData->pPackedStreamBuffer);
		xnOSFree(m_pBCData);
	}

	return XN_STATUS_OK;
}
