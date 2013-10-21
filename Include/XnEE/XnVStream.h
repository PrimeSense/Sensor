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
#ifndef _XNV_STREAM_H
#define _XNV_STREAM_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCore.h>
#include "XnVModule.h"
#include <XnStreamData.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * A stream is a specific kind of module - one that has a buffer attached to it.
 */
class XN_EE_CORE_API XnVStream : public XnVModule
{
public:
	XnVStream();

	XnStatus GetBuffer(void*& pBuffer) const;
	XnStatus GetBufferSize(XnUInt32& nSize) const;
	XnStatus GetBufferAllocatedSize(XnUInt32& nSize) const;

	XnStatus CopyFrom(const XnVStream& other, XnUInt64 nTimestamp = 0);
	XnStatus CopyFrom(const void* pSource, XnUInt32 nDataSize, XnUInt64 nTimestamp = 0);
};

#endif // XNV_STREAM_H
