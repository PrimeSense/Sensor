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





#ifndef __XN_GENERAL_PROPERTY_H__
#define __XN_GENERAL_PROPERTY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnProperty.h>
#include <XnGeneralBuffer.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type general.
*/
class XN_DDK_CPP_API XnGeneralProperty : public XnProperty
{
public:
	typedef XnStatus (XN_CALLBACK_TYPE* ReadValueFromFileFuncPtr)(XnGeneralProperty* pSender, const XnChar* csINIFile, const XnChar* csSection);

	XnGeneralProperty(const XnChar* strName, XnGeneralBuffer* pValueHolder = NULL, ReadValueFromFileFuncPtr pReadFromFileFunc = NULL, const XnChar* strModule = "");

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);

	inline XnStatus SetValue(const XnGeneralBuffer& gbValue)
	{
		return XnProperty::SetValue(&gbValue);
	}

	inline XnStatus GetValue(const XnGeneralBuffer& gbValue) const
	{
		return XnProperty::GetValue((void*)&gbValue);
	}

	inline XnStatus UnsafeUpdateValue(const XnGeneralBuffer& gbValue)
	{
		return XnProperty::UnsafeUpdateValue(&gbValue);
	}

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		XnProperty::UpdateSetCallback((XnProperty::SetFuncPtr)pFunc, pCookie);
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		XnProperty::UpdateGetCallback((XnProperty::GetFuncPtr)pFunc, pCookie);
	}

	XnStatus AddToPropertySet(XnPropertySet* pSet);

protected:
	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	virtual XnStatus CopyValueImpl(void* pDest, const void* pSource) const;
	virtual XnBool IsEqual(const void* pValue1, const void* pValue2) const;
	virtual XnStatus CallSetCallback(XnProperty::SetFuncPtr pFunc, const void* pValue, void* pCookie);
	virtual XnStatus CallGetCallback(XnProperty::GetFuncPtr pFunc, void* pValue, void* pCookie) const;
	virtual XnStatus ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection);

private:
	ReadValueFromFileFuncPtr m_pReadFromFileFunc;
};

#endif //__XN_GENERAL_PROPERTY_H__