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
#include "XnStringProperty.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStringProperty::XnStringProperty(const XnChar* strName, XnChar* pValueHolder, const XnChar* strModule /* = "" */) :
	XnProperty(XN_PROPERTY_TYPE_STRING, pValueHolder, strName, strModule)
{
}

XnStatus XnStringProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar csValue[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = xnOSReadStringFromINI(csINIFile, csSection, GetName(), csValue, XN_DEVICE_MAX_STRING_LENGTH);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = SetValue(csValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnStringProperty::CopyValueImpl(void* pDest, const void* pSource) const
{
	strncpy((char*)pDest, (const char*)pSource, XN_DEVICE_MAX_STRING_LENGTH);
	return XN_STATUS_OK;
}

XnBool XnStringProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	return (strncmp((const XnChar*)pValue1, (const XnChar*)pValue2, XN_DEVICE_MAX_STRING_LENGTH) == 0);
}

XnStatus XnStringProperty::CallSetCallback(XnProperty::SetFuncPtr pFunc, const void* pValue, void* pCookie)
{
	SetFuncPtr pCallback = (SetFuncPtr)pFunc;
	return pCallback(this, (const XnChar*)pValue, pCookie);
}

XnStatus XnStringProperty::CallGetCallback(XnProperty::GetFuncPtr pFunc, void* pValue, void* pCookie) const
{
	GetFuncPtr pCallback = (GetFuncPtr)pFunc;
	return pCallback(this, (XnChar*)pValue, pCookie);
}

XnBool XnStringProperty::ConvertValueToString(XnChar* csValue, const void* pValue) const
{
	strncpy(csValue, (const XnChar*)pValue, XN_DEVICE_MAX_STRING_LENGTH);
	return TRUE;
}

XnStatus XnStringProperty::AddToPropertySet(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = GetValue(strValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddStringProperty(pSet, GetModule(), GetName(), strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

