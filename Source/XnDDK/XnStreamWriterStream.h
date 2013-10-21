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
#ifndef __XN_STREAM_WRITER_STREAM_H__
#define __XN_STREAM_WRITER_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceStream.h"
#include "XnDataPacker.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnStreamWriterStream : public XnDeviceStream
{
public:
	XnStreamWriterStream(const XnChar* strType, const XnChar* strName, XnDataPacker* pDataPacker);
	~XnStreamWriterStream();

	inline XnUInt32 GetFrameID() { return m_nFrameID; }
	inline XnDataPacker* GetDataPacker() { return m_pDataPacker; }

protected:
	XnStatus WriteImpl(XnStreamData* pStreamData);
	XnStatus ReadImpl(XnStreamData* /*pStreamOutput*/) { return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED; }
	XnStatus Mirror(XnStreamData* /*pStreamData*/) const { return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED; }

	XnStatus CalcRequiredSize(XnUInt32* pnRequiredSize) const;

private:
	XnUInt32 m_nFrameID;
	XnDataPacker* m_pDataPacker;
};

#endif //__XN_STREAM_WRITER_STREAM_H__
