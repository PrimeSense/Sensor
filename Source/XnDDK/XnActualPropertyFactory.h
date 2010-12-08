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





#ifndef __XN_ACTUAL_PROPERTY_FACTORY_H__
#define __XN_ACTUAL_PROPERTY_FACTORY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnProperty.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnActualPropertyFactory
{
public:
	static XnStatus CreateProperty(XnPropertyType nType, const XnChar* strName, XnProperty** ppProperty, XnUInt32 nSize = 0);
	static XnStatus FreeProperty(XnProperty* pProperty);
};

#endif //__XN_ACTUAL_PROPERTY_FACTORY_H__
