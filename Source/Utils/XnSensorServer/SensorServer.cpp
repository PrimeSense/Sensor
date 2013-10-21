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
