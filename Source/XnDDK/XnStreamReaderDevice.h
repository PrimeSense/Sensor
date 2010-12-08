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
