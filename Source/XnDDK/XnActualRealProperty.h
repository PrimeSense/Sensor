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





#ifndef __XN_ACTUAL_REAL_PROPERTY_H__
#define __XN_ACTUAL_REAL_PROPERTY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnRealProperty.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type integer.
*/
class XN_DDK_CPP_API XnActualRealProperty : public XnRealProperty
{
public:
	XnActualRealProperty(const XnChar* strName, XnDouble dInitialValue = 0.0, const XnChar* strModule = "");

	inline XnDouble GetValue() const { return m_dValue; }

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnActualRealProperty* pSender, XnDouble dValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnActualRealProperty* pSender, XnDouble* pdValue, void* pCookie);

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		XnRealProperty::UpdateSetCallback((XnRealProperty::SetFuncPtr)pFunc, pCookie);
	}

	inline void UpdateSetCallbackToDefault()
	{
		UpdateSetCallback(SetCallback, this);
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		XnRealProperty::UpdateGetCallback((XnRealProperty::GetFuncPtr)pFunc, pCookie);
	}

private:
	static XnStatus XN_CALLBACK_TYPE SetCallback(XnActualRealProperty* pSender, XnDouble dValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCallback(const XnActualRealProperty* pSender, XnDouble* pdValue, void* pCookie);

	XnDouble m_dValue;
};

#endif //__XN_ACTUAL_REAL_PROPERTY_H__
