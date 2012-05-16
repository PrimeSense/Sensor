/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
#ifndef __XN_DEVICE_MODULE_HOLDER_H__
#define __XN_DEVICE_MODULE_HOLDER_H__

#include "XnActualPropertiesHash.h"
#include "XnDeviceModule.h"
#include <XnListT.h>

class XN_DDK_CPP_API XnDeviceModuleHolder
{
public:
	/**
	* Creates a new module holder.
	*
	* @param	pModule			[in]	The actual module.
	* @param	bAllowNewProps	[in]	When TRUE, Init() method will create non-existing properties.
	*/
	XnDeviceModuleHolder(XnDeviceModule* pModule, XnBool bAllowNewProps = FALSE);
	virtual ~XnDeviceModuleHolder();

	virtual XnStatus Init(const XnActualPropertiesHash* pInitialValues);

	inline XnDeviceModule* GetModule() const { return m_pModule; }

protected:
	virtual XnStatus Free();

private:
	XnStatus UnsafeSetProperties(const XnActualPropertiesHash& props);
	XnStatus CreateProperty(XnProperty* pRequestProp);
	XnStatus UnsafeSetProperty(const XnProperty* pRequest, XnProperty* pProp);

	XnDeviceModule* m_pModule;
	XnPropertiesList m_Allocated;
	XnBool m_bAllowNewProps;
};

typedef XnListT<XnDeviceModuleHolder*> XnDeviceModuleHolderList;


#endif //__XN_DEVICE_MODULE_HOLDER_H__
