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
	nRetVal = m_pNode->GetSensor()->RegisterToPropertyChange(m_strModule, strName, PropertyChangedCallback, this, &hCallback);
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
	for (XnPropertyHandleHash::Iterator it = m_Registered.begin(); it != m_Registered.end(); ++it)
	{
		m_pNode->GetSensor()->UnregisterFromPropertyChange(m_strModule, it.Key(), it.Value());
	}
}

void XnMultiPropChangedHandler::PropertyChangedCallback(XnDeviceHandle pDeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, void* pCookie)
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

void XnMultiPropStateChangedHandler::OnModulePropertyChanged(const XnChar* strModule, const XnChar* strProperty)
{
	m_pHandler(m_pCookie);
}

