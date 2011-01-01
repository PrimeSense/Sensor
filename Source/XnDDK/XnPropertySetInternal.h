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




#ifndef __XN_PROPERTY_SET_INTERNAL_H__
#define __XN_PROPERTY_SET_INTERNAL_H__

#include <XnPropertySet.h>
#include "XnActualPropertiesHash.h"

XN_DECLARE_STRINGS_HASH_DECL(XN_DDK_CPP_API, XnActualPropertiesHash*, XnPropertySetDataInternal)

class XnPropertySetData;

struct XnPropertySet
{
	XnPropertySetData* pData;
};

class XnPropertySetData : public XnPropertySetDataInternal
{
public:
	~XnPropertySetData()
	{
		XnPropertySet set;
		set.pData = this;
		XnPropertySetClear(&set);
	}
};

#define _XN_PROPERTY_SET_NAME(name)	__ ## name ## _ ## Data

#define XN_PROPERTY_SET_CREATE_ON_STACK(name)		\
	XnPropertySetData _XN_PROPERTY_SET_NAME(name);	\
	XnPropertySet name;								\
	name.pData = &_XN_PROPERTY_SET_NAME(name);

XN_DDK_API XnStatus XnPropertySetDataAttachModule(XnPropertySetData* pSetData, const XnChar* strModuleName, XnActualPropertiesHash* pModule);
XN_DDK_API XnStatus XnPropertySetDataDetachModule(XnPropertySetData* pSetData, const XnChar* strModuleName, XnActualPropertiesHash** ppModule);
XN_DDK_API XnStatus XnPropertySetCloneModule(const XnPropertySet* pSource, XnPropertySet* pDest, const XnChar* strModule, const XnChar* strNewName);

#endif //__XN_PROPERTY_SET_INTERNAL_H__
