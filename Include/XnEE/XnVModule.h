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
#ifndef _XNV_MODULE_H_
#define _XNV_MODULE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include "XnVStreamContainer.h"
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * A module represents a number of parameters under the same title.
 * Module and Property names are available in XnStreamParams.
 */
class XN_EE_CORE_API XnVModule
{
public:
	XnVModule();
	XnVModule(const XnVModule& other);

	XnVModule& operator=(const XnVModule& other);

	virtual ~XnVModule();

	/**
	* Attach to a certain module in a certain container.
	* Module names are 
	*
	* @param	[in]	pStreamContainer	The container (device) to attach
	* @param	[in]	strName				Name of the module.
	*/
	XnStatus Attach(XnVStreamContainer* pStreamContainer, const XnChar* strName);
	/**
	* Detach from the device.
	*/
	XnStatus Detach();

	/**
	* Get a 'sibling module' - another module from the same device.
	*
	* @param	[in]	strName	Name of the other module
	* @param	[out]	module	The other module
	*/
	XnStatus GetOtherModule(const XnChar* strName, XnVModule& module);

	const XnChar* GetName() const;

	XnVStreamContainer* GetCreator();
	const XnVStreamContainer* GetCreator() const;

	/**
	* Get a certain property from the module.
	*
	* @param	[in]	strProperty	Name of the property
	* @param	[out]	nValue		Value of the property
	*/
	XnStatus GetProperty(const XnChar* strProperty, XnUInt64& nValue) const;
	XnStatus GetProperty(const XnChar* strProperty, XnDouble& dValue) const;
	XnStatus GetProperty(const XnChar* strProperty, XnChar* csValue) const;
	XnStatus GetProperty(const XnChar* strProperty, const XnGeneralBuffer& gbValue) const;

	XnStatus GetAllProperties(XnVPropertySet& PropertySet);

	/**
	* Set a certain property to the module.
	*
	* @param	[in]	strProperty	Name of the property
	* @param	[in]	nValue		Value of the property
	*/
	XnStatus SetProperty(const XnChar* strProperty, XnUInt64 nValue);
	XnStatus SetProperty(const XnChar* strProperty, XnDouble fValue);
	XnStatus SetProperty(const XnChar* strProperty, const XnChar* strValue);
	XnStatus SetProperty(const XnChar* strProperty, const XnGeneralBuffer& gbValue);

	XnStatus RegisterForPropertyChangedEvent(const XnChar* strProperty, XnVModulePropertyChangedHandler* pHandler, XnCallbackHandle& hCallback);
	XnStatus UnregisterFromPropertyChangedEvent(const XnChar* strProperty, XnCallbackHandle hCallback);

	XN_3_6_API XnStatus GetProperty(const XnChar* strProperty, XnUInt8& nValue) const;
	XN_3_6_API XnStatus GetProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetProperty(const XnChar* strProperty, XnInt32& nValue) const;
	XN_3_6_API XnStatus GetProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetProperty(const XnChar* strProperty, XnFloat& fValue) const;
	XN_3_6_API XnStatus GetProperty(const XnChar* strProperty, void*& pValue) const;

	XN_3_6_API XnStatus SetProperty(const XnChar* strProperty, XnUInt8 nValue);
	XN_3_6_API XnStatus SetProperty(const XnChar* strProperty, XnUInt16 nValue);
	XN_3_6_API XnStatus SetProperty(const XnChar* strProperty, XnInt32 nValue);
	XN_3_6_API XnStatus SetProperty(const XnChar* strProperty, XnUInt32 nValue);
	XN_3_6_API XnStatus SetProperty(const XnChar* strProperty, XnFloat fValue);
	XN_3_6_API XnStatus SetProperty(const XnChar* strProperty, void* pValue);

protected:
	XnVStreamContainer* m_pStreamContainer;

	XnChar* m_strModule;
	XnBool m_bAttached;
};

#endif // XNV_MODULE_H
