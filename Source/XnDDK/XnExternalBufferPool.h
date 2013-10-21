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
#ifndef _XN_EXTERNAL_BUFFER_POOL_H_
#define _XN_EXTERNAL_BUFFER_POOL_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnBufferPool.h>
#include <XnArray.h>
#include <XnGeneralBuffer.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnExternalBufferPool : public XnBufferPool
{
public:
	XnExternalBufferPool();
	~XnExternalBufferPool();

	XnStatus SetBuffers(XnUInt32 nCount, const XnGeneralBuffer* aBuffers);

protected:
	virtual XnStatus AllocateBuffers(XnUInt32 nSize);
	virtual void DestroyBuffer(void* pBuffer);

private:
	XnArray<XnGeneralBuffer> m_buffers;
};


#endif // _XN_EXTERNAL_BUFFER_POOL_H_