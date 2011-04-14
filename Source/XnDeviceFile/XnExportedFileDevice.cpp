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
#include "XnExportedFileDevice.h"
#include <XnPsVersion.h>
#include "XnDeviceFileReader.h"
#include "XnFileDevice.h"
#include <XnOpenNI.h>
#include <XnCommon/XnCommon.h>

//---------------------------------------------------------------------------
// XnExportedFileDevice class
//---------------------------------------------------------------------------
void XnExportedFileDevice::FillCommonDescriptionFields(XnProductionNodeDescription* pDescription)
{
	strcpy(pDescription->strName, XN_DEVICE_NAME);
	strcpy(pDescription->strVendor, XN_VENDOR_PRIMESENSE);

	pDescription->Version.nMajor = XN_PS_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_PS_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_PS_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_PS_BUILD_VERSION;
}

void XnExportedFileDevice::GetDescription(XnProductionNodeDescription* pDescription)
{
	FillCommonDescriptionFields(pDescription);
	pDescription->Type = XN_NODE_TYPE_PLAYER;
}

XnStatus XnExportedFileDevice::EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnProductionNodeDescription Description;
	GetDescription(&Description);
	nRetVal = TreesList.Add(Description, NULL, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnExportedFileDevice::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnFileDevice* pDevice = XN_NEW(XnFileDevice, context, strInstanceName);
	if (pDevice == NULL)
	{
		return (XN_STATUS_ALLOC_FAILED);
	}

	nRetVal = pDevice->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		return (nRetVal);
	}

	*ppInstance = pDevice;

	return (XN_STATUS_OK);
}

void XnExportedFileDevice::Destroy(xn::ModuleProductionNode* pInstance)
{
	XN_DELETE(pInstance);
}

