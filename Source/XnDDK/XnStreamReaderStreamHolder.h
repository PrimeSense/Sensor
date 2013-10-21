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
