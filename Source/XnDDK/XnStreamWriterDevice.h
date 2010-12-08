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
