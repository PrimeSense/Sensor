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
