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
#include "XnIntProperty.h"
#include <XnLog.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnIntProperty::XnIntProperty(const XnChar* strName, XnUInt64* pValueHolder /* = NULL */, const XnChar* strModule /* = "" */ ) :
	XnProperty(XN_PROPERTY_TYPE_INTEGER, pValueHolder, strName, strModule)
{
}

XnStatus XnIntProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt32 nValue;

	nRetVal = xnOSReadIntFromINI(csINIFile, csSection, GetName(), &nValue);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = SetValue(nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnIntProperty::CopyValueImpl(void* pDest, const void* pSource) const
{
	(*(XnUInt64*)pDest) = (*(const XnUInt64*)pSource);
	return XN_STATUS_OK;
}

XnBool XnIntProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	return (*(XnUInt64*)pValue1) == (*(XnUInt64*)pValue2);
}

XnStatus XnIntProperty::CallSetCallback(XnProperty::SetFuncPtr pFunc, const void* pValue, void* pCookie)
{
	SetFuncPtr pCallback = (SetFuncPtr)pFunc;
	return pCallback(this, *(const XnUInt64*)pValue, pCookie);
}

XnStatus XnIntProperty::CallGetCallback(XnProperty::GetFuncPtr pFunc, void* pValue, void* pCookie) const
{
	GetFuncPtr pCallback = (GetFuncPtr)pFunc;
	return pCallback(this, (XnUInt64*)pValue, pCookie);
}

XnBool XnIntProperty::ConvertValueToString(XnChar* csValue, const void* pValue) const
{
	sprintf(csValue, "%llu", *(XnUInt64*)pValue);
	return TRUE;
}

XnStatus XnIntProperty::AddToPropertySet(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt64 nValue;
	nRetVal = GetValue(&nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, GetModule(), GetName(), nValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

