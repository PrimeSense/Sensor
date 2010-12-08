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






//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnRealProperty.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnRealProperty::XnRealProperty(const XnChar* strName, XnDouble* pValueHolder, const XnChar* strModule /* = "" */) : 
	XnProperty(XN_PROPERTY_TYPE_REAL, pValueHolder, strName, strModule)
{
}

XnStatus XnRealProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDouble dValue;

	nRetVal = xnOSReadDoubleFromINI(csINIFile, csSection, GetName(), &dValue);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = SetValue(dValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnRealProperty::CopyValueImpl(void* pDest, const void* pSource) const
{
	(*(XnDouble*)pDest) = (*(const XnDouble*)pSource);
	return XN_STATUS_OK;
}

XnBool XnRealProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	return (*(XnDouble*)pValue1) == (*(XnDouble*)pValue2);
}

XnStatus XnRealProperty::CallSetCallback(XnProperty::SetFuncPtr pFunc, const void* pValue, void* pCookie)
{
	SetFuncPtr pCallback = (SetFuncPtr)pFunc;
	return pCallback(this, *(const XnDouble*)pValue, pCookie);
}

XnStatus XnRealProperty::CallGetCallback(XnProperty::GetFuncPtr pFunc, void* pValue, void* pCookie) const
{
	GetFuncPtr pCallback = (GetFuncPtr)pFunc;
	return pCallback(this, (XnDouble*)pValue, pCookie);
}

XnBool XnRealProperty::ConvertValueToString(XnChar* csValue, const void* pValue) const
{
	sprintf(csValue, "%f", *(XnDouble*)pValue);
	return TRUE;
}

XnStatus XnRealProperty::AddToPropertySet(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDouble dValue;
	nRetVal = GetValue(&dValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddRealProperty(pSet, GetModule(), GetName(), dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}
