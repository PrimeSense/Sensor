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

