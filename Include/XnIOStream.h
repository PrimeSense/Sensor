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





#ifndef __XN_IO_STREAM_H__
#define __XN_IO_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnStatus.h>
#include <XnCore.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_CORE_CPP_API XnIOStream
{
public:
	XnIOStream() {}
	virtual ~XnIOStream() {}

	virtual XnStatus WriteData(const XnUChar* pData, XnUInt32 nDataSize) = 0;
	virtual XnStatus ReadData(XnUChar* pData, XnUInt32 nDataSize) = 0;
	virtual XnStatus Init() { return XN_STATUS_OK; }
	virtual XnStatus Free() { return XN_STATUS_OK; }
};

#endif //__XN_IO_STREAM_H__