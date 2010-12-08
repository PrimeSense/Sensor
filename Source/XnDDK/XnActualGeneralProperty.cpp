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

XnStatus XnActualGeneralProperty::SetCallback(XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie)
{
	return pSender->UnsafeUpdateValue(gbValue);
}

XnStatus XnActualGeneralProperty::GetCallback(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie)
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
