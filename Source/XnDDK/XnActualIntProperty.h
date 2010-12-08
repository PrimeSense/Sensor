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





#ifndef __XN_ACTUAL_INT_PROPERTY_H__
#define __XN_ACTUAL_INT_PROPERTY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnIntProperty.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* A property of type integer.
*/
class XN_DDK_CPP_API XnActualIntProperty : public XnIntProperty
{
public:
	XnActualIntProperty(const XnChar* strName, XnUInt64 nInitialValue = 0, const XnChar* strModule = "");

	inline XnUInt64 GetValue() const { return m_nValue; }

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnActualIntProperty* pSender, XnUInt64* pnValue, void* pCookie);

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		XnIntProperty::UpdateSetCallback((XnIntProperty::SetFuncPtr)pFunc, pCookie);
	}

	inline void UpdateSetCallbackToDefault()
	{
		UpdateSetCallback(SetCallback, this);
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		XnIntProperty::UpdateGetCallback((XnIntProperty::GetFuncPtr)pFunc, pCookie);
	}

private:
	static XnStatus XN_CALLBACK_TYPE SetCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCallback(const XnActualIntProperty* pSender, XnUInt64* pnValue, void* pCookie);

	XnUInt64 m_nValue;
};

#endif //__XN_ACTUAL_INT_PROPERTY_H__
