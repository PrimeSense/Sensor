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
#include "XnActualGeneralProperty.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnActualGeneralProperty::XnActualGeneralProperty(const XnChar* strName, void* pData, XnUInt32 nDataSize, ReadValueFromFileFuncPtr pReadFromFileFunc /* = NULL */, const XnChar* strModule /* = "" */) :
	XnGeneralProperty(strName, &m_gbValue, pReadFromFileFunc, strModule),
	m_gbValue(XnGeneralBufferPack(pData, nDataSize)),
	m_bOwner(FALSE)
{
	// set a callback for get operations
	UpdateGetCallback(GetCallback, this);
}

XnActualGeneralProperty::XnActualGeneralProperty(const XnChar* strName, const XnGeneralBuffer& gbValue, ReadValueFromFileFuncPtr pReadFromFileFunc /* = NULL */, const XnChar* strModule /* = "" */) :
	XnGeneralProperty(strName, &m_gbValue, pReadFromFileFunc, strModule),
	m_gbValue(gbValue),
	m_bOwner(FALSE)
{
	// set a callback for get operations
	UpdateGetCallback(GetCallback, this);
}

XnActualGeneralProperty::~XnActualGeneralProperty()
{
	if (m_bOwner)
	{
		XnGeneralBufferFree(&m_gbValue);
	}
}

void XnActualGeneralProperty::SetAsBufferOwner(XnBool bOwner)
{
	m_bOwner = bOwner;
}

XnStatus XnActualGeneralProperty::SetCallback(XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* /*pCookie*/)
{
	return pSender->UnsafeUpdateValue(gbValue);
}

XnStatus XnActualGeneralProperty::GetCallback(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* /*pCookie*/)
{
	if (gbValue.nDataSize != pSender->GetValue().nDataSize)
	{
		return XN_STATUS_DEVICE_PROPERTY_SIZE_DONT_MATCH;
	}

	xnOSMemCopy(gbValue.pData, pSender->GetValue().pData, pSender->GetValue().nDataSize);
	return XN_STATUS_OK;
}

XnStatus XnActualGeneralProperty::AddToPropertySet(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnPropertySetAddGeneralProperty(pSet, GetModule(), GetName(), &m_gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}
