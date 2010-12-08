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

XnStatus XnDeviceFile::Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount)
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

