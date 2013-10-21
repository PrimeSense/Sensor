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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnExportedFileDevice.h"
#include <XnPsVersion.h>
#include "XnDeviceFileReader.h"
#include "XnFileDevice.h"
#include <XnOpenNI.h>
#include <XnCommon.h>

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

XnStatus XnExportedFileDevice::EnumerateProductionTrees(xn::Context& /*context*/, xn::NodeInfoList& TreesList, xn::EnumerationErrors* /*pErrors*/)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnProductionNodeDescription Description;
	GetDescription(&Description);
	nRetVal = TreesList.Add(Description, NULL, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnExportedFileDevice::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* /*strCreationInfo*/, xn::NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, xn::ModuleProductionNode** ppInstance)
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
		XN_DELETE(pDevice);
		return (nRetVal);
	}

	*ppInstance = pDevice;

	return (XN_STATUS_OK);
}

void XnExportedFileDevice::Destroy(xn::ModuleProductionNode* pInstance)
{
	XN_DELETE(pInstance);
}

