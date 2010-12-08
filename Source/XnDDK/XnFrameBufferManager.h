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





#ifndef __XN_MULTI_FRAME_BUFFER_H__
#define __XN_MULTI_FRAME_BUFFER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnCore/XnBuffer.h>
#include <XnEvent.h>
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

	XN_DECLARE_EVENT_2ARG(NewFrameEvent, NewFrameEventInterface, XnFrameBufferManager*, pTripleBuffer, XnUInt64, nTimestamp);
	NewFrameEventInterface& OnNewFrameEvent() { return m_NewFrameEvent; }

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