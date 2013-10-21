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

	virtual XnStatus RegisterForPropertyChangedEvent(const XnChar* strModule, const XnChar* strProperty, XnVModulePropertyChangedHandler* pHandler, XnCallbackHandle& hCallback) = 0;
	virtual XnStatus RegisterForStreamCollectionChangedEvent(XnVStreamCollectionChangedHandler* pHandler, XnCallbackHandle& hCallback) = 0;
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
