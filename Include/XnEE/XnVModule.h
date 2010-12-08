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

	XnStatus RegisterForPropertyChangedEvent(const XnChar* strProperty, XnVModulePropertyChangedHandler* pHandler, XnCallbackHandle* phCallback);
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
