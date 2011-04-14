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
#include <XnUtils.h>

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

XnResolution OldResToOpenNIRes(XnResolutions res)
{
	switch (res)
	{
	case XN_RESOLUTION_CUSTOM: return XN_RES_CUSTOM;
	case XN_RESOLUTION_QVGA: return XN_RES_QVGA;
	case XN_RESOLUTION_VGA: return XN_RES_VGA;
	case XN_RESOLUTION_SXGA: return XN_RES_SXGA;
	case XN_RESOLUTION_UXGA: return XN_RES_UXGA;
	case XN_RESOLUTION_QQVGA: return XN_RES_QQVGA;
	case XN_RESOLUTION_QCIF: return XN_RES_QCIF;
	case XN_RESOLUTION_240P: return XN_RES_240P;
	case XN_RESOLUTION_CIF: return XN_RES_CIF;
	case XN_RESOLUTION_WVGA: return XN_RES_WVGA;
	case XN_RESOLUTION_480P: return XN_RES_480P;
	case XN_RESOLUTION_800_448: return XN_RES_CUSTOM;
	case XN_RESOLUTION_SVGA: return XN_RES_SVGA;
	case XN_RESOLUTION_576P: return XN_RES_576P;
	case XN_RESOLUTION_DV: return XN_RES_DV;
	case XN_RESOLUTION_720P: return XN_RES_720P;
	case XN_RESOLUTION_1280_960: return XN_RES_CUSTOM;
	default: 
		XN_ASSERT(FALSE);
		return XN_RES_CUSTOM;
	}
}

XnResolutions OpenNIResToOldRes(XnResolution res)
{
	switch (res)
	{
	case XN_RES_CUSTOM: return XN_RESOLUTION_CUSTOM;
	case XN_RES_QQVGA: return XN_RESOLUTION_QQVGA;
	case XN_RES_CGA: return XN_RESOLUTION_CUSTOM;
	case XN_RES_QVGA: return XN_RESOLUTION_QVGA;
	case XN_RES_VGA: return XN_RESOLUTION_VGA;
	case XN_RES_SVGA: return XN_RESOLUTION_SVGA;
	case XN_RES_XGA: return XN_RESOLUTION_CUSTOM;
	case XN_RES_720P: return XN_RESOLUTION_720P;
	case XN_RES_SXGA: return XN_RESOLUTION_SXGA;
	case XN_RES_UXGA: return XN_RESOLUTION_UXGA;
	case XN_RES_1080P: return XN_RESOLUTION_CUSTOM;
	case XN_RES_QCIF: return XN_RESOLUTION_QCIF;
	case XN_RES_240P: return XN_RESOLUTION_240P;
	case XN_RES_CIF: return XN_RESOLUTION_CIF;
	case XN_RES_WVGA: return XN_RESOLUTION_WVGA;
	case XN_RES_480P: return XN_RESOLUTION_480P;
	case XN_RES_576P: return XN_RESOLUTION_576P;
	case XN_RES_DV: return XN_RESOLUTION_DV;
	default: 
		XN_ASSERT(FALSE);
		return XN_RESOLUTION_CUSTOM;
	}
}

XN_DDK_API XnResolutions XnDDKGetResolutionFromXY(XnUInt32 nXRes, XnUInt32 nYRes)
{
	// check if this is a known OpenNI resolution
	XnResolution res = xnResolutionGetFromXYRes(nXRes, nYRes);
	if (res == XN_RES_CUSTOM)
	{
		// check if this is one of our special resolutions
		if (nXRes == 800 && nYRes == 448)
		{
			return XN_RESOLUTION_800_448;
		}
		else if (nXRes == 1280 && nYRes == 960)
		{
			return XN_RESOLUTION_1280_960;
		}
	}

	return OpenNIResToOldRes(res);
}

XN_DDK_API XnBool XnDDKGetXYFromResolution(XnResolutions res, XnUInt32* pnXRes, XnUInt32* pnYRes)
{
	// check if this is a known OpenNI resolution
	XnResolution openRes = OldResToOpenNIRes(res);
	if (openRes == XN_RES_CUSTOM)
	{
		// check if this is one of our special resolutions
		if (res == XN_RESOLUTION_800_448)
		{
			*pnXRes = 800; 
			*pnYRes = 448; 
			return TRUE;
		}
		else if (res == XN_RESOLUTION_1280_960)
		{
			*pnXRes = 1280;
			*pnYRes = 960;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		*pnXRes = xnResolutionGetXRes(openRes);
		*pnYRes = xnResolutionGetYRes(openRes);
		return TRUE;
	}
}

XN_DDK_API const XnChar* XnDDKGetResolutionName(XnResolutions res)
{
	const XnChar* strName = NULL;

	// check if this is a known OpenNI resolution
	XnResolution openRes = OldResToOpenNIRes(res);

	if (openRes == XN_RES_CUSTOM)
	{
		// check if this is one of our special resolutions
		if (res == XN_RESOLUTION_800_448)
		{
			return "800x448";
		}
		else if (res == XN_RESOLUTION_1280_960)
		{
			return "1280x960";
		}
		else
		{
			return "Custom";
		}
	}
	else
	{
		return xnResolutionGetName(openRes);
	}
}
