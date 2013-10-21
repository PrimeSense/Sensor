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
