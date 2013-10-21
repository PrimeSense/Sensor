/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.x Alpha                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Sensor                                    *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
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
#include <XnHashT.h>

#define XN_FILE_PROPERTY_INSTANCE_POINTER	"InstancePointer"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnFileBCData;
typedef XnHashT<XnUInt64, XnValue> XnUIntHash;

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

	static void XN_CALLBACK_TYPE StreamCollectionChangedCallback(const XnStreamCollectionChangedEventArgs& args, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetInstanceCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);

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

	XnGeneralProperty m_InstancePointer;
};

#endif //__XN_DEVICE_FILE_READER_H__

