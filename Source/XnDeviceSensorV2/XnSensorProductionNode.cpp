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
#include "XnSensorProductionNode.h"
#include "XnMultiPropChangedHandler.h"

//---------------------------------------------------------------------------
// XnSensorProductionNode class
//---------------------------------------------------------------------------
XnSensorProductionNode::XnSensorProductionNode(xn::Context& context, const XnChar* strInstanceName, XnDeviceBase* pSensor, const XnChar* strModuleName) : 
	m_Context(context),
	m_pSensor(pSensor),
	m_pNotifications(NULL)
{
	strcpy(m_strInstanceName, strInstanceName);
	strcpy(m_strModule, strModuleName);
}

XnSensorProductionNode::~XnSensorProductionNode()
{
	// free all memory allocated for registration, even if client did not unregister from it
	for (XnMultiPropChangedHandlerHash::Iterator it = m_AllHandlers.begin(); it != m_AllHandlers.end(); ++it)
	{
		XN_DELETE(it.Key());
	}
}

XnBool XnSensorProductionNode::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return ((strcmp(strCapabilityName, XN_CAPABILITY_EXTENDED_SERIALIZATION) == 0) ||
		(strcmp(strCapabilityName, XN_CAPABILITY_LOCK_AWARE) == 0));
}

XnStatus XnSensorProductionNode::SetIntProperty(const XnChar* strName, XnUInt64 nValue)
{
	return m_pSensor->SetProperty(m_strModule, strName, nValue);
}

XnStatus XnSensorProductionNode::SetRealProperty(const XnChar* strName, XnDouble dValue)
{
	return m_pSensor->SetProperty(m_strModule, strName, dValue);
}

XnStatus XnSensorProductionNode::SetStringProperty(const XnChar* strName, const XnChar* strValue)
{
	return m_pSensor->SetProperty(m_strModule, strName, strValue);
}

XnStatus XnSensorProductionNode::SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	return m_pSensor->SetProperty(m_strModule, strName, XnGeneralBufferPack((void*)pBuffer, nBufferSize));
}

XnStatus XnSensorProductionNode::GetIntProperty(const XnChar* strName, XnUInt64& nValue) const
{
	return m_pSensor->GetProperty(m_strModule, strName, &nValue);
}

XnStatus XnSensorProductionNode::GetRealProperty(const XnChar* strName, XnDouble& dValue) const
{
	return m_pSensor->GetProperty(m_strModule, strName, &dValue);
}

XnStatus XnSensorProductionNode::GetStringProperty(const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize) const
{
	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	XnStatus nRetVal = m_pSensor->GetProperty(m_strModule, strName, strValue);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = xnOSStrCopy(csValue, strValue, nBufSize);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus XnSensorProductionNode::GetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer) const
{
	return m_pSensor->GetProperty(m_strModule, strName, XnGeneralBufferPack(pBuffer, nBufferSize));
}

XnStatus XnSensorProductionNode::SetLockState(XnBool bLocked)
{
	return m_pSensor->SetProperty(m_strModule, XN_MODULE_PROPERTY_LOCK, (XnUInt64)bLocked);
}

XnBool XnSensorProductionNode::GetLockState()
{
	XnUInt64 nValue = FALSE;
	XnStatus nRetVal = m_pSensor->GetProperty(m_strModule, XN_MODULE_PROPERTY_LOCK, &nValue);
	return (nValue == TRUE);
}

XnStatus XnSensorProductionNode::RegisterToLockChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_MODULE_PROPERTY_LOCK,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorProductionNode::UnregisterFromLockChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

void XnSensorProductionNode::FilterProperties(XnActualPropertiesHash* pHash)
{
	pHash->Remove(XN_MODULE_PROPERTY_LOCK);
}

XnStatus XnSensorProductionNode::NotifyExState(XnNodeNotifications* pNotifications, void* pCookie)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// get all properties
	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	nRetVal = m_pSensor->GetAllProperties(&props, FALSE, GetModuleName());
	XN_IS_STATUS_OK(nRetVal);

	XnActualPropertiesHash* pPropsHash = props.pData->begin().Value();

	// filter properties (remove the ones already exposed as OpenNI interfaces)
	FilterProperties(pPropsHash);

	const XnChar* astrIntProps[200] = {0};
	const XnChar* astrRealProps[200] = {0};
	const XnChar* astrStringProps[200] = {0};
	const XnChar* astrGeneralProps[200] = {0};

	XnUInt32 nIntProps = 0;
	XnUInt32 nRealProps = 0;
	XnUInt32 nStringProps = 0;
	XnUInt32 nGeneralProps = 0;

	// enumerate over properties
	for (XnActualPropertiesHash::Iterator it = pPropsHash->begin(); it != pPropsHash->end(); ++it)
	{
		XnProperty* pProp = it.Value();

		switch (pProp->GetType())
		{
		case XN_PROPERTY_TYPE_INTEGER:
			{
				XnActualIntProperty* pIntProp = (XnActualIntProperty*)pProp;
				pNotifications->OnNodeIntPropChanged(pCookie, GetInstanceName(), pProp->GetName(), pIntProp->GetValue());
				astrIntProps[nIntProps++] = pProp->GetName();
			}
			break;
		case XN_PROPERTY_TYPE_REAL:
			{
				XnActualRealProperty* pRealProp = (XnActualRealProperty*)pProp;
				pNotifications->OnNodeRealPropChanged(pCookie, GetInstanceName(), pProp->GetName(), pRealProp->GetValue());
				astrRealProps[nRealProps++] = pProp->GetName();
			}
			break;
		case XN_PROPERTY_TYPE_STRING:
			{
				XnActualStringProperty* pStrProp = (XnActualStringProperty*)pProp;
				pNotifications->OnNodeStringPropChanged(pCookie, GetInstanceName(), pProp->GetName(), pStrProp->GetValue());
				astrStringProps[nStringProps++] = pProp->GetName();
			}
			break;
		case XN_PROPERTY_TYPE_GENERAL:
			{
				XnActualGeneralProperty* pGenProp = (XnActualGeneralProperty*)pProp;
				pNotifications->OnNodeGeneralPropChanged(pCookie, GetInstanceName(), pProp->GetName(), pGenProp->GetValue().nDataSize, pGenProp->GetValue().pData);
				astrGeneralProps[nGeneralProps++] = pProp->GetName();
			}
			break;
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DEVICE_SENSOR, "Unknown property type: %d", pProp->GetType());
		}
	}

	// TODO: also register to these properties, and if changed, notify.

	// store notifications object
	m_pNotifications = pNotifications;
	m_pCookie = pCookie;

	return (XN_STATUS_OK);
}
void XnSensorProductionNode::UnregisterExNotifications()
{
	// TODO: unregister from props

	// reset notifications object
	m_pNotifications = NULL;
	m_pCookie = NULL;
}

XnStatus XnSensorProductionNode::RegisterToProps(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback, const XnChar** strNames, const XnChar* strModule /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnMultiPropStateChangedHandler* pHandler;
	XN_VALIDATE_NEW(pHandler, XnMultiPropStateChangedHandler, this, handler, pCookie, strModule);

	nRetVal = pHandler->AddProperties(strNames);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pHandler);
		return (nRetVal);
	}

	// register it for later deletion
	m_AllHandlers.Set(pHandler, pHandler);

	hCallback = (XnCallbackHandle)pHandler;

	return (XN_STATUS_OK);
}

void XnSensorProductionNode::UnregisterFromProps(XnCallbackHandle hCallback)
{
	XnMultiPropStateChangedHandler* pHandler = (XnMultiPropStateChangedHandler*)hCallback;
	m_AllHandlers.Remove(pHandler);
	pHandler->Unregister();
	XN_DELETE(pHandler);
}

