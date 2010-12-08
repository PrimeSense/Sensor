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
#include <XnDeviceProxy.h>
#include "XnDeviceManager.h"
#include <XnOS.h>
#include <XnHash.h>
#include "XnDeviceInterfaceAdapter.h"
#include <XnPsVersion.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DEVICE_PROXY_NAME		"Proxy"
#define XN_DEVICE_PROXY_DESCRIPTION "Xiron Proxy Device"
#define XN_DEVICE_PROXY_CONNECTION_STRING_SEPARATOR	";"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnDeviceProxyDeviceHandle
{
	XnDeviceDescriptor* pDesc;
	XnDeviceHandle ActualDevice;
} XnDeviceProxyDeviceHandle;

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
/** Stores a hash of streamoutput objects to their creating device name. */
XnHash g_StreamOutputHash;

//---------------------------------------------------------------------------
// XnDeviceProxy functions
//---------------------------------------------------------------------------
XN_DDK_API XnStatus XnDeviceProxyGetDeviceList(XnDeviceDefinition* aDeviceDefinitions, XnUInt32* pnCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// take definitions
	return XnDeviceManagerGetDeviceList(aDeviceDefinitions, pnCount);
}

XN_DDK_API XnStatus XnDeviceProxyEnumerateDeviceByName(const XnChar* csDeviceName, XnConnectionString* aConnectionStrings, XnUInt32* pnCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get device interface
	XnDeviceDescriptor* pDescriptor = NULL;
	nRetVal = XnDeviceManagerGetDeviceByName(csDeviceName, &pDescriptor);
	XN_IS_STATUS_OK(nRetVal);

	// enumerate
	nRetVal = pDescriptor->Interface.Enumerate(aConnectionStrings, pnCount);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnDeviceProxyCreateDeviceByName(const XnChar* csDeviceName, XnDeviceHandle* pDeviceHandle, const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get device interface
	XnDeviceDescriptor* pDescriptor = NULL;
	nRetVal = XnDeviceManagerGetDeviceByName(csDeviceName, &pDescriptor);
	XN_IS_STATUS_OK(nRetVal);

	// now create the actual device
	XnDeviceHandle ActualDevice;
	nRetVal = pDescriptor->Interface.Create(&ActualDevice, pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	// create our handle
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)xnOSMalloc(sizeof(XnDeviceProxyDeviceHandle));
	if (pHandle == NULL)
	{
		pDescriptor->Interface.Destroy(&ActualDevice);
		return XN_STATUS_ALLOC_FAILED;
	}

	pHandle->ActualDevice = ActualDevice;
	pHandle->pDesc = pDescriptor;

	// and give it to user
	*pDeviceHandle = pHandle;

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnDeviceProxyCreateDeviceByINIFile(const XnChar* strIniFileName, const XnChar* strSectionName, XnDeviceHandle* pDeviceHandle, const XnPropertySet* pInitialValues /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar cpDeviceName[XN_INI_MAX_LEN];
	XnChar cpMode[XN_INI_MAX_LEN];
	XnChar cpConnectionString[XN_INI_MAX_LEN];
	XnChar cpSharing[XN_INI_MAX_LEN];

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(strIniFileName);
	XN_VALIDATE_INPUT_PTR(strSectionName);
	XN_VALIDATE_OUTPUT_PTR(pDeviceHandle);

	XnDeviceConfig config;
	config.cpConnectionString = cpConnectionString;
	config.SharingMode = XN_DEVICE_EXCLUSIVE;

	// Read the device name, mode and connection string from the INI file into a temp strings
	XN_VALIDATE_READ_INI_STR(strIniFileName, strSectionName, "Name", cpDeviceName, XN_INI_MAX_LEN, nRetVal);
	XN_VALIDATE_READ_INI_STR(strIniFileName, strSectionName, "Mode", cpMode, XN_INI_MAX_LEN, nRetVal);
	XN_VALIDATE_READ_INI_STR(strIniFileName, strSectionName, "ConnectionString", cpConnectionString, XN_INI_MAX_LEN, nRetVal);

	// Read optional parameters
	if (XN_STATUS_OK == xnOSReadStringFromINI(strIniFileName, strSectionName, "Sharing", cpSharing, XN_INI_MAX_LEN))
	{
		if (strcmp(cpSharing, "Exclusive") == 0)
		{
			config.SharingMode = XN_DEVICE_EXCLUSIVE;
		}
		else if (strcmp(cpSharing, "Shared") == 0)
		{
			config.SharingMode = XN_DEVICE_SHARED;
		}
		else
		{
			return (XN_STATUS_IO_DEVICE_INVALID_SHARING);
		}
	}
	
	if (strcmp(cpMode, "Read") == 0)
	{
		config.DeviceMode = XN_DEVICE_MODE_READ;
	}
	else if (strcmp(cpMode, "Write") == 0)
	{
		config.DeviceMode = XN_DEVICE_MODE_WRITE;
	}
	else
	{
		return (XN_STATUS_IO_DEVICE_INVALID_MODE);
	}

	config.pInitialValues = pInitialValues;

	nRetVal = XnDeviceProxyCreateDeviceByName(cpDeviceName, pDeviceHandle, &config);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnDeviceProxyDestroyStreamOutputByName(const XnChar* csDeviceName, XnStreamData** ppStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(csDeviceName);
	XN_VALIDATE_INPUT_PTR(ppStreamOutput);

	// get device interface
	XnDeviceDescriptor* pDescriptor = NULL;
	nRetVal = XnDeviceManagerGetDeviceByName(csDeviceName, &pDescriptor);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pDescriptor->Interface.DestroyStreamData(ppStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnDeviceProxyGetDeviceName(XnDeviceHandle DeviceHandle, XnChar* csDeviceName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(DeviceHandle);

	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	nRetVal = xnOSStrCopy(csDeviceName, pHandle->pDesc->Definition.cpName, XN_DEVICE_MAX_STRING_LENGTH);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}


//---------------------------------------------------------------------------
// Device Interface implementation
//---------------------------------------------------------------------------
XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetDefinition)(XnDeviceDefinition* pDeviceDefinition)
{
	XN_VALIDATE_INPUT_PTR(pDeviceDefinition);

	pDeviceDefinition->cpName = XN_DEVICE_PROXY_NAME;
	pDeviceDefinition->cpDescription = XN_DEVICE_PROXY_DESCRIPTION;
	pDeviceDefinition->nMajorVersion = XN_PS_MAJOR_VERSION;
	pDeviceDefinition->nMinorVersion = XN_PS_MINOR_VERSION;
	pDeviceDefinition->nXironVersion = XN_PS_MAJOR_VERSION;

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(Enumerate)(XnConnectionString* aConnectionStrings, XnUInt32* pnCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(aConnectionStrings);
	XN_VALIDATE_INPUT_PTR(pnCount);

	// take all loaded devices
	XnDeviceDefinition aDeviceDefinitions[XN_DEVICE_MANAGER_MAX_NUMBER_OF_DEVICES];
	XnUInt32 nDevicesCount = XN_DEVICE_MANAGER_MAX_NUMBER_OF_DEVICES;
	nRetVal = XnDeviceManagerGetDeviceList(aDeviceDefinitions, &nDevicesCount);
	XN_IS_STATUS_OK(nRetVal);

	XnUInt32 nDeviceCount = 0;
	XnConnectionString* pDeviceConnectionStrings = NULL;
	XnUInt32 nTotalCount = 0;
	XnDeviceDescriptor* pDescriptor = NULL;
	XnChar csConnectionStringPrefix[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt32 nBytesWritten = 0;

	// now enumerate each device
	for (XnUInt32 nDevice = 0; nDevice < nDevicesCount; ++nDevice)
	{
		pDeviceConnectionStrings = aConnectionStrings + nTotalCount;
		nDeviceCount = *pnCount - nTotalCount;

		// enumerate device
		nRetVal = XnDeviceProxyEnumerateDeviceByName(aDeviceDefinitions[nDevice].cpName, pDeviceConnectionStrings, &nDeviceCount);

		// create connection string prefix for this device
		nRetVal = xnOSStrFormat(csConnectionStringPrefix, XN_DEVICE_MAX_STRING_LENGTH, &nBytesWritten, "%s%s", aDeviceDefinitions[nDevice].cpName, XN_DEVICE_PROXY_CONNECTION_STRING_SEPARATOR);
		XN_IS_STATUS_OK(nRetVal);

		// fix each connection string to include device name
		for (XnUInt32 nString = 0; nString < nDeviceCount; ++nString)
		{
			XN_VALIDATE_STR_PREFIX(csConnectionStringPrefix, pDeviceConnectionStrings[nString], XN_DEVICE_MAX_STRING_LENGTH, nRetVal);
		}

		// add to total count
		nTotalCount += nDeviceCount;
	}

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(Create)(XnDeviceHandle* pDeviceHandle, const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDeviceConfig);
	XN_VALIDATE_OUTPUT_PTR(pDeviceHandle);

	// get device type from connection string
	XN_VALIDATE_INPUT_PTR(pDeviceConfig->cpConnectionString);

	// search for separator
	const XnChar* pSeparator = strstr(pDeviceConfig->cpConnectionString, XN_DEVICE_PROXY_CONNECTION_STRING_SEPARATOR);
	if (pSeparator == NULL)
		return (XN_STATUS_IO_INVALID_CONNECTION_STRING);

	// copy device name
	XnChar csDeviceName[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = xnOSStrNCopy(csDeviceName, pDeviceConfig->cpConnectionString, pSeparator - pDeviceConfig->cpConnectionString, XN_DEVICE_MAX_STRING_LENGTH);
	XN_IS_STATUS_OK(nRetVal);
	csDeviceName[pSeparator - pDeviceConfig->cpConnectionString] = '\0';

	// create internal device config
	XnDeviceConfig InternalConfig = *pDeviceConfig;

	// replace internal connection string
	XnConnectionString csInternalConnectionString;
	nRetVal = xnOSStrCopy(csInternalConnectionString, pSeparator + 1, XN_DEVICE_MAX_STRING_LENGTH);
	XN_IS_STATUS_OK(nRetVal);

	InternalConfig.cpConnectionString = csInternalConnectionString;

	// create the device
	nRetVal = XnDeviceProxyCreateDeviceByName(csDeviceName, pDeviceHandle, &InternalConfig);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(Destroy)(XnDeviceHandle* pDeviceHandle)
{
	XN_VALIDATE_INPUT_PTR(pDeviceHandle);
	XN_VALIDATE_INPUT_PTR(*pDeviceHandle);

	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)*pDeviceHandle;

	// first destroy actual device
	XnStatus nRetVal = pHandle->pDesc->Interface.Destroy(&pHandle->ActualDevice);
	XN_IS_STATUS_OK(nRetVal);

	// now destroy our handle
	XN_FREE_AND_NULL(*pDeviceHandle);

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetSupportedStreams)(const XnDeviceHandle DeviceHandle, const XnChar** aStreamsName, XnUInt32* pnStreamNamesCount)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetSupportedStreams(pHandle->ActualDevice, aStreamsName, pnStreamNamesCount);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(CreateStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamType, const XnChar* StreamName, const XnPropertySet* pInitialValues)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.CreateStream(pHandle->ActualDevice, StreamType, StreamName, pInitialValues);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(DestroyStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.DestroyStream(pHandle->ActualDevice, StreamName);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(OpenStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.OpenStream(pHandle->ActualDevice, StreamName);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(CloseStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.CloseStream(pHandle->ActualDevice, StreamName);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(OpenAllStreams)(const XnDeviceHandle DeviceHandle)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.OpenAllStreams(pHandle->ActualDevice);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(CloseAllStreams)(const XnDeviceHandle DeviceHandle)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.CloseAllStreams(pHandle->ActualDevice);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetStreamNames)(const XnDeviceHandle DeviceHandle, const XnChar** pstrNames, XnUInt32* pnNamesCount)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetStreamNames(pHandle->ActualDevice, pstrNames, pnNamesCount);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(DoesModuleExist)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, XnBool* pbDoesExist)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.DoesModuleExist(pHandle->ActualDevice, ModuleName, pbDoesExist);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(RegisterToStreamsChange)(const XnDeviceHandle DeviceHandle, XnDeviceOnStreamsChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.RegisterToStreamsChange(pHandle->ActualDevice, Handler, pCookie, phCallback);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(UnregisterFromStreamsChange)(const XnDeviceHandle DeviceHandle, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.UnregisterFromStreamsChange(pHandle->ActualDevice, hCallback);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(CreateStreamData)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnStreamData** ppStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(DeviceHandle);

	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	nRetVal = pHandle->pDesc->Interface.CreateStreamData(pHandle->ActualDevice, StreamName, ppStreamData);
	XN_IS_STATUS_OK(nRetVal);

	// register this object in the hash
	nRetVal = g_StreamOutputHash.Set(*ppStreamData, pHandle->pDesc);
	if (nRetVal != XN_STATUS_OK)
	{
		pHandle->pDesc->Interface.DestroyStreamData(ppStreamData);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(DestroyStreamData)(XnStreamData** ppStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(ppStreamData);

	// keep original pointer (before we destroy it)
	XnStreamData* pObject = *ppStreamData;

	// find descriptor of the device that created this object
	XnDeviceDescriptor* pDesc = NULL;
	nRetVal = g_StreamOutputHash.Get(*ppStreamData, (XnValue&)pDesc);
	XN_IS_STATUS_OK(nRetVal);

	// destroy the object
	nRetVal = pDesc->Interface.DestroyStreamData(ppStreamData);
	XN_IS_STATUS_OK(nRetVal);

	// and remove it from map
	g_StreamOutputHash.Remove(pObject, (XnValue&)pDesc);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(RegisterToNewStreamData)(const XnDeviceHandle DeviceHandle, XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.RegisterToNewStreamData(pHandle->ActualDevice, Handler, pCookie, phCallback);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(UnregisterFromNewStreamData)(const XnDeviceHandle DeviceHandle, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.UnregisterFromNewStreamData(pHandle->ActualDevice, hCallback);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(IsNewDataAvailable)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnBool* pbNewDataAvailable, XnUInt64* pnTimestamp)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.IsNewDataAvailable(pHandle->ActualDevice, StreamName, pbNewDataAvailable, pnTimestamp);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(ReadStream)(const XnDeviceHandle DeviceHandle, XnStreamData* pStreamOutput)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.ReadStream(pHandle->ActualDevice, pStreamOutput);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(Read)(const XnDeviceHandle DeviceHandle, XnStreamDataSet* pStreamOutputSet)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.Read(pHandle->ActualDevice, pStreamOutputSet);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(WriteStream)(const XnDeviceHandle DeviceHandle, XnStreamData* pStreamOutput)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.WriteStream(pHandle->ActualDevice, pStreamOutput);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(Write)(const XnDeviceHandle DeviceHandle, XnStreamDataSet* pStreamOutputSet)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.Write(pHandle->ActualDevice, pStreamOutputSet);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(Tell)(const XnDeviceHandle DeviceHandle, XnUInt64* pnTimestamp)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.Tell(pHandle->ActualDevice, pnTimestamp);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(Seek)(const XnDeviceHandle DeviceHandle, XnUInt64 nTimestamp)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.Seek(pHandle->ActualDevice, nTimestamp);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(TellFrame)(const XnDeviceHandle DeviceHandle, XnUInt32* pnFrameID)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.TellFrame(pHandle->ActualDevice, pnFrameID);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(SeekFrame)(const XnDeviceHandle DeviceHandle, XnUInt32 nFrameID)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.SeekFrame(pHandle->ActualDevice, nFrameID);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(DoesPropertyExist)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnBool* pbDoesExist)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.DoesPropertyExist(pHandle->ActualDevice, ModuleName, PropertyName, pbDoesExist);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetPropertyType)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnPropertyType* pnType)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetPropertyType(pHandle->ActualDevice, ModuleName, PropertyName, pnType);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(SetIntProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.SetIntProperty(pHandle->ActualDevice, ModuleName, PropertyName, nValue);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(SetRealProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.SetRealProperty(pHandle->ActualDevice, ModuleName, PropertyName, dValue);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(SetStringProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.SetStringProperty(pHandle->ActualDevice, ModuleName, PropertyName, csValue);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(SetGeneralProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnGeneralBuffer Value)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.SetGeneralProperty(pHandle->ActualDevice, ModuleName, PropertyName, Value);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetIntProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetIntProperty(pHandle->ActualDevice, ModuleName, PropertyName, pnValue);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetRealProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetRealProperty(pHandle->ActualDevice, ModuleName, PropertyName, pdValue);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetStringProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnChar* csValue)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetStringProperty(pHandle->ActualDevice, ModuleName, PropertyName, csValue);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetGeneralProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer* pValue)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetGeneralProperty(pHandle->ActualDevice, ModuleName, PropertyName, pValue);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(LoadConfigFromFile)(const XnDeviceHandle DeviceHandle, const XnChar* csINIFilePath, const XnChar* csSectionName)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.LoadConfigFromFile(pHandle->ActualDevice, csINIFilePath, csSectionName);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(BatchConfig)(const XnDeviceHandle DeviceHandle, const XnPropertySet* pChangeSet)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.BatchConfig(pHandle->ActualDevice, pChangeSet);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(GetAllProperties)(const XnDeviceHandle DeviceHandle, XnPropertySet* pPropertySet, XnBool bNoStreams /* = FALSE */, const XnChar* strModule /* = NULL */)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.GetAllProperties(pHandle->ActualDevice, pPropertySet, bNoStreams, strModule);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(RegisterToPropertyChange)(const XnDeviceHandle DeviceHandle, const XnChar* Module, const XnChar* PropertyName, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.RegisterToPropertyChange(pHandle->ActualDevice, Module, PropertyName, Handler, pCookie, phCallback);
}

XN_DDK_API XnStatus XN_DEVICE_PROXY_PROTO(UnregisterFromPropertyChange)(const XnDeviceHandle DeviceHandle, const XnChar* Module, const XnChar* PropertyName, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INPUT_PTR(DeviceHandle);
	XnDeviceProxyDeviceHandle* pHandle = (XnDeviceProxyDeviceHandle*)DeviceHandle;
	return pHandle->pDesc->Interface.UnregisterFromPropertyChange(pHandle->ActualDevice, Module, PropertyName, hCallback);
}

