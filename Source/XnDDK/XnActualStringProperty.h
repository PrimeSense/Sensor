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





#ifndef __XN_ACTUAL_STRING_PROPERTY_H__
#define __XN_ACTUAL_STRING_PROPERTY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnStringProperty.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type general.
*/
class XN_DDK_CPP_API XnActualStringProperty : public XnStringProperty
{
public:
	XnActualStringProperty(const XnChar* strName, const XnChar* strInitialValue = "", const XnChar* strModule = "");

	inline const XnChar* GetValue() const { return m_strValue; }

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnActualStringProperty* pSender, const XnChar* strValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnActualStringProperty* pSender, XnChar* csValue, void* pCookie);

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		XnProperty::UpdateSetCallback((XnProperty::SetFuncPtr)pFunc, pCookie);
	}

	inline void UpdateSetCallbackToDefault()
	{
		UpdateSetCallback(SetCallback, this);
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		XnProperty::UpdateGetCallback((XnProperty::GetFuncPtr)pFunc, pCookie);
	}

private:
	static XnStatus XN_CALLBACK_TYPE SetCallback(XnActualStringProperty* pSender, const XnChar* strValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCallback(const XnActualStringProperty* pSender, XnChar* csValue, void* pCookie);

	XnChar m_strValue[XN_DEVICE_MAX_STRING_LENGTH];
};

#endif //__XN_ACTUAL_STRING_PROPERTY_H__
