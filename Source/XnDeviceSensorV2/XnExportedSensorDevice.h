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






#ifndef __XN_EXPORTED_SENSOR_DEVICE_H__
#define __XN_EXPORTED_SENSOR_DEVICE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleCppInterface.h>
#include "XnSensor.h"

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
		DeviceKey(XnContext* pContext, const XnChar* strConnStr);
		XnContext* m_pContext;
		XnChar m_strConnStr[XN_MAX_CREATION_INFO_LENGTH]; 
	};


	XN_DECLARE_LIST(DeviceKey, CreatedDevices);
	CreatedDevices m_createdDevices;

	CreatedDevices::Iterator FindCreatedDevice(XnContext* pContext, const XnChar* strConnStr);
};

#endif // __XN_EXPORTED_SENSOR_DEVICE_H__