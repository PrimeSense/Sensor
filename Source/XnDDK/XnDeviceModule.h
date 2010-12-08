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





#ifndef __XN_DEVICE_MODULE_H__
#define __XN_DEVICE_MODULE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnStringsHash.h>
#include <XnDDK/XnPropertySetInternal.h>
#include <XnDDK/XnIntProperty.h>
#include <XnDDK/XnRealProperty.h>
#include <XnDDK/XnStringProperty.h>
#include <XnDDK/XnGeneralProperty.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* Holds a set of properties related to a specific module.
*/
class XN_DDK_CPP_API XnDeviceModule
{
public:
	XnDeviceModule(const XnChar* strName);
	virtual ~XnDeviceModule();

	virtual XnStatus Init();
	virtual XnStatus Free();

	inline const XnChar* GetName() const { return m_strName; }

	XnStatus AddProperty(XnProperty* pProperty);
	XnStatus AddProperties(XnProperty** apProperties, XnUInt32 nCount);

	XnStatus DoesPropertyExist(const XnChar* strName, XnBool* pbDoesExist) const;
	XnStatus GetPropertyType(const XnChar* strName, XnPropertyType* pnType) const;

	XnStatus GetProperty(const XnChar* strName, XnProperty** ppProperty) const;

	virtual XnStatus GetProperty(const XnChar* strName, XnUInt64* pnValue) const;
	virtual XnStatus GetProperty(const XnChar* strName, XnDouble* pdValue) const;
	virtual XnStatus GetProperty(const XnChar* strName, XnChar* csValue) const;
	virtual XnStatus GetProperty(const XnChar* strName, const XnGeneralBuffer& gbValue) const;

	virtual XnStatus SetProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetProperty(const XnChar* strName, XnDouble dValue);
	virtual XnStatus SetProperty(const XnChar* strName, const XnChar* strValue);
	virtual XnStatus SetProperty(const XnChar* strName, const XnGeneralBuffer& gbValue);

	virtual XnStatus UnsafeUpdateProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus UnsafeUpdateProperty(const XnChar* strName, XnDouble dValue);
	virtual XnStatus UnsafeUpdateProperty(const XnChar* strName, const XnChar* strValue);
	virtual XnStatus UnsafeUpdateProperty(const XnChar* strName, const XnGeneralBuffer& gbValue);

	XnStatus GetAllProperties(XnPropertySet* pSet) const;

	XnStatus RegisterForOnPropertyValueChanged(const XnChar* strName, XnProperty::OnValueChangedHandler pFunc, void* pCookie, XnCallbackHandle* pHandle);
	XnStatus UnregisterFromOnPropertyValueChanged(const XnChar* strName, XnCallbackHandle hCallback);

	/**
	* Reads values for all properties in module from an INI file.
	*/
	XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* strSectionName = NULL);

	virtual XnStatus BatchConfig(const XnActualPropertiesHash& props);
	virtual XnStatus UnsafeBatchConfig(const XnActualPropertiesHash& props);

	XnStatus GetProperty(const XnChar* strName, XnIntProperty** ppIntProperty) const;
	XnStatus GetProperty(const XnChar* strName, XnRealProperty** ppRealProperty) const;
	XnStatus GetProperty(const XnChar* strName, XnStringProperty** ppStringProperty) const;
	XnStatus GetProperty(const XnChar* strName, XnGeneralProperty** ppGeneralProperty) const;

private:
	XnStatus GetPropertyImpl(const XnChar* Name, XnPropertyType Type, XnProperty** ppProperty) const;

	XnStatus SetLockState(XnBool bLocked);

	static XnStatus XN_CALLBACK_TYPE SetLockStateCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	XnChar m_strName[XN_DEVICE_MAX_STRING_LENGTH];

	XnPropertiesHash m_Properties;
	XnActualIntProperty m_Lock;
	XN_CRITICAL_SECTION_HANDLE m_hLockCS;
};

#define XN_VALIDATE_ADD_PROPERTIES(pModule, ...)	\
	do {	\
		XnProperty* _aProps[] = { __VA_ARGS__ };	\
		XnStatus _nRetVal = (pModule)->AddProperties(_aProps, sizeof(_aProps)/sizeof(XnProperty*));	\
		XN_IS_STATUS_OK(_nRetVal);	\
	} while (0)


#endif //__XN_DEVICE_MODULE_H__
