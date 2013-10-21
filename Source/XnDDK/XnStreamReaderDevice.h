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
#ifndef __XN_STREAM_READER_DEVICE_H__
#define __XN_STREAM_READER_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStreamDevice.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnStreamReaderDevice : public XnStreamDevice
{
public:
	XnStreamReaderDevice(const XnChar* strName, XnUInt32 nInternalBufferSize);
	~XnStreamReaderDevice();

	XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);

protected:
	XnStatus CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder);
	void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder);

	XnStatus SetInitialState(const XnDeviceConfig* pDeviceConfig, XnPropertySet* pSet);

	virtual XnStatus ReadInitialState(XnPropertySet* pSet);
	virtual XnStatus ReadNextEventFromStream(XnPackedDataType* pnObjectType = NULL);
	virtual XnStatus HandlePackedObject(XnPackedDataType nObjectType);
	virtual XnStatus HandleNewStream(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialValues);
	virtual XnStatus HandleStreamRemoved(const XnChar* strName);
	virtual XnStatus HandleIntProperty(const XnChar* strModule, const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus HandleRealProperty(const XnChar* strModule, const XnChar* strName, XnDouble dValue);
	virtual XnStatus HandleStringProperty(const XnChar* strModule, const XnChar* strName, const XnChar* strValue);
	virtual XnStatus HandleGeneralProperty(const XnChar* strModule, const XnChar* strName, const XnGeneralBuffer& gbValue);
	virtual XnStatus HandleStreamData(XnStreamData* pDataProps, XnCompressionFormats nCompression, XnUInt32 nCompressedSize);
	virtual XnStatus HandleEndOfStream();

private:
	XnStatus ReadNewStream();
	XnStatus ReadStreamRemoved();
	XnStatus ReadIntProperty();
	XnStatus ReadRealProperty();
	XnStatus ReadStringProperty();
	XnStatus ReadGeneralProperty();
	XnStatus ReadStreamData();
};

#endif //__XN_STREAM_READER_DEVICE_H__
