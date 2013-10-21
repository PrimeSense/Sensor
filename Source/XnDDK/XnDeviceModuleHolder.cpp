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
#include "XnDeviceModuleHolder.h"
#include "XnActualIntProperty.h"
#include "XnActualRealProperty.h"
#include "XnActualStringProperty.h"
#include "XnActualGeneralProperty.h"

XnDeviceModuleHolder::XnDeviceModuleHolder(XnDeviceModule* pModule, XnBool bAllowNewProps /* = FALSE */) :
	m_pModule(pModule),
	m_bAllowNewProps(bAllowNewProps)
{}

XnDeviceModuleHolder::~XnDeviceModuleHolder()
{
	XnDeviceModuleHolder::Free();
}

XnStatus XnDeviceModuleHolder::Init(const XnActualPropertiesHash* pInitialValues) 
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_pModule->Init();
	XN_IS_STATUS_OK(nRetVal);

	if (pInitialValues != NULL)
	{
		if (m_bAllowNewProps)
		{
			// clone all properties and add them to the device
			nRetVal = UnsafeSetProperties(*pInitialValues);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			nRetVal = m_pModule->BatchConfig(*pInitialValues);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModuleHolder::Free() 
{
	// free all allocated properties (we assume we're part of destruction, so that no need to remove
	// them from Module).
	while (!m_Allocated.IsEmpty())
	{
		XnProperty* pProp = *(m_Allocated.Begin());
		m_Allocated.Remove(m_Allocated.Begin());
		XN_DELETE(pProp);
	}

	return XN_STATUS_OK;
}

XnStatus XnDeviceModuleHolder::UnsafeSetProperties(const XnActualPropertiesHash& props)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	for (XnActualPropertiesHash::ConstIterator it = props.Begin(); it != props.End(); ++it)
	{
		XnProperty* pRequestProp = it->Value();
		XnProperty* pProp = NULL;

		// check if property already exist
		nRetVal = m_pModule->GetProperty(pRequestProp->GetName(), &pProp);
		if (nRetVal == XN_STATUS_DEVICE_PROPERTY_DONT_EXIST)
		{
			// property doesn't exist. create it now
			nRetVal = CreateProperty(pRequestProp);
			XN_IS_STATUS_OK(nRetVal);
		}
		else if (nRetVal == XN_STATUS_OK)
		{
			// property exists. Change its value
			nRetVal = UnsafeSetProperty(pRequestProp, pProp);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			// error
			return (nRetVal);
		}

	} // props loop
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModuleHolder::CreateProperty(XnProperty* pRequestProp)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnProperty* pNewProp = NULL;

	switch (pRequestProp->GetType())
	{
	case XN_PROPERTY_TYPE_INTEGER:
		{
			XnActualIntProperty* pProp = (XnActualIntProperty*)pRequestProp;
			XN_VALIDATE_NEW(pNewProp, XnActualIntProperty, pProp->GetName(), pProp->GetValue());
			break;
		}
	case XN_PROPERTY_TYPE_REAL:
		{
			XnActualRealProperty* pProp = (XnActualRealProperty*)pRequestProp;
			XN_VALIDATE_NEW(pNewProp, XnActualRealProperty, pProp->GetName(), pProp->GetValue());
			break;
		}
	case XN_PROPERTY_TYPE_STRING:
		{
			XnActualStringProperty* pProp = (XnActualStringProperty*)pRequestProp;
			XN_VALIDATE_NEW(pNewProp, XnActualStringProperty, pProp->GetName(), pProp->GetValue());
			break;
		}
	case XN_PROPERTY_TYPE_GENERAL:
		{
			XnActualGeneralProperty* pProp = (XnActualGeneralProperty*)pRequestProp;

			// create new buffer
			XnGeneralBuffer gbNew;
			nRetVal = XnGeneralBufferAlloc(&gbNew, pProp->GetValue().nDataSize);
			XN_IS_STATUS_OK(nRetVal);

			// copy content
			xnOSMemCopy(gbNew.pData, pProp->GetValue().pData, pProp->GetValue().nDataSize);

			XnActualGeneralProperty* pNewGeneralProp = NULL;
			XN_VALIDATE_NEW(pNewGeneralProp, XnActualGeneralProperty, pProp->GetName(), gbNew);
			pNewGeneralProp->SetAsBufferOwner(TRUE);
			pNewProp = pNewGeneralProp;
			break;
		}
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Unknown property type: %d\n", pRequestProp->GetType());
	} // switch

	// add the property to the module
	nRetVal = m_pModule->AddProperty(pNewProp);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNewProp);
		return (nRetVal);
	}

	// and add it to the list of allocated ones (so we'll delete it afterwards)
	m_Allocated.AddLast(pNewProp);

	return XN_STATUS_OK;
}

XnStatus XnDeviceModuleHolder::UnsafeSetProperty(const XnProperty* pRequest, XnProperty* pProp)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (pRequest->GetType() != pProp->GetType())
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_BAD_TYPE, XN_MASK_DDK, "Property '%s' has the wrong type!", pRequest->GetName());
	}

	switch (pRequest->GetType())
	{
	case XN_PROPERTY_TYPE_INTEGER:
		{
			XnActualIntProperty* pActualRequest = (XnActualIntProperty*)pRequest;
			XnActualIntProperty* pActualProp = (XnActualIntProperty*)pProp;
			nRetVal = pActualProp->UnsafeUpdateValue(pActualRequest->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PROPERTY_TYPE_REAL:
		{
			XnActualRealProperty* pActualRequest = (XnActualRealProperty*)pRequest;
			XnActualRealProperty* pActualProp = (XnActualRealProperty*)pProp;
			nRetVal = pActualProp->UnsafeUpdateValue(pActualRequest->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PROPERTY_TYPE_STRING:
		{
			XnActualStringProperty* pActualRequest = (XnActualStringProperty*)pRequest;
			XnActualStringProperty* pActualProp = (XnActualStringProperty*)pProp;
			nRetVal = pActualProp->UnsafeUpdateValue(pActualRequest->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_PROPERTY_TYPE_GENERAL:
		{
			XnActualGeneralProperty* pActualRequest = (XnActualGeneralProperty*)pRequest;
			XnActualGeneralProperty* pActualProp = (XnActualGeneralProperty*)pProp;
			nRetVal = pActualProp->UnsafeUpdateValue(pActualRequest->GetValue());
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Unknown property type: %d\n", pRequest->GetType());
	} // switch
	
	return (XN_STATUS_OK);
}
