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
#include <XnLog.h>
#include <XnDeviceSensorV2/XnSensorServerRunner.h>
#include <XnFormats/XnFormats.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_CHECK_RC(rc, what)														\
	if (rc != XN_STATUS_OK)															\
	{																				\
		printf("Failed to " what ": %s\n", xnGetStatusString(rc));					\
		return (rc);																\
	}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	XnStatus nRetVal = XN_STATUS_OK;

	printf("Starting sensor server...\n");

	const XnChar* strConfigDir = ".";
	if (argc >= 2)
	{
		strConfigDir = argv[1];
	}

	XnChar strConfigFile[XN_FILE_MAX_PATH];
	nRetVal = XnSensorServerGetGlobalConfigFile(strConfigDir, strConfigFile, XN_FILE_MAX_PATH);
	XN_CHECK_RC(nRetVal, "Resolving global config file");

#if (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_MACOSX)
	xnLogSetOutputFolder("/var/log/primesense/XnSensorServer/");
#endif

	nRetVal = XnFormatsInitFromINIFile(strConfigFile);
	XN_CHECK_RC(nRetVal, "Initializing DDK");
	
	printf("Running...\n");
	nRetVal = XnSensorServerRun(strConfigFile);
	XN_CHECK_RC(nRetVal, "starting sensor server");
	
	printf("\nShutting down sensor server...\n");
	XnDDKShutdown();

	return 0;
}
