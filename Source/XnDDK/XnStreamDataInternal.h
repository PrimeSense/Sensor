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
#ifndef __XN_STREAM_DATA_INTERNAL_H__
#define __XN_STREAM_DATA_INTERNAL_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStreamData.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnBuffer;

struct XnStreamDataInternal
{
	XnStreamDataUpdateMode UpdateMode;
	XnStreamOutputNotificationCallback Callback;
	void* pCallbackData;
	XnBool bAllocated;
	XnUInt32 nAllocSize;
	XnBuffer* pLockedBuffer;
};

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------
/**
* Creates a new stream output object which does not allocate data.
*
* @param	ppStreamOutput	[out]	A pointer to the newly created object.
* @param	StreamName		[in]	The name of the stream that this buffer will be used for.
* @param	nAllocSize		[in]	The number of bytes to allocate for the pData buffer.
*/
XN_DDK_API XnStatus XnStreamDataCreateNoBuffer(XnStreamData** ppStreamOutput, const XnChar* StreamName);

#endif //__XN_STREAM_DATA_INTERNAL_H__
