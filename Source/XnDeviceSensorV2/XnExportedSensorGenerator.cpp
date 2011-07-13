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






//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnExportedSensorGenerator.h"
#include "XnExportedSensorDevice.h"
#include <XnOpenNI.h>
#include "XnSensorGenerator.h"
#include <XnCommon/XnCommon.h>

//---------------------------------------------------------------------------
// XnExportedSensorGenerator class
//---------------------------------------------------------------------------
XnExportedSensorGenerator::XnExportedSensorGenerator(XnProductionNodeType Type, const XnChar* strStreamType, XnBool isAvailbaleInLowBand /* = TRUE */) : 
	m_Type(Type),
	m_bIsAvailableInLowBand(isAvailbaleInLowBand)
{
	strcpy(m_strStreamType, strStreamType);
}

void XnExportedSensorGenerator::GetDescription(XnProductionNodeDescription* pDescription)
{
	XnExportedSensorDevice::FillCommonDescriptionFields(pDescription);
	pDescription->Type = m_Type;
}

XnStatus XnExportedSensorGenerator::EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnProductionNodeDescription Description;
	GetDescription(&Description);

	// perform a query to be sure the device we'll find is the same one exported from this DLL.
	xn::Query query;
	query.SetVendor(XN_VENDOR_PRIMESENSE);
	query.SetName(XN_DEVICE_NAME);
	query.SetMinVersion(Description.Version);
	query.SetMaxVersion(Description.Version);

	nRetVal = context.AutoEnumerateOverSingleInput(TreesList, Description, NULL, XN_NODE_TYPE_DEVICE, pErrors, &query);
	XN_IS_STATUS_OK(nRetVal);

	if (!m_bIsAvailableInLowBand)
	{
		xn::NodeInfoList::Iterator it = TreesList.Begin();
		while (it != TreesList.End())
		{
			xn::NodeInfoList::Iterator curr = it;
			it++;

			xn::NodeInfo node = *curr;

			// take sensor node
			xn::NodeInfo sensorNode = *node.GetNeededNodes().Begin();

			XnBool bIsLowBand;
			nRetVal = XnSensorIO::IsSensorLowBandwidth(sensorNode.GetCreationInfo(), &bIsLowBand);
			XN_IS_STATUS_OK(nRetVal);

			if (bIsLowBand)
			{
				// sensor is low band
				nRetVal = TreesList.Remove(curr);
				XN_IS_STATUS_OK(nRetVal);
			}
		}

		if (TreesList.IsEmpty())
		{
			return XN_STATUS_NO_NODE_PRESENT;
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnExportedSensorGenerator::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// needed trees should contain a device node
	if (pNeededTrees == NULL || pNeededTrees->Begin() == pNeededTrees->End())
	{
		return XN_STATUS_MISSING_NEEDED_TREE;
	}

	xn::NodeInfo deviceInfo = *(pNeededTrees->Begin());
	if (deviceInfo.GetDescription().Type != XN_NODE_TYPE_DEVICE)
	{
		return XN_STATUS_MISSING_NEEDED_TREE;
	}

	// Get the sensor instance
	xn::Device device;
	nRetVal = deviceInfo.GetInstance(device);
	XN_IS_STATUS_OK(nRetVal);

	XnDeviceBase* pSensor = NULL;
	nRetVal = device.GetGeneralProperty(XN_SENSOR_PROPERTY_INSTANCE_POINTER, sizeof(XnDeviceBase*), &pSensor);
	XN_IS_STATUS_OK(nRetVal);

	// create stream
	nRetVal = pSensor->CreateStream(m_strStreamType, strInstanceName);
	XN_IS_STATUS_OK(nRetVal);

	// create generator
	XnSensorGenerator* pGenerator = CreateGenerator(context, device, pSensor, strInstanceName);
	if (pGenerator == NULL)
	{
		pSensor->DestroyStream(strInstanceName);
		return (XN_STATUS_ALLOC_FAILED);
	}

	// and initialize it
	nRetVal = pGenerator->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		pSensor->DestroyStream(strInstanceName);
		XN_DELETE(pGenerator);
		return (nRetVal);
	}

	*ppInstance = pGenerator;

	return (XN_STATUS_OK);
}

void XnExportedSensorGenerator::Destroy(xn::ModuleProductionNode* pInstance)
{
	XnSensorGenerator* pGenerator = dynamic_cast<XnSensorGenerator*>(pInstance);
	pGenerator->GetSensor()->DestroyStream(pGenerator->GetModuleName());
	XN_DELETE(pGenerator);
}

