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
#include <XnDevice.h>
#include <XnPsVersion.h>
#include "XnDeviceFile.h"
#include "XnDeviceFileReader.h"
#include "XnDeviceFileWriter.h"
#include <XnLog.h>

#define XN_DEVICE_BASE_DERIVATIVE XnDeviceFile
#include <XnDDK/XnDeviceInterfaceImpl.h>

XnStatus XnDeviceFile::GetDefinition(XnDeviceDefinition* pDeviceDefinition)
{
	XN_VALIDATE_OUTPUT_PTR(pDeviceDefinition);

	pDeviceDefinition->cpName = XN_DEVICE_NAME;
	pDeviceDefinition->cpDescription = XN_DEVICE_DESCRIPTION;
	pDeviceDefinition->nMajorVersion = XN_DEVICE_MAJORVERSION;
	pDeviceDefinition->nMinorVersion = XN_DEVICE_MINORVERSION;
	pDeviceDefinition->nXironVersion = XN_PS_MAJOR_VERSION;

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFile::Enumerate(XnConnectionString* /*aConnectionStrings*/, XnUInt32* pnCount)
{
	XN_VALIDATE_INPUT_PTR(pnCount);

	*pnCount = 0;

	return (XN_STATUS_OK);
}

XnDeviceFile::XnDeviceFile() : 
	XnDeviceBaseProxy(NULL)
{
}

XnDeviceFile::~XnDeviceFile()
{

}

XnStatus XnDeviceFile::Init(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDeviceConfig);

	XnDeviceBase* pActualDeviceFile = NULL;

	switch (pDeviceConfig->DeviceMode)
	{
	case XN_DEVICE_MODE_READ:
		XN_VALIDATE_NEW(pActualDeviceFile, XnDeviceFileReader);
		break;
	case XN_DEVICE_MODE_WRITE:
		XN_VALIDATE_NEW(pActualDeviceFile, XnDeviceFileWriter);
		break;
	default:
		return XN_STATUS_IO_DEVICE_INVALID_MODE;
	}

	// init actual device
	nRetVal = pActualDeviceFile->Init(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	ReplaceActualDevice(pActualDeviceFile);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceFile::Destroy()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// destroy actual
	nRetVal = XnDeviceBaseProxy::Destroy();
	XN_IS_STATUS_OK(nRetVal);

	ReplaceActualDevice(NULL);

	return (XN_STATUS_OK);
}

