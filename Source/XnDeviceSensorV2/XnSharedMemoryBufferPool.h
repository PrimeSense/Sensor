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






#ifndef __XN_SHARED_MEMORY_BUFFER_POOL_H__
#define __XN_SHARED_MEMORY_BUFFER_POOL_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnBufferPool.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnSharedMemoryBufferPool : public XnBufferPool
{
public:
	XnSharedMemoryBufferPool(XnUInt32 nBufferCount, const XnChar* strDeviceName, const XnChar* strStreamName, XnUInt32 nMaxBufferSize);
	~XnSharedMemoryBufferPool();

	void Free();

	inline const XnChar* GetSharedMemoryName() const { return m_strName; }

	inline XnUInt32 GetBufferOffset(XnBuffer* pBuffer) const { return ((XnUChar*)pBuffer->GetData() - m_pSharedMemoryAddress); }

protected:
	virtual XnStatus AllocateBuffers();
	virtual void DestroyBuffer(XnBufferInPool* pBuffer);

private:
	XnChar m_strName[XN_FILE_MAX_PATH];
	XnUInt32 m_nMaxBufferSize;
	XN_SHARED_MEMORY_HANDLE m_hSharedMemory;
	XnUChar* m_pSharedMemoryAddress;
};

#endif // __XN_SHARED_MEMORY_BUFFER_POOL_H__