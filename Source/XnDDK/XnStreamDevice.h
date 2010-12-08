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
	XnIOStream* m_pIOStream;
	XnDataPacker* m_pDataPacker;
	XnUInt32 m_nInternalBufferSize;
};

#endif //__XN_STREAM_DEVICE_H__
