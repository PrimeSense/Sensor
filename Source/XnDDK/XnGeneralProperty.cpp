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
#include "XnGeneralProperty.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnGeneralProperty::XnGeneralProperty(const XnChar* strName, XnGeneralBuffer* pValueHolder /* = NULL */, ReadValueFromFileFuncPtr pReadFromFileFunc /* = NULL */, const XnChar* strModule /* = "" */ ) :
	XnProperty(XN_PROPERTY_TYPE_GENERAL, pValueHolder, strName, strModule),
	m_pReadFromFileFunc(pReadFromFileFunc)
{
}

XnStatus XnGeneralProperty::CopyValueImpl(void* pDest, const void* pSource) const 
{
	return XnGeneralBufferCopy((XnGeneralBuffer*)pDest, (const XnGeneralBuffer*)pSource);
}

XnBool XnGeneralProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	const XnGeneralBuffer* pgb1 = (const XnGeneralBuffer*)pValue1;
	const XnGeneralBuffer* pgb2 = (const XnGeneralBuffer*)pValue2;

	if (pgb1->nDataSize != pgb2->nDataSize)
		return FALSE;

	return (memcmp(pgb1->pData, pgb2->pData, pgb1->nDataSize) == 0);
}

XnStatus XnGeneralProperty::CallSetCallback(XnProperty::SetFuncPtr pFunc, const void* pValue, void* pCookie)
{
	SetFuncPtr pCallback = (SetFuncPtr)pFunc;
	return pCallback(this, *(const XnGeneralBuffer*)pValue, pCookie);
}

XnStatus XnGeneralProperty::CallGetCallback(XnProperty::GetFuncPtr pFunc, void* pValue, void* pCookie) const
{
	GetFuncPtr pCallback = (GetFuncPtr)pFunc;
	return pCallback(this, *(const XnGeneralBuffer*)pValue, pCookie);
}

XnStatus XnGeneralProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_pReadFromFileFunc != NULL)
	{
		nRetVal = m_pReadFromFileFunc(this, csINIFile, csSection);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnGeneralProperty::AddToPropertySet(XnPropertySet* pSet)
{
	return (XN_STATUS_ERROR);
}
