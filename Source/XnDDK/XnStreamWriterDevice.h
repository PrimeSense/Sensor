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
#ifndef __XN_STREAM_WRITER_DEVICE_H__
#define __XN_STREAM_WRITER_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStreamDevice.h"
#include <XnIOStream.h>
#include "XnDataPacker.h"
#include <XnFormats/XnCodec.h>
#include "XnStreamDeviceStreamHolder.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnStreamWriterDevice : public XnStreamDevice
{
public:
	XnStreamWriterDevice(const XnChar* strName, XnUInt32 nInternalBufferSize);
	~XnStreamWriterDevice();

	XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);
	XnStatus Destroy();

	XnStatus CreateStream(const XnChar* StreamType, const XnChar* StreamName = NULL, const XnPropertySet* pInitialValues = NULL);
	XnStatus DestroyStream(const XnChar* StreamName);

	XnStatus Write(XnStreamDataSet* pStreamOutputSet);
	XnStatus WriteStream(XnStreamData* pStreamOutput);

	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue);
	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue);
	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue);
	XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& Value);

protected:
	XnStatus CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppModuleHolder);
	void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder);

	virtual XnCompressionFormats GetDefaultCompression(const XnChar* strType);

private:
	void SortStreamOutputsByTimestamp(XnStreamData* apOutputs[], XnUInt32 nCount);
};

#endif //__XN_STREAM_WRITER_DEVICE_H__
