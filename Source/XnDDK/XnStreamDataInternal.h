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
