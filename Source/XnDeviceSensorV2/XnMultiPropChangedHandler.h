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






#ifndef __XN_MULTI_PROP_CHANGED_HANDLER_H__
#define __XN_MULTI_PROP_CHANGED_HANDLER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnStringsHash.h>
#include "XnSensorProductionNode.h"
#include <XnDevice.h>

//---------------------------------------------------------------------------
// XnMultiPropChangedHandler
//---------------------------------------------------------------------------
class XnMultiPropChangedHandler
{
public:
	XnMultiPropChangedHandler(XnSensorProductionNode* pNode, const XnChar* strModule = NULL);
	virtual ~XnMultiPropChangedHandler();

	XnStatus AddProperty(const XnChar* strName);
	XnStatus AddProperties(const XnChar** strNames);

	void Unregister();

protected:
	virtual void OnModulePropertyChanged(const XnChar* strModule, const XnChar* strProperty) = 0;

private:
	static void XN_CALLBACK_TYPE PropertyChangedCallback(XnDeviceHandle pDeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, void* pCookie);

	XN_DECLARE_STRINGS_HASH(XnCallbackHandle, XnPropertyHandleHash);

	XnPropertyHandleHash m_Registered;
	XnSensorProductionNode* m_pNode;
	const XnChar* m_strModule;
};

//---------------------------------------------------------------------------
// XnMultiPropStateChangedHandler
//---------------------------------------------------------------------------
class XnMultiPropStateChangedHandler : public XnMultiPropChangedHandler
{
public:
	XnMultiPropStateChangedHandler(XnSensorProductionNode* pNode, XnModuleStateChangedHandler handler, void* pCookie, const XnChar* strModule = NULL);

protected:
	virtual void OnModulePropertyChanged(const XnChar* strModule, const XnChar* strProperty);

private:
	XnModuleStateChangedHandler m_pHandler;
	void* m_pCookie;
};


#endif // __XN_MULTI_PROP_CHANGED_HANDLER_H__