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
#include "XnDeviceModule.h"
#include <XnDDKStatus.h>
#include <XnMacros.h>
#include <XnLog.h>
#include "XnActualIntProperty.h"
#include "XnActualRealProperty.h"
#include "XnActualStringProperty.h"
#include "XnActualGeneralProperty.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDeviceModule::XnDeviceModule(const XnChar* strName) :
	m_Lock(XN_MODULE_PROPERTY_LOCK, FALSE, strName),
	m_hLockCS(NULL)
{
	strncpy(m_strName, strName, XN_DEVICE_MAX_STRING_LENGTH);
	m_Lock.UpdateSetCallback(SetLockStateCallback, this);
}

XnDeviceModule::~XnDeviceModule()
{
	Free();
}

XnStatus XnDeviceModule::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = AddProperty(&m_Lock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hLockCS);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::Free()
{
	xnOSCloseCriticalSection(&m_hLockCS);
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::AddProperty(XnProperty* pProperty)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// make sure another property with this name doesn't exist
	XnPropertiesHash::Iterator it = m_Properties.end();
	if (XN_STATUS_NO_MATCH != m_Properties.Find(pProperty->GetName(), it))
		return XN_STATUS_DEVICE_PROPERTY_ALREADY_EXISTS;

	nRetVal = m_Properties.Set(pProperty->GetName(), pProperty);
	XN_IS_STATUS_OK(nRetVal);

	pProperty->UpdateName(GetName(), pProperty->GetName());

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::AddProperties(XnProperty** apProperties, XnUInt32 nCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		nRetVal = AddProperty(apProperties[i]);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::DoesPropertyExist(const XnChar* strName, XnBool* pbDoesExist) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	*pbDoesExist = FALSE;

	XnPropertiesHash::ConstIterator it = m_Properties.end();
	nRetVal = m_Properties.Find(strName, it);
	if (nRetVal != XN_STATUS_NO_MATCH && nRetVal != XN_STATUS_OK)
	{
		return (nRetVal);
	}

	*pbDoesExist = (nRetVal == XN_STATUS_OK);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::GetPropertyType(const XnChar* strName, XnPropertyType* pnType) const
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	*pnType = pProp->GetType();
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::GetPropertyImpl(const XnChar* Name, XnPropertyType Type, XnProperty** ppProperty) const
{
	*ppProperty = NULL;

	XnProperty* pProperty;
	if (XN_STATUS_NO_MATCH == m_Properties.Get(Name, pProperty))
	{
		return XN_STATUS_DEVICE_PROPERTY_DONT_EXIST;
	}

	if (pProperty->GetType() != Type)
	{
		return XN_STATUS_DEVICE_PROPERTY_BAD_TYPE;
	}

	*ppProperty = pProperty;
	return XN_STATUS_OK;
}

XnStatus XnDeviceModule::GetProperty(const XnChar* Name, XnProperty **ppProperty) const
{
	XnProperty* pProperty;
	if (XN_STATUS_NO_MATCH == m_Properties.Get(Name, pProperty))
	{
		return XN_STATUS_DEVICE_PROPERTY_DONT_EXIST;
	}

	*ppProperty = pProperty;

	return XN_STATUS_OK;
}

XnStatus XnDeviceModule::GetProperty(const XnChar* strName, XnUInt64* pnValue) const
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnIntProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->GetValue(pnValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::GetProperty(const XnChar* strName, XnDouble* pdValue) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnRealProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->GetValue(pdValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::GetProperty(const XnChar* strName, XnChar* csValue) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnStringProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->GetValue(csValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);

}

XnStatus XnDeviceModule::GetProperty(const XnChar* strName, const XnGeneralBuffer& gbValue) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnGeneralProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->GetValue(gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::SetProperty(const XnChar* strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnIntProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->SetValue(nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::SetProperty(const XnChar* strName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnRealProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->SetValue(dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::SetProperty(const XnChar* strName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnStringProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->SetValue(strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::SetProperty(const XnChar* strName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnGeneralProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->SetValue(gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::UnsafeUpdateProperty(const XnChar* strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnIntProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->UnsafeUpdateValue(nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::UnsafeUpdateProperty(const XnChar* strName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnRealProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->UnsafeUpdateValue(dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::UnsafeUpdateProperty(const XnChar* strName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnStringProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->UnsafeUpdateValue(strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::UnsafeUpdateProperty(const XnChar* strName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnGeneralProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->UnsafeUpdateValue(gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::RegisterForOnPropertyValueChanged(const XnChar* strName, XnProperty::OnValueChangedHandler pFunc, void* pCookie, XnCallbackHandle* pHandle)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->OnChangeEvent().Register(pFunc, pCookie, pHandle);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::UnregisterFromOnPropertyValueChanged(const XnChar* strName, XnCallbackHandle hCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnProperty* pProp;
	nRetVal = GetProperty(strName, &pProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pProp->OnChangeEvent().Unregister(hCallback);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::GetProperty(const XnChar* Name, XnIntProperty **ppIntProperty) const
{
	return GetPropertyImpl(Name, XN_PROPERTY_TYPE_INTEGER, (XnProperty**)ppIntProperty);
}

XnStatus XnDeviceModule::GetProperty(const XnChar* Name, XnRealProperty **ppRealProperty) const
{
	return GetPropertyImpl(Name, XN_PROPERTY_TYPE_REAL, (XnProperty**)ppRealProperty);
}

XnStatus XnDeviceModule::GetProperty(const XnChar* Name, XnStringProperty **ppStringProperty) const
{
	return GetPropertyImpl(Name, XN_PROPERTY_TYPE_STRING, (XnProperty**)ppStringProperty);
}

XnStatus XnDeviceModule::GetProperty(const XnChar* Name, XnGeneralProperty **ppPtrProperty) const
{
	return GetPropertyImpl(Name, XN_PROPERTY_TYPE_GENERAL, (XnProperty**)ppPtrProperty);
}

XnStatus XnDeviceModule::GetAllProperties(XnPropertySet* pSet) const
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// add the module
	nRetVal = XnPropertySetAddModule(pSet, GetName());
	XN_IS_STATUS_OK(nRetVal);

	// now add all properties
	for (XnPropertiesHash::ConstIterator it = m_Properties.begin(); it != m_Properties.end(); ++it)
	{
		XnProperty* pProperty = it.Value();

		if (pProperty->IsActual())
		{
			nRetVal = pProperty->AddToPropertySet(pSet);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* strSectionName /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strSectionName == NULL)
	{
		strSectionName = GetName();
	}

	xnLogVerbose(XN_MASK_DDK, "Configuring module '%s' from section '%s' in file '%s'...", GetName(), strSectionName, csINIFilePath);

	for (XnPropertiesHash::Iterator it = m_Properties.begin(); it != m_Properties.end(); ++it)
	{
		XnProperty* pProp = it.Value();

		// only read writable properties
		if (!pProp->IsReadOnly())
		{
			nRetVal = pProp->ReadValueFromFile(csINIFilePath, strSectionName);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	xnLogInfo(XN_MASK_DDK, "Module '%s' configuration was loaded from file.", GetName());

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::BatchConfig(const XnActualPropertiesHash& props)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	for (XnActualPropertiesHash::ConstIterator it = props.begin(); it != props.end(); ++it)
	{
		XnProperty* pRequestProp = it.Value();
		switch (pRequestProp->GetType())
		{
		case XN_PROPERTY_TYPE_INTEGER:
			{
				XnActualIntProperty* pProp = (XnActualIntProperty*)pRequestProp;
				nRetVal = SetProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_REAL:
			{
				XnActualRealProperty* pProp = (XnActualRealProperty*)pRequestProp;
				nRetVal = SetProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_STRING:
			{
				XnActualStringProperty* pProp = (XnActualStringProperty*)pRequestProp;
				nRetVal = SetProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_GENERAL:
			{
				XnActualGeneralProperty* pProp = (XnActualGeneralProperty*)pRequestProp;
				nRetVal = SetProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Unknown property type: %d\n", pRequestProp->GetType());
		} // type switch
	} // props loop
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::UnsafeBatchConfig(const XnActualPropertiesHash& props)
{
	XnStatus nRetVal = XN_STATUS_OK;

	for (XnActualPropertiesHash::ConstIterator it = props.begin(); it != props.end(); ++it)
	{
		XnProperty* pRequestProp = it.Value();
		switch (pRequestProp->GetType())
		{
		case XN_PROPERTY_TYPE_INTEGER:
			{
				XnActualIntProperty* pProp = (XnActualIntProperty*)pRequestProp;
				nRetVal = UnsafeUpdateProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_REAL:
			{
				XnActualRealProperty* pProp = (XnActualRealProperty*)pRequestProp;
				nRetVal = UnsafeUpdateProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_STRING:
			{
				XnActualStringProperty* pProp = (XnActualStringProperty*)pRequestProp;
				nRetVal = UnsafeUpdateProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_GENERAL:
			{
				XnActualGeneralProperty* pProp = (XnActualGeneralProperty*)pRequestProp;
				nRetVal = UnsafeUpdateProperty(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Unknown property type: %d\n", pRequestProp->GetType());
		} // type switch
	} // props loop

	return (XN_STATUS_OK);
}

XnStatus XnDeviceModule::SetLockState(XnBool bLocked)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (bLocked && m_Lock.GetValue() == TRUE)
	{
		return XN_STATUS_NODE_IS_LOCKED;
	}

	xnOSEnterCriticalSection(&m_hLockCS);

	// check again
	if (bLocked && m_Lock.GetValue() == TRUE)
	{
		xnOSLeaveCriticalSection(&m_hLockCS);
		return XN_STATUS_NODE_IS_LOCKED;
	}

	nRetVal = m_Lock.UnsafeUpdateValue(bLocked);
	xnOSLeaveCriticalSection(&m_hLockCS);

	return (nRetVal);
}

XnStatus XnDeviceModule::SetLockStateCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDeviceModule* pThis = (XnDeviceModule*)pCookie;
	return pThis->SetLockState(nValue != FALSE);
}
