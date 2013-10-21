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
#ifndef __XN_STREAM_DEVICE_H__
#define __XN_STREAM_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceBase.h"
#include "XnDataPacker.h"
#include "XnStreamDeviceStreamHolder.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnStreamDevice : public XnDeviceBase
{
public:
	XnStreamDevice(const XnChar* strName, XnUInt32 nInternalBufferSize);
	~XnStreamDevice();

	XnStatus Destroy();

	XnStatus Seek(XnUInt64 nTimestamp);
	XnStatus SeekFrame(XnUInt32 nFrameID);

protected:
	XnStatus FindStream(const XnChar* strName, XnStreamDeviceStreamHolder** ppHolder);

	inline XnIOStream* GetIOStream() { return m_pIOStream; }
	inline XnDataPacker* GetDataPacker() { return m_pDataPacker; }

	XnStatus InitPacker(const XnChar* strConnectionString);

	void DestroyImpl(XnBool bHasVTable);

	virtual XnStatus CreateIOStreamImpl(const XnChar* strConnectionString, XnIOStream*& pStream) = 0;
	virtual void DestroyIOStreamImpl(XnIOStream* pStream) = 0;

private:
	XN_DISABLE_COPY_AND_ASSIGN(XnStreamDevice);

	XnIOStream* m_pIOStream;
	XnDataPacker* m_pDataPacker;
	XnUInt32 m_nInternalBufferSize;
};

#endif //__XN_STREAM_DEVICE_H__
