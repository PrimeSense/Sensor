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
#include <XnDDK.h>
#include <XnFormats/XnFormats.h>
#include <XnOS.h>
#include "XnDeviceManager.h"

// The following line is needed to be once in *ALL* of the high level shared library modules. DO NOT REMOVE!!! 
XN_API_EXPORT_INIT()

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
static XnBool g_XnDDKWasInit = FALSE;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_DDK_API XnStatus XnDDKInit(const XnChar* strDevicesDir)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Was the DDK subsystem already initialized?
	if (g_XnDDKWasInit == FALSE)
	{
		// Init the Formats library
		nRetVal = XnFormatsInit();
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_ALREADY_INIT)
			return nRetVal;

		// Init DeviceManager
		nRetVal = XnDeviceManagerInit(strDevicesDir);
		XN_IS_STATUS_OK(nRetVal);

		g_XnDDKWasInit = TRUE;
	}
	else
	{
		// Trying to init twice...
		return (XN_STATUS_DDK_ALREADY_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnDDKInitFromINIFile(const XnChar* cpINIFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpINIFileName);

	// Was the DDK subsystem already initialized?
	if (g_XnDDKWasInit == FALSE)
	{
		// Init the Formats library
		nRetVal = XnFormatsInitFromINIFile(cpINIFileName);
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_ALREADY_INIT)
			return nRetVal;

		// read devices directory
		XnChar strDevicesDirectory[XN_INI_MAX_LEN] = "";
		XnChar* pDir = NULL;
		if (XN_STATUS_OK == xnOSReadStringFromINI(cpINIFileName, "DDK", "DevicesDir", strDevicesDirectory, XN_INI_MAX_LEN))
		{
			XN_VALIDATE_STR_APPEND(strDevicesDirectory, XN_FILE_DIR_SEP, XN_INI_MAX_LEN, nRetVal);
			pDir = strDevicesDirectory;
		}

		// Init DeviceManager
		nRetVal = XnDeviceManagerInit(pDir);
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_ALREADY_INIT)
			return nRetVal;

		g_XnDDKWasInit = TRUE;
	}
	else
	{
		// Trying to init twice...
		return (XN_STATUS_DDK_ALREADY_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnDDKShutdown()
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Was the DDK subsystem initialized?
	if (g_XnDDKWasInit == TRUE)
	{
		// shutdown device manager
		nRetVal = XnDeviceManagerShutdown();
		XN_IS_STATUS_OK(nRetVal);

		// shutdown the Formats library
		nRetVal = XnFormatsShutdown();
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_FORMATS_NOT_INIT)
			return nRetVal;

		g_XnDDKWasInit = FALSE;
	}
	else
	{
		// Trying to shutdown without doing init...
		return (XN_STATUS_DDK_NOT_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}

