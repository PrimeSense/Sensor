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
