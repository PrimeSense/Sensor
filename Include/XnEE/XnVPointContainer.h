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
#ifndef _XNV_POINT_CONTAINER_H_
#define _XNV_POINT_CONTAINER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

#include "XnVPoint.h"
#include "XnVContainer.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a specific XnVContainer, for holding XnVPoint instances.
 */

class XN_EE_CORE_API XnVPointContainer : public XnVContainer
{
public:
	XnVPointContainer(XnUInt32 nCapacity) :
	  XnVContainer(nCapacity, sizeof(XnVPoint))
	  {}

	inline XnVPoint& operator[](XnInt32 nIndex)
	{
		return ((XnVPoint*)Data())[nIndex];
	}
	inline const XnVPoint& operator[](XnInt32 nIndex) const
	{
		return ((XnVPoint*)Data())[nIndex];
	}

	inline void Add(const XnVPoint& ptPoint);
protected:
};

#endif //_XNV_POINT_CONTAINER_H_

