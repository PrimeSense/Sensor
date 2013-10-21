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