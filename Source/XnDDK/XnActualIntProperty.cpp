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
#include "XnActualIntProperty.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnActualIntProperty::XnActualIntProperty(const XnChar* strName, XnUInt64 nInitialValue /* = 0 */, const XnChar* strModule /* = "" */) :
	XnIntProperty(strName, &m_nValue, strModule),
	m_nValue(nInitialValue)
{
	// set a callback for get operations
	UpdateGetCallback(GetCallback, this);
}

XnStatus XN_CALLBACK_TYPE XnActualIntProperty::SetCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	return pSender->UnsafeUpdateValue(nValue);
}

XnStatus XN_CALLBACK_TYPE XnActualIntProperty::GetCallback(const XnActualIntProperty* pSender, XnUInt64* pnValue, void* pCookie)
{
	*pnValue = pSender->GetValue();
	return XN_STATUS_OK;
}
