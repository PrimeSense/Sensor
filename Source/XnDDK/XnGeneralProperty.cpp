/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.x Alpha                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Sensor                                    *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
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

XnStatus XnGeneralProperty::AddToPropertySet(XnPropertySet* /*pSet*/)
{
	return (XN_STATUS_ERROR);
}
