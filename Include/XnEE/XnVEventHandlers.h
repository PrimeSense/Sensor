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
#ifndef __XN_V_EVENT_HANDLERS_H__
#define __XN_V_EVENT_HANDLERS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDevice.h>
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_EE_CORE_API XnVModulePropertyChangedHandler
{
public:
	virtual ~XnVModulePropertyChangedHandler() {}
	virtual void OnModulePropertyChanged(const XnChar* strModule, const XnChar* strProperty) = 0;
};

class XN_EE_CORE_API XnVNewDataFromStreamHandler
{
public:
	virtual ~XnVNewDataFromStreamHandler() {}
	virtual void OnNewDataFromStream(const XnChar* strStream) = 0;
};

class XN_EE_CORE_API XnVStreamCollectionChangedHandler
{
public:
	virtual ~XnVStreamCollectionChangedHandler() {}
	virtual void OnStreamAdded(const XnChar* strStream) = 0;
	virtual void OnStreamRemoved(const XnChar* strStream) = 0;
};

#endif //__XN_V_EVENT_HANDLERS_H__
