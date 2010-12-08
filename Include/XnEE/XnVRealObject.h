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

