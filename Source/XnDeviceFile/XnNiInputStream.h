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
	XnStatus WriteData(const XnUChar* /*pData*/, XnUInt32 /*nDataSize*/)
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