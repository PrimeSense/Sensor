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





#ifndef __XN_PROPERTY_H__
#define __XN_PROPERTY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDevice.h>
#include <XnList.h>
#include <XnStringsHash.h>
#include <XnLog.h>
#include <XnEvent.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* A holder for a property (a name and value pair). Note that this class should be inherited, and
* can not be used directly.
*/
class XN_DDK_CPP_API XnProperty
{
public:
	/**
	* Creates a new property. 
	*
	* @param	Type			[in]	Type of the property.
	* @param	pValueHolder	[in]	A pointer to the holder of the value.
	* @param	strName			[in]	Name of the property.
	* @param	strModule		[in]	Name of the module holding this property.
	*/
	XnProperty(XnPropertyType Type, void* pValueHolder, const XnChar* strName, const XnChar* strModule);
	virtual ~XnProperty();

	inline const XnChar* GetName() const { return m_strName; }
	inline const XnChar* GetModule() const { return m_strModule; }
	inline XnBool IsActual() const { return (m_pValueHolder != NULL); }
	inline XnBool IsReadOnly() const { return (m_pGetCallback == NULL); }
	inline XnPropertyType GetType() const { return m_Type; }

	XN_DECLARE_EVENT_1ARG_RETVAL(ChangeEvent, ChangeEventInterface, const XnProperty*, pSender);

	inline ChangeEventInterface& OnChangeEvent() { return m_OnChangeEvent; }

	/** Updates property name. */
	void UpdateName(const XnChar* strModule, const XnChar* strName);

	/** Updates the value of the property according to an INI file. */
	virtual XnStatus ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection) = 0;

	typedef XnStatus (XN_CALLBACK_TYPE* OnValueChangedHandler)(const XnProperty* pSender, void* pCookie);

	/** Adds this property to the property set. */
	virtual XnStatus AddToPropertySet(XnPropertySet* pSet) = 0;

	/** Sets the log severity under which changes to the property are printed. */
	inline void SetLogSeverity(XnInt32 nSeverity) { m_LogSeverity = nSeverity; }

	/** When TRUE, the property will always call the set callback, even if value hasn't changed. */
	inline void SetAlwaysSet(XnBool bAlwaysSet) { m_bAlwaysSet = bAlwaysSet; }

protected:

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnProperty* pSender, const void* pValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnProperty* pSender, void* pValue, void* pCookie);

	/** Sets the property value. */
	XnStatus SetValue(const void* pValue);

	/** Gets the property value. */
	XnStatus GetValue(void* pValue) const;

	/** Updates the value of the property without any checks. */
	XnStatus UnsafeUpdateValue(const void* pValue = NULL);

	/** Updates the set callback. */
	void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie);

	/** Updates the get callback. */
	void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie);

	virtual XnStatus CopyValueImpl(void* pDest, const void* pSource) const = 0;
	virtual XnBool IsEqual(const void* pValue1, const void* pValue2) const = 0;
	virtual XnStatus CallSetCallback(SetFuncPtr pFunc, const void* pValue, void* pCookie) = 0;
	virtual XnStatus CallGetCallback(GetFuncPtr pFunc, void* pValue, void* pCookie) const = 0;
	virtual XnBool ConvertValueToString(XnChar* csValue, const void* pValue) const;

	inline void* Value() const { return m_pValueHolder; }

private:
	XnChar m_strModule[XN_DEVICE_MAX_STRING_LENGTH]; // module name
	XnChar m_strName[XN_DEVICE_MAX_STRING_LENGTH]; // property name
	XnPropertyType m_Type; // property type

	// Set callback
	SetFuncPtr m_pSetCallback; 
	void* m_pSetCallbackCookie;

	// Get callback
	GetFuncPtr m_pGetCallback;
	void* m_pGetCallbackCookie;

	void* m_pValueHolder; // a pointer to the storage of the property

	ChangeEvent m_OnChangeEvent;

	XnInt32 m_LogSeverity;
	XnBool m_bAlwaysSet;
};

/** A property list */
XN_DECLARE_LIST_DECL(XN_DDK_CPP_API, XnProperty*, XnPropertiesList)

/** A hash table, mapping property name to the property */
XN_DECLARE_STRINGS_HASH_DECL(XN_DDK_CPP_API, XnProperty*, XnPropertiesHash)

#endif //__XN_PROPERTY_H__
