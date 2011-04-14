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





#ifndef __XN_STREAM_READER_STREAM_H__
#define __XN_STREAM_READER_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceStream.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnStreamReaderStream : public XnDeviceStream
{
public:
	XnStreamReaderStream(const XnChar* strType, const XnChar* strName);
	~XnStreamReaderStream();

	XnStatus Init();
	XnStatus Free();

	inline XnStreamData* GetStreamData() { return m_pLastData; }
	virtual void NewDataAvailable(XnUInt64 nTimestamp, XnUInt32 nFrameID);
	void ReMarkDataAsNew();
	void Reset();

protected:
	XnStatus WriteImpl(XnStreamData* pStreamData) { return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED; }
	XnStatus ReadImpl(XnStreamData* pStreamOutput);
	XnStatus Mirror(XnStreamData* pStreamData) const { return XN_STATUS_OK; }

	XnStatus CalcRequiredSize(XnUInt32* pnRequiredSize) const;

private:
	XnStatus OnRequiredSizeChanged();

	static XnStatus XN_CALLBACK_TYPE RequiredSizeChangedCallback(const XnProperty* pSender, void* pCookie);

	XnStreamData* m_pLastData;
	XnUInt32 m_nLastFrameIDFromStream;
};

#endif //__XN_STREAM_READER_STREAM_H__
