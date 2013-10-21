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
#include "XnMultiPropChangedHandler.h"

//---------------------------------------------------------------------------
// XnMultiPropChangedHandler Helper Class
//---------------------------------------------------------------------------
XnMultiPropChangedHandler::XnMultiPropChangedHandler(XnSensorProductionNode* pNode, const XnChar* strModule) :
	m_pNode(pNode), 
	m_strModule((strModule != NULL) ? strModule : pNode->GetModuleName())
{}

XnMultiPropChangedHandler::~XnMultiPropChangedHandler()
{
	// we cannot assume node or sensor still exist, so don't unregister.
}

XnStatus XnMultiPropChangedHandler::AddProperty(const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnCallbackHandle hCallback;
	nRetVal = m_pNode->GetSensor()->RegisterToPropertyChange(m_strModule, strName, PropertyChangedCallback, this, hCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Registered.Set(strName, hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		m_pNode->GetSensor()->UnregisterFromPropertyChange(m_strModule, strName, hCallback);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnMultiPropChangedHandler::AddProperties(const XnChar** strNames)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar** pstrName = strNames;

	while (*pstrName != NULL)
	{
		nRetVal = AddProperty(*pstrName);
		XN_IS_STATUS_OK(nRetVal);

		++pstrName;
	}

	return (XN_STATUS_OK);
}

void XnMultiPropChangedHandler::Unregister()
{
	for (XnPropertyHandleHash::Iterator it = m_Registered.Begin(); it != m_Registered.End(); ++it)
	{
		m_pNode->GetSensor()->UnregisterFromPropertyChange(m_strModule, it->Key(), it->Value());
	}
}

void XnMultiPropChangedHandler::PropertyChangedCallback(XnDeviceHandle /*pDeviceHandle*/, const XnChar* ModuleName, const XnChar* PropertyName, void* pCookie)
{
	XnMultiPropChangedHandler* pThis = (XnMultiPropChangedHandler*)pCookie;
	pThis->OnModulePropertyChanged(ModuleName, PropertyName);
}

//---------------------------------------------------------------------------
// XnMultiPropStateChangedHandler
//---------------------------------------------------------------------------
XnMultiPropStateChangedHandler::XnMultiPropStateChangedHandler(XnSensorProductionNode* pNode, XnModuleStateChangedHandler handler, void* pCookie, const XnChar* strModule /* = NULL */) :
	XnMultiPropChangedHandler(pNode, strModule),
	m_pHandler(handler), 
	m_pCookie(pCookie)
{}

void XnMultiPropStateChangedHandler::OnModulePropertyChanged(const XnChar* /*strModule*/, const XnChar* /*strProperty*/)
{
	m_pHandler(m_pCookie);
}

