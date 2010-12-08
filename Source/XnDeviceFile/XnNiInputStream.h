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






#ifndef __XN_XN_INPUT_STREAM_H__
#define __XN_XN_INPUT_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnIOStream.h>
#include <XnOpenNI.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnInputStream : public XnIOStream
{
public:
	XnInputStream(XnPlayerInputStreamInterface* pStreamInterface, void* pCookie) :
		m_pStreamInterface(pStreamInterface),
		m_pCookie(pCookie)
	{}

	virtual ~XnInputStream() { Free(); }

	XnStatus Init() 
	{ 
		return m_pStreamInterface->Open(m_pCookie); 
	}
	XnStatus Free() 
	{ 
		m_pStreamInterface->Close(m_pCookie);
		return (XN_STATUS_OK);
	}
	XnStatus WriteData(const XnUChar* pData, XnUInt32 nDataSize)
	{
		return XN_STATUS_NOT_IMPLEMENTED;
	}
	XnStatus ReadData(XnUChar* pData, XnUInt32 nDataSize)
	{
		XnUInt32 nDummy;
		return m_pStreamInterface->Read(m_pCookie, pData, nDataSize, &nDummy);
	}
	XnStatus Tell(XnUInt32* pnOffset)
	{
		*pnOffset = m_pStreamInterface->Tell(m_pCookie);
		return (XN_STATUS_OK);
	}
	XnStatus Seek(XnUInt32 nOffset)
	{
		return m_pStreamInterface->Seek(m_pCookie, XN_OS_SEEK_SET, nOffset);
	}

private:
	XnPlayerInputStreamInterface* m_pStreamInterface;
	void* m_pCookie;
};


#endif // __XN_XN_INPUT_STREAM_H__