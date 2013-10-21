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
#ifndef __XN_STREAM_DEVICE_STREAM_HOLDER_H__
#define __XN_STREAM_DEVICE_STREAM_HOLDER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceModuleHolder.h"
#include "XnDeviceStream.h"
#include <XnHashT.h>
#include <XnFormats/XnCodec.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnStreamDeviceStreamHolder : public XnDeviceModuleHolder
{
public:
	XnStreamDeviceStreamHolder(XnDeviceStream* pStream, XnBool bCompressionIsReadOnly);

	virtual XnStatus Init(const XnActualPropertiesHash* pProps);
	virtual XnStatus Free();

	inline XnDeviceStream* GetStream() { return (XnDeviceStream*)GetModule(); }

	inline XnCompressionFormats GetCompression() const { return (XnCompressionFormats)m_Compression.GetValue(); }

	inline XnCodec* GetCodec() const { return m_pCodec; }

protected:
	XnActualIntProperty& CompressionProperty() { return m_Compression; }

private:
	XnStatus ChooseCodec();

	static XnStatus XN_CALLBACK_TYPE CodecPropertyChangedCallback(const XnProperty* pSender, void* pCookie);

	XnActualIntProperty m_Compression;
	XnCodec* m_pCodec;

	typedef XnHashT<XnProperty*, XnValue> XnPropertiesHash;
	XnPropertiesHash m_CodecProperties;
};

#endif //__XN_STREAM_DEVICE_STREAM_HOLDER_H__
