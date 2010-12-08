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
#include "XnActualStringProperty.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnActualStringProperty::XnActualStringProperty(const XnChar* strName, const XnChar* strInitialValue /* = "" */, const XnChar* strModule /* = "" */ ) :
	XnStringProperty(strName, m_strValue, strModule)
{
	strncpy(m_strValue, strInitialValue, XN_DEVICE_MAX_STRING_LENGTH);
	// set a callback for get operations
	UpdateGetCallback(GetCallback, this);
}

XnStatus XnActualStringProperty::SetCallback(XnActualStringProperty* pSender, const XnChar* strValue, void* pCookie)
{
	return pSender->UnsafeUpdateValue(strValue);
}

XnStatus XnActualStringProperty::GetCallback(const XnActualStringProperty* pSender, XnChar* csValue, void* pCookie)
{
	strncpy(csValue, pSender->GetValue(), XN_DEVICE_MAX_STRING_LENGTH);
	return XN_STATUS_OK;
}
