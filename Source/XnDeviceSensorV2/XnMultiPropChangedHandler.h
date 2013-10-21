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
#ifndef __XN_MULTI_PROP_CHANGED_HANDLER_H__
#define __XN_MULTI_PROP_CHANGED_HANDLER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnStringsHashT.h>
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

	typedef XnStringsHashT<XnCallbackHandle> XnPropertyHandleHash;

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