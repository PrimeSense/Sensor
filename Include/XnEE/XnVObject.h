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






#ifndef _XNV_OBJECT_H_
#define _XNV_OBJECT_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnV3DVector.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * Any Object in the scene, either inserted by the user or that really exists there
 * and can be defined, is an XnVObject.
 * There are 2 subclasses of XnVObject: XnVVirtualObject, for any user-created object,
 * such as boxes and spheres, and XnVRealObject, for any object that really exist
 * in the scene, such as hands, connected components and collision.
 */

class XN_EE_FW_API XnVObject
{
public:
	typedef XnUInt16 XnVLabel;

	XnVLabel GetID() const {return m_nID;}
	void SetID(XnVLabel Label) {m_nID = Label;}
	const XnVPoint& GetPoint() const {return m_ptRepresentative;}
	void SetPoint(const XnV3DVector& ptPoint) {m_ptRepresentative = ptPoint;}
protected:
	XnVLabel m_nID;
	XnV3DVector m_ptRepresentative;
};

#endif //_XNV_OBJECT_H_

