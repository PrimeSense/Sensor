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
	if (pProperty->GetType() == XN_PROPERTY_TYPE_GENERAL)
	{
		XnActualGeneralProperty* pGenProp = (XnActualGeneralProperty*)pProperty;
		xnOSFree(pGenProp->GetValue().pData);
	}

	XN_DELETE(pProperty);
	
	return (XN_STATUS_OK);
}