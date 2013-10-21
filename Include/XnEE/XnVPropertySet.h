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
