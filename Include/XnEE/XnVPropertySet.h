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





#ifndef __XN_V_PROPERTY_SET_H__
#define __XN_V_PROPERTY_SET_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnGeneralBuffer.h>
#include <XnPropertySet.h>
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_EE_CORE_API XnVPropertySet
{
public:
	XnVPropertySet();
	~XnVPropertySet();

	class ModuleEnumerator
	{
	public:
		ModuleEnumerator(XnVPropertySet& Set);
		~ModuleEnumerator();

		XnStatus MoveNext(XnBool& bEnd);
		XnStatus GetCurrentModule(const XnChar*& strModule) const;

	private:
		XnPropertySetModuleEnumerator* m_pInner;
	};

	class PropertyEnumerator
	{
	public:
		PropertyEnumerator(XnVPropertySet& Set, const XnChar* strModule = NULL);
		~PropertyEnumerator();

		XnStatus MoveNext(XnBool& bEnd);
		XnStatus GetCurrentInfo(XnPropertyType& nType, const XnChar*& strModule, const XnChar*& strProperty) const;
		XnStatus GetCurrentValue(XnUInt64& nValue) const;
		XnStatus GetCurrentValue(XnDouble& dValue) const;
		XnStatus GetCurrentValue(const XnChar*& strValue) const;
		XnStatus GetCurrentValue(XnGeneralBuffer& gbValue) const;

	private:
		XnPropertySetEnumerator* m_pInner;
	};

	friend class ModuleEnumerator;
	friend class PropertyEnumerator;

	XnStatus AddModule(const XnChar* strModule);
	XnStatus RemoveModule(const XnChar* strModule);

	virtual XnStatus AddProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64 nValue);
	virtual XnStatus AddProperty(const XnChar* strModule, const XnChar* strProperty, XnDouble fValue);
	virtual XnStatus AddProperty(const XnChar* strModule, const XnChar* strProperty, const XnChar* strValue);
	virtual XnStatus AddProperty(const XnChar* strModule, const XnChar* strProperty, const XnGeneralBuffer& gbValue);

	XnStatus RemoveProperty(const XnChar* strModule, const XnChar* strProperty);

private:
	friend class XnVDevice;
	XnPropertySet* m_pSet;
};

#endif //__XN_V_PROPERTY_SET_H__
