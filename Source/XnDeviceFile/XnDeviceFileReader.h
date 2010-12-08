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





#ifndef __XN_DEVICE_FILE_READER_H__
#define __XN_DEVICE_FILE_READER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceFile.h"
#include <XnDDK/XnStreamReaderDevice.h>
#include <XnIOFileStream.h>
#include <XnHash.h>

#define XN_FILE_PROPERTY_INSTANCE_POINTER	"InstancePointer"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnFileBCData;
XN_DECLARE_DEFAULT_HASH(XnUInt32, XnValue, XnUIntHash);

class XnDeviceFileReader : public XnStreamReaderDevice
{
public:
	XnDeviceFileReader();
	~XnDeviceFileReader();

	XnStatus Seek(XnUInt64 nTimestamp);
	XnStatus SeekFrame(XnUInt32 nFrameID);

	XnStatus ReadNextData();

protected:
	XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);

	virtual XnStatus CreateIOStreamImpl(const XnChar* strConnectionString, XnIOStream*& pStream);
	virtual void DestroyIOStreamImpl(XnIOStream* pStream);

	virtual XnStatus CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder);

	virtual XnStatus ReadInitialState(XnPropertySet* pSet);
	virtual XnStatus HandleStreamRemoved(const XnChar* strName);
	virtual XnStatus HandleIntProperty(const XnChar* strModule, const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus HandleStreamData(XnStreamData* pDataProps, XnCompressionFormats nCompression, XnUInt32 nCompressedSize);
	virtual XnStatus HandleEndOfStream();

	XnStatus WaitForPrimaryStream(XN_EVENT_HANDLE hNewDataEvent, XnStreamDataSet* pSet);
	XnStatus WaitForStream(XN_EVENT_HANDLE hNewDataEvent, XnDeviceStream* pStream);

	inline XnIOFileStream* GetIOStream() { return (XnIOFileStream*)XnStreamReaderDevice::GetIOStream(); }

private:
	XnStatus Rewind();
	XnStatus ReadFileVersion();
	XnStatus ReadTillNextData(XnBool* pbWrapOccurred);
	void FrameDelay(XnUInt64 nTimestamp);
	XnStatus SeekTo(XnUInt64 nMinTimestamp, XnUInt32 nMinFrameID);

	XnStatus OnStreamCollectionChanged(const XnChar* StreamName, XnStreamsChangeEventType EventType);

	static void XN_CALLBACK_TYPE StreamCollectionChangedCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnStreamsChangeEventType EventType, void* pCookie);

	// Some BC functions
	XnStatus BCSeek(XnUInt64 nTimestamp);
	XnStatus BCSeekFrame(XnUInt32 nFrameID);
	XnStatus BCInit();
	XnStatus BCCalculatePackedBufferSize();
	XnStatus BCReadInitialState(XnPropertySet* pSet);
	XnStatus BCReadFrame(XnBool* pbRewind);
	XnStatus BCDestroy();

	XnBool m_bFileHasData;
	XnBool m_bStreamsCollectionChanged;
	XnUInt32 m_nFileVersion;
	XnUInt64 m_nReferenceTime;
	XnUInt64 m_nReferenceTimestamp;
	XnOSTimer m_FrameDelayTimer;
	XnActualIntProperty m_FrameDelay;
	XnFileBCData* m_pBCData;
	XnUIntHash m_PositionsToIgnore;

	XnDeviceFileReader* m_pThis;
	XnActualGeneralProperty m_InstancePointer;
};

#endif //__XN_DEVICE_FILE_READER_H__

