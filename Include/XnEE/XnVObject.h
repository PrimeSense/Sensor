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

