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






#ifndef __XN_BUFFER_POOL_H__
#define __XN_BUFFER_POOL_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCore/XnBuffer.h>
#include <XnList.h>
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
	XnBufferPool(XnUInt32 nBufferCount);
	virtual ~XnBufferPool();

	XnStatus Init(XnUInt32 nBufferSize);
	virtual void Free();

	XnStatus ChangeBufferSize(XnUInt32 nBufferSize);

	XnStatus GetBuffer(XnBuffer** ppBuffer);

	void AddRef(XnBuffer* pBuffer);

	void DecRef(XnBuffer* pBuffer);

	inline void Lock() { xnOSEnterCriticalSection(&m_hLock); }
	inline void Unlock() { xnOSLeaveCriticalSection(&m_hLock); }

	inline XnDump& Dump() { return m_dump; }

protected:
	XN_DECLARE_LIST(XnBufferInPool*, XnBuffersList);

	void FreeAll(XnBool bForceDestroyOfLockedBuffers);

	virtual XnStatus AllocateBuffers() = 0;
	virtual void DestroyBuffer(XnBufferInPool* pBuffer) = 0;

protected:
	XnUInt32 m_nBufferSize;
	XnUInt32 m_nBufferCount;
	XnBuffersList m_AllBuffers;
	XnBuffersList m_FreeBuffers; // a list of available buffers
	XN_CRITICAL_SECTION_HANDLE m_hLock;
	XnDump m_dump;
};

#endif // __XN_BUFFER_POOL_H__