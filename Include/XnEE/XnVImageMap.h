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






#ifndef _XNV_IMAGE_MAP_H_
#define _XNV_IMAGE_MAP_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVImageMatrix.h"
#include "XnVStream.h"
#include "XnVPixelStreamData.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This object holds the image using XnVImageMatrix.
 * A XnVImageMap instance should be initialized either by XnVStreamDetails or
 * by another XnVImageMap instance using the Clone method.
 */

class XN_EE_CORE_API XnVImageMap : public XnVPixelStreamData
{
public:
	XnVImageMap(XnVStream* pStream, XnBool bIsLive = FALSE, XnBool bWaitForDataUpdate = FALSE);
	XnVImageMap();
	~XnVImageMap();

	typedef enum
	{
		XNV_IMAGE_RED,
		XNV_IMAGE_GREEN,
		XNV_IMAGE_BLUE,
	} XnVImageComponent;

	XnStatus Initialize(const XnVImageMap& imOther, XnUInt16 nXRes, XnUInt16 nYRes, XnUInt16 nXOffset = 0, XnUInt16 nYOffset = 0);

	XnStatus Clone(XnVImageMap& imOther) const;

	XnUInt8* Data();
	const XnUInt8* Data() const;
	XnUInt8 &operator[](XnInt32 nIndex);
	const XnUInt8 &operator[](XnInt32 nIndex) const;

	XnUInt8& operator()(XnInt32 nX, XnInt32 nY, XnVImageComponent eComponent);
	const XnUInt8& operator()(XnInt32 nX, XnInt32 nY, XnVImageComponent eComponent) const;

	void Clear();

	inline XnUInt8 GetBytesPerPixel() const { return m_nBytesPerPixel; }
	inline XnOutputFormats GetOutputFormat() const { return m_OutputFormat; }

	XnStatus CopyFrom(const XnVImageMap& im, XnUInt64 nTimestamp = 0);
	
protected:
	XnStatus ReadPropertiesFromStream();

private:
	XN_3_6_PUBLIC_MEMBER XnVImageMatrix m_pImage;
	XN_3_6_PUBLIC_MEMBER XnUInt32 m_ImageFormat;
	XN_3_6_PUBLIC_MEMBER XnOutputFormats m_OutputFormat;
	XN_3_6_PUBLIC_MEMBER XnUInt8 m_nBytesPerPixel;
};

#endif //_XNV_IMAGE_MAP_H_

