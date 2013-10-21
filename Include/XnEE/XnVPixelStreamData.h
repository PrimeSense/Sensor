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
#ifndef __XN_V_PIXEL_STREAM_DATA_H__
#define __XN_V_PIXEL_STREAM_DATA_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVStreamData.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_EE_CORE_API XnVPixelStreamData : public XnVStreamData
{
public:
	XnVPixelStreamData(XnVStream* pStream, XnBool bIsLive = FALSE, XnBool bWaitForDataUpdate = FALSE);
	XnVPixelStreamData(XnUInt16 nXRes = 0, XnUInt16 nYRes = 0);
	~XnVPixelStreamData();

	XnStatus Initialize(const XnVPixelStreamData& soOther, XnUInt16 nXOffset, XnUInt16 nYOffset, XnUInt16 nXRes, XnUInt16 nYRes);

	inline XnUInt16 GetXRes() const { return m_nXRes; }
	inline XnUInt16 GetYRes() const { return m_nYRes; }
	inline XnUInt16 GetXOffset() const { return m_nXOffset; }
	inline XnUInt16 GetYOffset() const { return m_nYOffset; }
	inline XnUInt16 GetParentXRes() const { return m_nParentXRes; }
	inline XnUInt16 GetParentYRes() const { return m_nParentYRes; }

	/** Removes the parent resolution (sets it to current resolution). */
	XnStatus ResetSubMap();

protected:
	XnStatus ReadPropertiesFromStream();
	XnStatus UpdateResolution(XnUInt16 nXRes, XnUInt16 nYRes);

private:
	XN_3_6_PUBLIC_MEMBER XnUInt16 m_nXRes;
	XN_3_6_PUBLIC_MEMBER XnUInt16 m_nYRes;
	XN_3_6_PUBLIC_MEMBER XnUInt16 m_nXOffset;
	XN_3_6_PUBLIC_MEMBER XnUInt16 m_nYOffset;
	XN_3_6_PUBLIC_MEMBER XnUInt16 m_nParentXRes;
	XN_3_6_PUBLIC_MEMBER XnUInt16 m_nParentYRes;
};

#endif //__XN_V_PIXEL_STREAM_DATA_H__
