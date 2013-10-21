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
#ifndef __XN_EXPORTED_SENSOR_GENERATOR_H__
#define __XN_EXPORTED_SENSOR_GENERATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleCppInterface.h>
#include "XnSensor.h"
#include "XnSensorGenerator.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnExportedSensorGenerator : public xn::ModuleExportedProductionNode
{
public:
	XnExportedSensorGenerator(XnProductionNodeType Type, const XnChar* strStreamType, XnBool isAvailbaleInLowBand = TRUE);

	void GetDescription(XnProductionNodeDescription* pDescription);
	XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors);
	XnStatus Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance);
	void Destroy(xn::ModuleProductionNode* pInstance);

protected:
	virtual XnStatus IsSupportedForDevice(xn::Context& context, xn::NodeInfo& sensorInfo, XnBool* pbSupported);
	virtual XnSensorGenerator* CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) = 0;

private:
	XnProductionNodeType m_Type;
	XnChar m_strStreamType[XN_DEVICE_MAX_STRING_LENGTH];
	XnBool m_bIsAvailableInLowBand;
};

#endif // __XN_EXPORTED_SENSOR_GENERATOR_H__