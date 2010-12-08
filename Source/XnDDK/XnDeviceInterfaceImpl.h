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






/**
* This file is intended to allow any IXnDevice derivative to export its interface for 
* a "C" environment.
* To use, one must first define the following:
* XN_DEVICE_BASE_DERIVATIVE - the name of the class inheriting from IXnDevice.
* XN_DEVICE_BASE_DERIVATIVE_PREFIX (optional) - a prefix for function names.
*/

#ifndef XN_DEVICE_BASE_DERIVATIVE
#error "To use this file, XN_DEVICE_BASE_DERIVATIVE must be defined as the class inheriting from IXnDevice"
#endif

#include <XnDevice.h>
#include <XnOS.h>

// use default prefix, if one is not provided
#ifndef XN_DEVICE_BASE_DERIVATIVE_PREFIX
#define XN_DEVICE_BASE_DERIVATIVE_PREFIX XN_DEVICE_EXPORT_PREFIX
#endif

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetDefinition)(XnDeviceDefinition* pDeviceDefinition)
{
	return XN_DEVICE_BASE_DERIVATIVE::GetDefinition(pDeviceDefinition);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(Enumerate)(XnConnectionString* aConnectionStrings, XnUInt32* pnCount)
{
	return XN_DEVICE_BASE_DERIVATIVE::Enumerate(aConnectionStrings, pnCount);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(Create)(XnDeviceHandle* pDeviceHandle, const XnDeviceConfig* pDeviceConfig)
{
	IXnDevice* pDevice = XN_NEW(XN_DEVICE_BASE_DERIVATIVE);
	XnStatus nRetVal = pDevice->Init(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	*pDeviceHandle = pDevice->GetDeviceHandle();

	return XN_STATUS_OK;
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(Destroy)(XnDeviceHandle* pDeviceHandle)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(*pDeviceHandle);
	XnStatus nRetVal = pDevice->Destroy();
	XN_IS_STATUS_OK(nRetVal);

	XN_DELETE(pDevice);
	*pDeviceHandle = NULL;

	return XN_STATUS_OK;
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetSupportedStreams)(const XnDeviceHandle DeviceHandle, const XnChar** aStreamName, XnUInt32* pnStreamNamesCount)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetSupportedStreams(aStreamName, pnStreamNamesCount);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(CreateStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamType, const XnChar* StreamName, const XnPropertySet* pInitialValues)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->CreateStream(StreamType, StreamName, pInitialValues);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(DestroyStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->DestroyStream(StreamName);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(OpenStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->OpenStream(StreamName);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(CloseStream)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->CloseStream(StreamName);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(OpenAllStreams)(const XnDeviceHandle DeviceHandle)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->OpenAllStreams();
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(CloseAllStreams)(const XnDeviceHandle DeviceHandle)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->CloseAllStreams();
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetStreamNames)(const XnDeviceHandle DeviceHandle, const XnChar** pstrNames, XnUInt32* pnNamesCount)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetStreamNames(pstrNames, pnNamesCount);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(DoesModuleExist)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, XnBool* pbDoesExist)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->DoesModuleExist(ModuleName, pbDoesExist);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(RegisterToStreamsChange)(const XnDeviceHandle DeviceHandle, XnDeviceOnStreamsChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->RegisterToStreamsChange(Handler, pCookie, phCallback);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(UnregisterFromStreamsChange)(const XnDeviceHandle DeviceHandle, XnCallbackHandle hCallback)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->UnregisterFromStreamsChange(hCallback);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(CreateStreamData)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnStreamData** ppStreamData)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->CreateStreamData(StreamName, ppStreamData);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(DestroyStreamData)(XnStreamData** ppStreamData)
{
	return XN_DEVICE_BASE_DERIVATIVE::DestroyStreamData(ppStreamData);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(RegisterToNewStreamData)(const XnDeviceHandle DeviceHandle, XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->RegisterToNewStreamData(Handler, pCookie, phCallback);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(UnregisterFromNewStreamData)(const XnDeviceHandle DeviceHandle, XnCallbackHandle hCallback)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->UnregisterFromNewStreamData(hCallback);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(IsNewDataAvailable)(const XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnBool* pbNewDataAvailable, XnUInt64* pnTimestamp)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->IsNewDataAvailable(StreamName, pbNewDataAvailable, pnTimestamp);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(ReadStream)(const XnDeviceHandle DeviceHandle, XnStreamData* pStreamOutput)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->ReadStream(pStreamOutput);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(Read)(const XnDeviceHandle DeviceHandle, XnStreamDataSet* pStreamOutputSet)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->Read(pStreamOutputSet);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(WriteStream)(const XnDeviceHandle DeviceHandle, XnStreamData* pStreamOutput)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->WriteStream(pStreamOutput);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(Write)(const XnDeviceHandle DeviceHandle, XnStreamDataSet* pStreamOutputSet)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->Write(pStreamOutputSet);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(Tell)(const XnDeviceHandle DeviceHandle, XnUInt64* pnTimestamp)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->Tell(pnTimestamp);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(Seek)(const XnDeviceHandle DeviceHandle, XnUInt64 nTimestamp)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->Seek(nTimestamp);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(TellFrame)(const XnDeviceHandle DeviceHandle, XnUInt32* pnFrameID)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->TellFrame(pnFrameID);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(SeekFrame)(const XnDeviceHandle DeviceHandle, XnUInt32 nFrameID)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->SeekFrame(nFrameID);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(DoesPropertyExist)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnBool* pbDoesExist)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->DoesPropertyExist(ModuleName, PropertyName, pbDoesExist);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetPropertyType)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnPropertyType* pnType)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetPropertyType(ModuleName, PropertyName, pnType);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(SetIntProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->SetProperty(ModuleName, PropertyName, nValue);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(SetRealProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->SetProperty(ModuleName, PropertyName, dValue);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(SetStringProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->SetProperty(ModuleName, PropertyName, csValue);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(SetGeneralProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnGeneralBuffer Value)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->SetProperty(ModuleName, PropertyName, Value);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetIntProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetProperty(ModuleName, PropertyName, pnValue);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetRealProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetProperty(ModuleName, PropertyName, pdValue);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetStringProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, XnChar* csValue)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetProperty(ModuleName, PropertyName, csValue);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetGeneralProperty)(const XnDeviceHandle DeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer* pValue)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetProperty(ModuleName, PropertyName, *pValue);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(LoadConfigFromFile)(const XnDeviceHandle DeviceHandle, const XnChar* csINIFilePath, const XnChar* csSectionName)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->LoadConfigFromFile(csINIFilePath, csSectionName);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(BatchConfig)(const XnDeviceHandle DeviceHandle, const XnPropertySet* pChangeSet)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->BatchConfig(pChangeSet);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(GetAllProperties)(const XnDeviceHandle DeviceHandle, XnPropertySet* pPropertySet, XnBool bNoStreams /* = FALSE */, const XnChar* strModule /* = NULL */)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->GetAllProperties(pPropertySet, bNoStreams, strModule);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(RegisterToPropertyChange)(const XnDeviceHandle DeviceHandle, const XnChar* Module, const XnChar* PropertyName, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->RegisterToPropertyChange(Module, PropertyName, Handler, pCookie, phCallback);
}

XN_DEVICE_API XnStatus XN_DEVICE_PROTO_NAME(UnregisterFromPropertyChange)(const XnDeviceHandle DeviceHandle, const XnChar* Module, const XnChar* PropertyName, XnCallbackHandle hCallback)
{
	IXnDevice* pDevice = IXnDevice::GetFromDeviceHandle(DeviceHandle);
	return pDevice->UnregisterFromPropertyChange(Module, PropertyName, hCallback);
}

