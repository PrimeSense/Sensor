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
#include "XnActualRealProperty.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnActualRealProperty::XnActualRealProperty(const XnChar* strName, XnDouble dInitialValue /* = 0.0 */, const XnChar* strModule /* = "" */) :
	XnRealProperty(strName, &m_dValue, strModule),
	m_dValue(dInitialValue)
{
	// set a callback for get operations
	UpdateGetCallback(GetCallback, this);
}

XnStatus XnActualRealProperty::SetCallback(XnActualRealProperty* pSender, XnDouble dValue, void* pCookie)
{
	return pSender->UnsafeUpdateValue(dValue);
}

XnStatus XN_CALLBACK_TYPE XnActualRealProperty::GetCallback(const XnActualRealProperty* pSender, XnDouble* pdValue, void* pCookie)
{
	*pdValue = pSender->GetValue();
	return XN_STATUS_OK;
}
