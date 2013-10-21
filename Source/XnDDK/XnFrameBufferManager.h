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
#ifndef __XN_MULTI_FRAME_BUFFER_H__
#define __XN_MULTI_FRAME_BUFFER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnCore/XnBuffer.h>
#include <XnEventT.h>
#include "XnBufferPool.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XN_DDK_CPP_API XnFrameBufferManager
{
public:
	XnFrameBufferManager(XnBufferPool* pBufferPool);
	~XnFrameBufferManager();

	XnStatus Init(XnUInt32 nBufferSize);
	XnStatus Reallocate(XnUInt32 nBufferSize);
	void Free();

	inline XnBuffer* GetWriteBuffer() 
	{ 
		// NOTE: no need to lock buffer, as we assume the same thread is the one that is responsible
		// for marking working buffer as stable.
		return m_pWorkingBuffer; 
	}

	void MarkWriteBufferAsStable(XnUInt64 nTimestamp, XnUInt32* pnFrameID);

	void ReadLastStableBuffer(XnBuffer** ppBuffer, XnUInt64* pnTimestamp, XnUInt32* pnFrameID);

	XnStatus CopyLastStableBuffer(void* pDest, XnUInt32 nDestSize, XnUInt32* pnWritten);

	inline XnUInt32 GetLastFrameID() const { return m_nStableFrameID; }

	typedef struct NewFrameEventArgs
	{
		XnFrameBufferManager* pTripleBuffer;
		XnUInt64 nTimestamp;
	} NewFrameEventArgs;

	typedef XnEventT<NewFrameEventArgs> NewFrameEvent;
	NewFrameEvent::TInterface& OnNewFrameEvent() { return m_NewFrameEvent; }

private:
	XnBufferPool* m_pBufferPool;
	XnBuffer* m_pWorkingBuffer;
	XnBuffer* m_pStableBuffer;
	XnUInt32 m_nStableFrameID;
	XnUInt64 m_nStableTimestamp;
	NewFrameEvent m_NewFrameEvent;
	XN_CRITICAL_SECTION_HANDLE m_hLock;
};

#endif //__XN_MULTI_FRAME_BUFFER_H__