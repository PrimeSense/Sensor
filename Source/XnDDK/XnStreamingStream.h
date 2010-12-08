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





#ifndef __XN_STREAMING_STREAM_H__
#define __XN_STREAMING_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnDeviceStream.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** Represents a base class for streams which are not frame based, by streaming. */
class XN_DDK_CPP_API XnStreamingStream : public XnDeviceStream
{
public:
	XnStreamingStream(const XnChar* csType, const XnChar* csName);
	~XnStreamingStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& ReadChunkSizeProperty() { return m_ReadChunkSize; }

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline XnUInt32 GetReadChunkSize() { return (XnUInt32)m_ReadChunkSize.GetValue(); }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetReadChunkSize(XnUInt32 nChunkSize);

private:
	static XnStatus XN_CALLBACK_TYPE SetReadChunkSizeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnActualIntProperty m_IsStreamingStream;
	XnActualIntProperty m_ReadChunkSize;
};

#endif //__XN_STREAMING_STREAM_H__