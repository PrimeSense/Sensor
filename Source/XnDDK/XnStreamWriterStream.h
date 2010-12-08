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
	XnStatus ReadImpl(XnStreamData* pStreamOutput) { return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED; }
	XnStatus Mirror(XnStreamData* pStreamData) const { return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED; }

	XnStatus CalcRequiredSize(XnUInt32* pnRequiredSize) const;

private:
	XnUInt32 m_nFrameID;
	XnDataPacker* m_pDataPacker;
};

#endif //__XN_STREAM_WRITER_STREAM_H__
