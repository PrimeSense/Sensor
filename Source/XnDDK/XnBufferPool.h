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
#ifndef __XN_BUFFER_POOL_H__
#define __XN_BUFFER_POOL_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCore/XnBuffer.h>
#include <XnListT.h>
#include <XnLog.h>
#include <XnDDK.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnBufferInPool : public XnBuffer
{
public:
	XnBufferInPool() : m_nRefCount(0), m_bDestroy(FALSE) {}
	~XnBufferInPool() {}

	XnUInt32 m_nID;
private:
	friend class XnBufferPool;

	XnUInt32 m_nRefCount;
	XnBool m_bDestroy;
};

class XN_DDK_CPP_API XnBufferPool
{
public:
	XnBufferPool();
	virtual ~XnBufferPool();

	XnStatus Init(XnUInt32 nBufferSize);
	virtual void Free();

	XnStatus ChangeBufferSize(XnUInt32 nBufferSize);

	XnStatus GetBuffer(XnBuffer** ppBuffer);

	void AddRef(XnBuffer* pBuffer);
	void DecRef(XnBuffer* pBuffer);

	inline void Lock() { xnOSEnterCriticalSection(&m_hLock); }
	inline void Unlock() { xnOSLeaveCriticalSection(&m_hLock); }

protected:
	inline XnDumpFile* Dump() { return m_dump; }
	XnStatus AddNewBuffer(void* pBuffer, XnUInt32 nSize);
	void FreeAll(XnBool bForceDestroyOfLockedBuffers);

	virtual XnStatus AllocateBuffers(XnUInt32 nSize) = 0;
	virtual void DestroyBuffer(void* pBuffer) = 0;

private:
	typedef XnListT<XnBufferInPool*> XnBuffersList;

	XnUInt32 m_nBufferSize;
	XnUInt32 m_nNextBufferID;
	XnBuffersList m_AllBuffers;
	XnBuffersList m_FreeBuffers; // a list of available buffers
	XN_CRITICAL_SECTION_HANDLE m_hLock;
	XnDumpFile* m_dump;
};

#endif // __XN_BUFFER_POOL_H__