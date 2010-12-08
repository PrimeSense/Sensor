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





#ifndef __XN_STREAM_READER_STREAM_HOLDER_H__
#define __XN_STREAM_READER_STREAM_HOLDER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStreamDeviceStreamHolder.h"
#include "XnShiftToDepthStreamHelper.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API XnStreamReaderStreamHolder : public XnStreamDeviceStreamHolder
{
public:
	XnStreamReaderStreamHolder(XnDeviceStream* pStream);
	~XnStreamReaderStreamHolder();

	virtual XnStatus Init(const XnActualPropertiesHash* pProps);
	virtual XnStatus Free();

private:
	// helpers
	XnShiftToDepthStreamHelper* m_pS2DHelper;
};

#endif //__XN_STREAM_READER_STREAM_HOLDER_H__
