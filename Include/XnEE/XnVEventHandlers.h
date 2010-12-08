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
