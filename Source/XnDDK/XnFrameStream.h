/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
#ifndef __XN_FRAME_STREAM_H__
#define __XN_FRAME_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceStream.h"
#include "XnFrameBufferManager.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** Represents a stream that is frame-based. */
class XN_DDK_CPP_API XnFrameStream : public XnDeviceStream
{
public:
	XnFrameStream(const XnChar* csType, const XnChar* csName);
	~XnFrameStream() { Free(); }

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline XnUInt32 GetFPS() const { return (XnUInt32)m_FPS.GetValue(); }
	inline XnBufferPool* GetBufferPool() { return m_pBufferPool; }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	XnStatus Free();
	XnStatus CreateStreamData(XnStreamData** ppStreamData);
	XnStatus Read(XnStreamData* pStreamOutput);

protected:
	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& FPSProperty() { return m_FPS; }

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	XnStatus GetTripleBuffer(XnFrameBufferManager** pBufferManager);

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetFPS(XnUInt32 nFPS);

	//---------------------------------------------------------------------------
	// Virtual Methods
	//---------------------------------------------------------------------------
	virtual XnStatus PostProcessFrame(XnStreamData* /*pFrameData*/) { return XN_STATUS_OK; }
	virtual XnStatus ReallocTripleFrameBuffer();

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus ReadImpl(XnStreamData* pStreamOutput);

private:
	XnStatus OnRequiredSizeChanging();
	XnStatus GetLastRawFrame(XnDynamicSizeBuffer* gbValue);
	XnStatus SetExternalBufferPool(XnUInt32 nCount, XnGeneralBuffer* aBuffers);

	static XnStatus XN_CALLBACK_TYPE SetFPSCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE RequiredSizeChangedCallback(const XnProperty* pSenser, void* pCookie);
	static void XN_CALLBACK_TYPE OnTripleBufferNewData(const XnFrameBufferManager::NewFrameEventArgs& args, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetLastRawFrameCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetExternalBufferPoolCallback(XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnFrameBufferManager* m_pBufferManager;
	XnBufferPool* m_pBufferPool;
	XnBool m_bPoolAllocated;

	XnUInt32 m_nLastReadFrame; // the ID that was given

	XnActualIntProperty m_IsFrameStream;
	XnActualIntProperty m_FPS;
	XnGeneralProperty m_externalBufferPool;
	XnGeneralProperty m_LastRawFrame;
	XnBool m_bTripleBufferReallocated;
};

#endif //__XN_FRAME_STREAM_H__