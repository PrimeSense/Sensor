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
#include "XnActualPropertyFactory.h"
#include "XnActualIntProperty.h"
#include "XnActualRealProperty.h"
#include "XnActualStringProperty.h"
#include "XnActualGeneralProperty.h"
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnActualPropertyFactory::CreateProperty(XnPropertyType nType, const XnChar* strName, XnProperty** ppProperty, XnUInt32 nSize /* = 0 */)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	switch (nType)
	{
	case XN_PROPERTY_TYPE_INTEGER:
		XN_VALIDATE_NEW(*ppProperty, XnActualIntProperty, strName);
		break;
	case XN_PROPERTY_TYPE_REAL:
		XN_VALIDATE_NEW(*ppProperty, XnActualRealProperty, strName);
		break;
	case XN_PROPERTY_TYPE_STRING:
		XN_VALIDATE_NEW(*ppProperty, XnActualStringProperty, strName);
		break;
	case XN_PROPERTY_TYPE_GENERAL:
		XnGeneralBuffer gbValue;
		gbValue.pData = xnOSMalloc(nSize);
		XN_VALIDATE_ALLOC_PTR(gbValue.pData);
		gbValue.nDataSize = nSize;
		XN_VALIDATE_NEW(*ppProperty, XnActualGeneralProperty, strName, gbValue);
		break;
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnActualPropertyFactory::FreeProperty(XnProperty* pProperty)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (pProperty->GetType() == XN_PROPERTY_TYPE_GENERAL)
	{
		XnActualGeneralProperty* pGenProp = (XnActualGeneralProperty*)pProperty;
		xnOSFree(pGenProp->GetValue().pData);
	}

	XN_DELETE(pProperty);
	
	return (XN_STATUS_OK);
}