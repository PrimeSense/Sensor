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
#ifndef __XN_EXPORTED_SENSOR_DEVICE_H__
#define __XN_EXPORTED_SENSOR_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensor.h"
#include <XnModuleCppInterface.h>
#include <XnListT.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnExportedSensorDevice : public xn::ModuleExportedProductionNode
{
public:
	XnExportedSensorDevice();
	static void FillCommonDescriptionFields(XnProductionNodeDescription* pDescription);
	void GetDescription(XnProductionNodeDescription* pDescription);
	XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors);
	XnStatus Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance);
	void Destroy(xn::ModuleProductionNode* pInstance);

private:
	struct DeviceKey
	{
		DeviceKey() {}
		DeviceKey(XnContext* pContext, const XnChar* strConnStr);

		XnContext* m_pContext;
		XnChar m_strConnStr[XN_MAX_CREATION_INFO_LENGTH]; 
	};

	typedef XnListT<DeviceKey> CreatedDevices;

	CreatedDevices::Iterator FindCreatedDevice(XnContext* pContext, const XnChar* strConnStr);

	CreatedDevices m_createdDevices;
};

#endif // __XN_EXPORTED_SENSOR_DEVICE_H__