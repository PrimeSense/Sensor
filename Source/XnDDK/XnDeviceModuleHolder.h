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
