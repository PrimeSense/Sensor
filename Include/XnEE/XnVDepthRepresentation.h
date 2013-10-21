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

