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






#ifndef _XNV_DEPTH_REPRESENTATION_H_
#define _XNV_DEPTH_REPRESENTATION_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVStream.h"
#include "XnVRealWorldTranslator.h"
#include "XnVStatus.h"
#include "XnVPixelStreamData.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a general representation of depths.
 * It holds the resolutions, the instance of XnVStreamDetails that was used to create
 * the XnVDepthRepresentation and an instance of XnVRealWorldTranslator,
 * to translate between Projective and Real World coordinations
 * according to the resolutions and offset specifics.
 * Its single implemented subclass is XnVDepthMap.
 */

class XN_EE_CORE_API XnVDepthRepresentation : public XnVPixelStreamData
{
public:
	typedef enum {
		XNV_UNKNOWN,
		XNV_PROJECTIVE,
		XNV_REAL_WORLD
	} XnVProjectionMode;

	XnVDepthRepresentation();
	XnVDepthRepresentation(XnVStream* pStream, XnBool bIsLive = FALSE, XnBool bWaitForDataUpdate = FALSE);
	XnVDepthRepresentation(XnUInt16 nXRes, XnUInt16 nYRes);
	virtual ~XnVDepthRepresentation();

	XnStatus Initialize(const XnVDepthRepresentation& drOther, XnUInt16 nXOffset, XnUInt16 nYOffset, XnUInt16 nXRes, XnUInt16 nYRes);

	void SetMode(XnVProjectionMode mode);
	XnVProjectionMode GetMode() const;

	inline XnDepthPixel GetZMin() const { return m_nZMin; }
	inline XnDepthPixel GetZRes() const { return m_nZRes; }
	inline XnDepthPixel GetNoSampleValue() const { return m_nNoSampleValue; }
	inline XnDepthPixel GetShadowValue() const { return m_nShadowValue; }
	inline XnVRealWorldTranslator* GetRealWorldTranslator() const { return m_pRWTranslator; }

protected:
	XnStatus ReadPropertiesFromStream();

private:

	XN_3_6_PUBLIC_MEMBER XnDepthPixel m_nZMin;
	XN_3_6_PUBLIC_MEMBER XnDepthPixel m_nZRes;
	XN_3_6_PUBLIC_MEMBER XnDepthPixel m_nNoSampleValue;
	XN_3_6_PUBLIC_MEMBER XnVRealWorldTranslator* m_pRWTranslator;
	XN_3_6_PUBLIC_MEMBER XnDepthPixel m_nShadowValue;
	XN_3_6_PUBLIC_MEMBER XnVProjectionMode m_eMode;
};


#endif //_XNV_DEPTH_REPRESENTATION_H_

