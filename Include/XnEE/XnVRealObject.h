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
#ifndef _XNV_REAL_OBJECT_H_
#define _XNV_REAL_OBJECT_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVObject.h"
#include "XnVDepthMap.h"
//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * A Real Object is an object that exists in the scene.
 * These objects contain the XnVDepthMap that describes only them.
 * There are currently no implemented real objects.
 */

class XN_EE_FW_API XnVRealObject : public XnVObject
{
public:
	XnVRealObject()
	{
		m_pdmDepthMap = XN_NEW(XnVDepthMap);
		m_bAllocated = true;
	}
	XnVRealObject(XnVDepthMap* pdmMap)
	{
		m_pdmDepthMap = pdmMap;
		m_bAllocated = false;
	}

	virtual ~XnVRealObject()
	{
		if (m_bAllocated && m_pdmDepthMap != NULL)
			XN_DELETE(m_pdmDepthMap);
	}

	void SetDepthMap(XnVDepthMap* pdmMap)
	{
		if (m_bAllocated)
			XN_DELETE(m_pdmDepthMap);
		m_pdmDepthMap = pdmMap;
		m_bAllocated = false;
	}

	XnVDepthMap* GetDepthMap() const
	{
		return m_pdmDepthMap;
	}

	inline XnUInt16 GetXRes() const
	{
		return m_pdmDepthMap->GetXRes();
	}
	inline XnUInt16 GetYRes() const
	{
		return m_pdmDepthMap->GetYRes();
	}

	virtual XnDepthPixel GetAt(XnUInt16 nX, XnUInt16 nY) const
	{
		return GetAt(nX + nY*GetXRes());
	}

	virtual XnDepthPixel GetAt(XnUInt32 nPos) const
	{
		return m_pdmDepthMap->Data()[nPos];
	}

protected:
	XnVDepthMap* m_pdmDepthMap;
	XnBool m_bAllocated;
};

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------


#endif //_XNV_REAL_OBJECT_H_

