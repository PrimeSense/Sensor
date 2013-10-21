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
#ifndef __XN_SENSOR_DEVICE_H__
#define __XN_SENSOR_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleCppInterface.h>
#include <XnDDK/XnDataPacker.h>
#include <XnCppWrapper.h>
#include <XnStringsHashT.h>
#include <XnFormats/XnNiCodec.h>
#include <XnDDK/XnActualPropertiesHash.h>
#include "XnDeviceFileReaderBC.h"
#include "XnNiInputStream.h"
#include "XnDeviceFile.h"
#include <XnDDK/XnShiftToDepth.h>
#include <XnEventT.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnFileDevice : virtual public xn::ModulePlayer
{
public:
	XnFileDevice(xn::Context& context, const XnChar* strInstanceName);
	virtual ~XnFileDevice();

	XnStatus Init();

	virtual XnBool IsCapabilitySupported(const XnChar* /*strCapabilityName*/) { return FALSE; }

	virtual const XnChar* GetSupportedFormat() { return "XNS"; }
	virtual XnStatus SetInputStream(void *pStreamCookie, XnPlayerInputStreamInterface *pStream);
	virtual XnStatus ReadNext();
	virtual XnStatus SetNodeNotifications(void *pNodeNotificationsCookie, XnNodeNotifications *pNodeNotifications);
	virtual XnStatus SetRepeat(XnBool bRepeat);
	virtual XnStatus SeekToTimeStamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin);
	virtual XnStatus SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin);
	virtual XnStatus TellTimestamp(XnUInt64& nTimestamp);
	virtual XnStatus TellFrame(const XnChar* strNodeName, XnUInt32 &nFrameOffset);
	virtual XnStatus GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames);
	virtual XnBool IsEOF();
	virtual XnStatus RegisterToEndOfFileReached(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromEndOfFileReached(XnCallbackHandle hCallback);

private:
	void Free();

	XnStatus ReadFileVersion();
	XnStatus ReadNextEventFromStream(XnPackedDataType* pnObjectType = NULL);
	XnStatus ReadInitialState(XnPropertySet* pSet);
	XnStatus SetInitialState(XnPropertySet* pSet);
	XnStatus ReadTillNextData(XnBool* pbWrapOccurred);

	XnStatus HandlePackedObject(XnPackedDataType nObjectType);
	XnStatus ReadNewStream();
	XnStatus ReadStreamRemoved();
	XnStatus ReadIntProperty();
	XnStatus ReadRealProperty();
	XnStatus ReadStringProperty();
	XnStatus ReadGeneralProperty();
	XnStatus ReadStreamData();
	XnStatus HandleNewStream(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialValues);
	XnStatus HandleStreamRemoved(const XnChar* strName);
	XnStatus HandleIntProperty(const XnChar* strModule, const XnChar* strName, XnUInt64 nValue);
	XnStatus HandleRealProperty(const XnChar* strModule, const XnChar* strName, XnDouble dValue);
	XnStatus HandleStringProperty(const XnChar* strModule, const XnChar* strName, const XnChar* strValue);
	XnStatus HandleGeneralProperty(const XnChar* strModule, const XnChar* strName, const XnGeneralBuffer& gbValue);
	XnStatus HandleStreamData(XnStreamData* pDataProps, XnCompressionFormats nCompression, XnUInt32 nCompressedSize);
	XnStatus HandleEndOfStream();
	XnStatus Rewind();
	XnStatus SeekTo(XnUInt64 nMinTimestamp, const XnChar* strNode, XnUInt32 nFrameID);
	XnStatus UpdateS2DTables(const xn::DepthGenerator& depth);
	XnStatus UpdateRWData(const xn::DepthGenerator& depth);
	XnStatus CreateCodec(xn::ProductionNode& node);
	XnStatus CheckIRCompatibility(xn::ProductionNode& node);
	XnNodeHandle GetSelfNodeHandle();

	// Some BC functions
	XnStatus BCSeek(XnUInt64 nTimestamp);
	XnStatus BCSeekFrame(XnUInt32 nFrameID);
	XnStatus BCInit();
	XnStatus BCCalculatePackedBufferSize();
	XnStatus BCReadInitialState(XnPropertySet* pSet);
	XnStatus BCReadFrame(XnBool* pbRewind);
	XnStatus BCDestroy();

	xn::Context m_context;

	XnInputStream* m_pInputStream;
	XnDataPacker* m_pDataPacker;

	XnNodeNotifications* m_pNotifications;
	void* m_pNotificationsCookie;

	typedef struct XnNodeInfo
	{
		xn::Codec codec;
		XnCodec* pXnCodec;
		XnUInt32 nCurrFrameID;
		XnBool bIRisRGB;
	} XnNodeInfo;

	typedef XnStringsHashT<XnNodeInfo> XnNodeInfoMap;
	XnNodeInfoMap m_nodeInfoMap;
	XnNodeInfoMap m_ignoreNewNodes;

	XnBool m_bHighresTimestamps;
	XnStreamData* m_pStreamData;
	XnBool m_bRepeat;

	XnUInt32 m_nFileVersion;
	XnBool m_bFileHasData;
	XnBool m_bNodeCollectionChanged;

	XnUInt64 m_nCurrTimestamp;

	typedef XnHashT<XnUInt32, XnValue> XnUIntHash;
	XnUIntHash m_PositionsToIgnore; // used for seeking

	XnEventNoArgs m_eofEvent;
	XnBool m_bEOF;

	XnShiftToDepthTables m_ShiftToDepth;

	XnFileBCData* m_pBCData;
	const XnChar* m_strName;
	XnNodeHandle m_hSelf;
};

#endif // __XN_SENSOR_DEVICE_H__