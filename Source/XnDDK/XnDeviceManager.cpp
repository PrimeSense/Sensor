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
#include "XnDeviceManager.h"
#include <XnOS.h>
#include <XnLog.h>
#include <XnCore.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_DEVICE_MANAGER	"DeviceManager"

/** The shared library file name prefix for Xiron I/O device files. */ 
#define XN_DEVICE_FILE_PREFIX	"XnDevice"

#define XN_DEVICE_MANAGER_FIND_FUNC_IN_LIB(lib, name, descriptor, rc)													\
	rc = xnOSGetProcAddress(lib, XN_STRINGIFY(XN_DEVICE_PROTO_NAME(name)), (XnFarProc*)&descriptor->Interface.name);	\
	if (rc != XN_STATUS_OK)																								\
	{																													\
		xnOSFreeLibrary(lib);																							\
		xnLogError(XN_MASK_DEVICE_MANAGER, "Failed to find function %s!", XN_STRINGIFY(XN_DEVICE_PROTO_NAME(name)));	\
		return (rc);																									\
	}

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnDeviceManagerData
{
	XnDeviceDescriptor aDevices[XN_DEVICE_MANAGER_MAX_NUMBER_OF_DEVICES];
	XnUInt32 nDevicesCount;
} XnDeviceManagerData;

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
XnDeviceManagerData* g_pDeviceManager = NULL;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnDeviceManagerUpdateDefinition(XnDeviceDescriptor* pDescriptor)
{
	return pDescriptor->Interface.GetDefinition(&pDescriptor->Definition);
}

#ifdef XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS

XnStatus XnDeviceManagerLoadDevice(const XnChar* csFileName, XnDeviceDescriptor* pDescriptor)
{
	// first load library
	XnStatus nRetVal = xnOSLoadLibrary(csFileName, &pDescriptor->hLib);
	XN_IS_STATUS_OK(nRetVal);

	// now search for all functions in device interface

	// NOTE: the following lines will cause each function to be found and stored in the descriptor
	#define XN_DEVICE_INTERFACE_FUNCTION(name, sig) XN_DEVICE_MANAGER_FIND_FUNC_IN_LIB(pDescriptor->hLib, name, pDescriptor, nRetVal)
	#include <XnDeviceProto.inl>
	#undef XN_DEVICE_INTERFACE_FUNCTION

	return XnDeviceManagerUpdateDefinition(pDescriptor);
}

XnStatus XnDeviceManagerLoadAllDevices(const XnChar* strDir)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar cpSearchString[XN_FILE_MAX_PATH] = "";

	if (strDir == NULL)
	{
		strDir = XN_FILE_LOCAL_DIR;
	}

	// Build the search pattern string
	XN_VALIDATE_STR_APPEND(cpSearchString, strDir, XN_FILE_MAX_PATH, nRetVal);
	XN_VALIDATE_STR_APPEND(cpSearchString, XN_FILE_DIR_SEP, XN_FILE_MAX_PATH, nRetVal);
	XN_VALIDATE_STR_APPEND(cpSearchString, XN_SHARED_LIBRARY_PREFIX, XN_FILE_MAX_PATH, nRetVal);
	XN_VALIDATE_STR_APPEND(cpSearchString, XN_DEVICE_FILE_PREFIX, XN_FILE_MAX_PATH, nRetVal);
	XN_VALIDATE_STR_APPEND(cpSearchString, XN_FILE_ALL_WILDCARD, XN_FILE_MAX_PATH, nRetVal);
	XN_VALIDATE_STR_APPEND(cpSearchString, XN_SHARED_LIBRARY_POSTFIX, XN_FILE_MAX_PATH, nRetVal);

	// Get a file list of Xiron devices
	XnChar acsFileList[XN_DEVICE_MANAGER_MAX_NUMBER_OF_DEVICES][XN_FILE_MAX_PATH];
	XnUInt32 nFileCount = 0;

	xnLogVerbose(XN_MASK_DEVICE_MANAGER, "Searching for %s...", cpSearchString);
	nRetVal = xnOSGetFileList(cpSearchString, NULL, acsFileList, XN_DEVICE_MANAGER_MAX_NUMBER_OF_DEVICES, &nFileCount);
	if ((nRetVal != XN_STATUS_OS_FILE_NOT_FOUND) && (nRetVal != XN_STATUS_OK))
	{
		return (nRetVal);
	}

	// now try to load each file
	for (XnUInt32 nIndex = 0; nIndex < nFileCount; ++nIndex)
	{
		xnLogVerbose(XN_MASK_DEVICE_MANAGER, "Trying to load a device '%s'...", acsFileList[nIndex]);
		nRetVal = XnDeviceManagerLoadDevice(acsFileList[nIndex], &g_pDeviceManager->aDevices[g_pDeviceManager->nDevicesCount]);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogWarning(XN_MASK_DEVICE_MANAGER, "'%s' is not a valid device: %s", acsFileList[nIndex], xnGetStatusString(nRetVal));
		}
		else
		{
			xnLogInfo(XN_MASK_DEVICE_MANAGER, "device '%s' loaded.", acsFileList[nIndex]);
			g_pDeviceManager->nDevicesCount++;
		}
	}

	return (XN_STATUS_OK);
}

#else // no dynamic libs

XnStatus XnDeviceManagerLoadAllDevices()
{
	XnStatus nRetVal = XN_STATUS_OK;

	#define XN_DEVICE_INTERFACE_FUNCTION(name, sig) pDescriptor->Interface.name = XN_DEVICE_PROTO_NAME(name);

	XnDeviceDescriptor* pDescriptor = g_pDeviceManager->aDevices[g_pDeviceManager->nDevicesCount];

	#define XN_DEVICE_EXPORT_PREFIX SensorV2_
	#include <XnDeviceProto.inl>
	#undef XN_DEVICE_EXPORT_PREFIX

	nRetVal = XnDeviceManagerUpdateDefinition(pDescriptor);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_DEVICE_MANAGER, "'SensorV2' is not a valid device: %s", xnGetStatusString(nRetVal));
	}
	else
	{
		g_pDeviceManager->nDevicesCount++;
	}

	XnDeviceDescriptor* pDescriptor = g_pDeviceManager->aDevices[g_pDeviceManager->nDevicesCount];

	#define XN_DEVICE_EXPORT_PREFIX File_
	#include <XnDeviceProto.inl>
	#undef XN_DEVICE_EXPORT_PREFIX

	nRetVal = XnDeviceManagerUpdateDefinition(pDescriptor);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_DEVICE_MANAGER, "'SensorV2' is not a valid device: %s", xnGetStatusString(nRetVal));
	}
	else
	{
		g_pDeviceManager->nDevicesCount++;
	}

	#undef XN_DEVICE_INTERFACE_FUNCTION

	return (XN_STATUS_OK)
}

#endif

XnStatus XnDeviceManagerInit(const XnChar* strDevicesDir)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if device manager is already initialized
	if (g_pDeviceManager != NULL)
	{
		return XN_STATUS_ALREADY_INIT;
	}

	// allocate data
	XN_VALIDATE_ALLOC(g_pDeviceManager, XnDeviceManagerData);
	g_pDeviceManager->nDevicesCount = 0;

	// load devices
	nRetVal = XnDeviceManagerLoadAllDevices(strDevicesDir);
	XN_IS_STATUS_OK(nRetVal);

	// make sure we found at least one device
	if (g_pDeviceManager->nDevicesCount == 0)
	{
		return (XN_STATUS_IO_NO_DEVICES);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceManagerShutdown()
{
#ifdef XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	// unload all libraries
	for (XnUInt32 i = 0; i < g_pDeviceManager->nDevicesCount; ++i)
	{
		xnOSFreeLibrary(g_pDeviceManager->aDevices[i].hLib);
	}
#endif

	// check if device manager is initialized
	if (g_pDeviceManager == NULL)
	{
		return XN_STATUS_NOT_INIT;
	}

	// free data
	XN_FREE_AND_NULL(g_pDeviceManager);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceManagerGetDeviceList(XnDeviceDefinition* aDeviceDefinitions, XnUInt32* pnCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if device manager is initialized
	if (g_pDeviceManager == NULL)
	{
		return XN_STATUS_NOT_INIT;
	}

	// check count
	XnUInt32 nArraySize = *pnCount;

	*pnCount = g_pDeviceManager->nDevicesCount;

	if (nArraySize < g_pDeviceManager->nDevicesCount)
	{
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
	}

	// copy definitions
	for (XnUInt32 nIndex = 0; nIndex < g_pDeviceManager->nDevicesCount; ++nIndex)
	{
		aDeviceDefinitions[nIndex] = g_pDeviceManager->aDevices[nIndex].Definition;
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceManagerGetDeviceByName(const XnChar* csName, XnDeviceDescriptor** ppDeviceDescriptor)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if device manager is initialized
	if (g_pDeviceManager == NULL)
	{
		return XN_STATUS_NOT_INIT;
	}

	// special case: AUTO
	if (strncmp(csName, XN_DEVICE_MANAGER_AUTO_NAME, strlen(XN_DEVICE_MANAGER_AUTO_NAME)) == 0)
	{
		for (XnUInt32 nIndex = 0; nIndex < g_pDeviceManager->nDevicesCount; ++nIndex)
		{	
			// Get a pointer to the current device descriptor
			XnDeviceDescriptor* pDescriptor = &g_pDeviceManager->aDevices[nIndex];

			XnUInt32 nCount = 0;
			nRetVal = pDescriptor->Interface.Enumerate(NULL, &nCount);
			if (nCount > 0)
			{
				*ppDeviceDescriptor = pDescriptor;
				return (XN_STATUS_OK);
			}
		}
	}
	else
	{
		// search by name
		for (XnUInt32 nIndex = 0; nIndex < g_pDeviceManager->nDevicesCount; ++nIndex)
		{
			if (strcmp(g_pDeviceManager->aDevices[nIndex].Definition.cpName, csName) == 0)
			{
				*ppDeviceDescriptor = &g_pDeviceManager->aDevices[nIndex];
				return (XN_STATUS_OK);
			}
		}
	}

	return (XN_STATUS_IO_DEVICE_NOT_FOUND);
}
