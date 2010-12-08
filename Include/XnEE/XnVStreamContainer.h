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






#ifndef _XNV_STREAM_CONTAINER_H_
#define _XNV_STREAM_CONTAINER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include "XnVStatus.h"
#include "XnVPropertySet.h"
#include <XnGeneralBuffer.h>
#include "XnVEventHandlers.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is an interface for any class that holds Streams and Modules.
 * Currently implemented a single subclass - XnVDevice.
 */
class XN_EE_CORE_API XnVStreamContainer
{
public:
	virtual ~XnVStreamContainer() {}

	virtual XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64& nValue) const = 0;
	virtual XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnDouble& fValue) const = 0;

	virtual XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64 nValue) = 0;
	virtual XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnDouble fValue) = 0;

	virtual XnStatus DoesModuleExist(const XnChar* strModule, XnBool* pbDoesExist) const = 0;
	virtual XnStatus DoesPropertyExist(const XnChar* strModule, const XnChar* strProperty, XnBool* pbDoesExist) const = 0;

	virtual XnStatus GetAllProperties(XnVPropertySet& PropertySet, XnBool bNoStreams = FALSE, const XnChar* strModule = NULL) const = 0;

	virtual XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnChar* pcsValue) const = 0;
	virtual XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, const XnGeneralBuffer& gbValue) const = 0;
	virtual XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, const XnChar* csValue) = 0;
	virtual XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, const XnGeneralBuffer& gbValue) = 0;

	virtual XnStatus RegisterForPropertyChangedEvent(const XnChar* strModule, const XnChar* strProperty, XnVModulePropertyChangedHandler* pHandler, XnCallbackHandle* phCallback) = 0;
	virtual XnStatus RegisterForStreamCollectionChangedEvent(XnVStreamCollectionChangedHandler* pHandler, XnCallbackHandle* phCallback) = 0;
	virtual XnStatus UnregisterFromPropertyChangedEvent(const XnChar* strModule, const XnChar* strProperty, XnCallbackHandle hCallback) = 0;
	virtual XnStatus UnregisterFromStreamCollectionChangedEvent(XnCallbackHandle hCallback) = 0;

	XN_3_6_API virtual XnBool IsModuleExist(const XnChar* strModule) = 0;
	XN_3_6_API virtual XnBool IsPropertyExist(const XnChar* strModule, const XnChar* strProperty) = 0;
	XN_3_6_API virtual XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt16& nValue) const = 0;
	XN_3_6_API virtual XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt32& nValue) const = 0;
	XN_3_6_API virtual XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, void*& pValue) const = 0;
	XN_3_6_API virtual XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt16 nValue) = 0;
	XN_3_6_API virtual XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt32 nValue) = 0;
	XN_3_6_API virtual XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, void* pValue) = 0;
};

#endif // _XNV_STREAM_CONTAINER_H_
