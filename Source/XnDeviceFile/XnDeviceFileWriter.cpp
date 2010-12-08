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
#include "XnDeviceFileWriter.h"
#include <XnLog.h>
#include <XnOS.h>
#include "XnDeviceFile.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDeviceFileWriter::XnDeviceFileWriter() :
	XnStreamWriterDevice(XN_DEVICE_NAME, XN_DEVICE_FILE_MAX_INTERNAL_BUFFER),
	m_bTimerStarted(FALSE)
{
}

XnDeviceFileWriter::~XnDeviceFileWriter()
{
}

XnStatus XnDeviceFileWriter::Destroy()
{
	if (m_bTimerStarted)
	{
		xnOSStopTimer(&m_Timer);
		m_bTimerStarted = FALSE;
	}

	return XnStreamWriterDevice::Destroy();
}

XnStatus XnDeviceFileWriter::CreateStream(const XnChar* StreamType, const XnChar* StreamName /* = NULL */, const XnPropertySet* pInitialValues /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// create stream using base (this will also write it down to the file)
	nRetVal = XnStreamWriterDevice::CreateStream(StreamType, StreamName, pInitialValues);
	XN_IS_STATUS_OK(nRetVal);

	// now, we leave a place for a property change - the number of frames. We will update this
	// property once the stream is closed.
	XnFileWriterStream* pStream;
	nRetVal = FindStream(StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = GetIOStream()->Tell(&pStream->m_nNumFramesPos);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetDataPacker()->WriteProperty(StreamName, XN_STREAM_PROPERTY_NUMBER_OF_FRAMES, 0ULL);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileWriter::DestroyStream(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// before closing this stream, we want to write down how many frames were written
	XnFileWriterStream* pStream;
	nRetVal = FindStream(StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);

	XnUInt32 nCurPos;
	nRetVal = GetIOStream()->Tell(&nCurPos);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetIOStream()->Seek(pStream->m_nNumFramesPos);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetDataPacker()->WriteProperty(StreamName, XN_STREAM_PROPERTY_NUMBER_OF_FRAMES, pStream->GetNumberOfFrames());
	XN_IS_STATUS_OK(nRetVal);

	// now seek back
	nRetVal = GetIOStream()->Seek(nCurPos);
	XN_IS_STATUS_OK(nRetVal);

	// and destroy it
	nRetVal = XnStreamWriterDevice::DestroyStream(StreamName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileWriter::WriteStream(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// start the timer on the first data written
	nRetVal = StartTimer();
	XN_IS_STATUS_OK(nRetVal);

	// fill in timestamp if needed
	if (pStreamOutput->nTimestamp == 0)
	{
		XnUInt64 nNow;
		xnOSQueryTimer(m_Timer, &nNow);

		if (!IsHighResTimestamps())
		{
			nNow /= 1000;
		}

		pStreamOutput->nTimestamp = nNow;
	}

	// and write it down (using base)
	nRetVal = XnStreamWriterDevice::WriteStream(pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileWriter::Write(XnStreamDataSet* pStreamOutputSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);

	// get a list of objects in the set
	XnStreamData* aOutputs[XN_DEVICE_BASE_MAX_STREAMS_COUNT];
	XnUInt32 nCount = XN_DEVICE_BASE_MAX_STREAMS_COUNT;
	nRetVal = XnStreamDataSetCopyToArray(pStreamOutputSet, aOutputs, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// BC: old applications wrote down all streams every frame, even if no new data. This might
	// cause some of the streams to have timestamp 0, while other have a real timestamp.
	// In this case, remove these frames. However, we need to check - if all timestamps
	// are 0, then we'll fill it up by ourselves.
	XnBool bSomeHasTimestamps = FALSE;

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		if (aOutputs[i]->nTimestamp != 0)
		{
			bSomeHasTimestamps = TRUE;
			break;
		}
	}

	if (bSomeHasTimestamps)
	{
		// remove all the ones with zero timestamp
		for (XnUInt32 i = 0; i < nCount; ++i)
		{
			if (aOutputs[i]->nTimestamp == 0)
			{
				aOutputs[i]->bIsNew = FALSE;
			}
		}
	}

	// OK. write it down using base
	nRetVal = XnStreamWriterDevice::Write(pStreamOutputSet);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileWriter::StartTimer()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (!m_bTimerStarted)
	{
		nRetVal = xnOSStartHighResTimer(&m_Timer);
		XN_IS_STATUS_OK(nRetVal);

		m_bTimerStarted = TRUE;
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceFileWriter::CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnFileWriterStream* pStream;
	XN_VALIDATE_NEW(pStream, XnFileWriterStream, StreamType, StreamName, GetDataPacker());

	XnStreamDeviceStreamHolder* pHolder = XN_NEW(XnStreamDeviceStreamHolder, pStream, FALSE);
	if (pHolder == NULL)
	{
		XN_DELETE(pStream);
		return XN_STATUS_ALLOC_FAILED;
	}

	*ppStreamHolder = pHolder;
	
	return (XN_STATUS_OK);
}

void XnDeviceFileWriter::DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder)
{
	XN_DELETE(pStreamHolder->GetModule());
	XN_DELETE(pStreamHolder);
}

XnStatus XnDeviceFileWriter::CreateIOStreamImpl(const XnChar* strConnectionString, XnIOStream*& pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_NEW_AND_INIT(pStream, XnIOFileStream, strConnectionString, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE);

	// write down the magic
	nRetVal = pStream->WriteData((const XnUChar*)XN_DEVICE_FILE_MAGIC_V4, strlen(XN_DEVICE_FILE_MAGIC_V4));
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pStream);
		pStream = NULL;
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

void XnDeviceFileWriter::DestroyIOStreamImpl(XnIOStream* pStream)
{
	pStream->Free();
	XN_DELETE(pStream);
}

XnStatus XnDeviceFileWriter::FindStream(const XnChar* strName, XnFileWriterStream** ppStream)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnStreamDeviceStreamHolder* pHolder = NULL;
	nRetVal = XnStreamWriterDevice::FindStream(strName, &pHolder);
	XN_IS_STATUS_OK(nRetVal);

	*ppStream = (XnFileWriterStream*)pHolder->GetStream();
	
	return (XN_STATUS_OK);
}
