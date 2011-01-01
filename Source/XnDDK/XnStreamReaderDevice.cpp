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
#include "XnStreamReaderDevice.h"
#include "XnPropertySetInternal.h"
#include "XnStreamReaderStream.h"
#include "XnStreamReaderStreamHolder.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamReaderDevice::XnStreamReaderDevice(const XnChar* strName, XnUInt32 nInternalBufferSize) :
	XnStreamDevice(strName, nInternalBufferSize)
{}

XnStreamReaderDevice::~XnStreamReaderDevice()
{}

XnStatus XnStreamReaderDevice::InitImpl(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// we will init the device using state from the stream, instead of the one from user.
	// the one from user will be used to set properties afterwards.

	// first open the stream
	nRetVal = InitPacker(pDeviceConfig->cpConnectionString);
	XN_IS_STATUS_OK(nRetVal);

	// create a property set
	XnPropertySet* pSet;
	nRetVal = XnPropertySetCreate(&pSet);
	XN_IS_STATUS_OK(nRetVal);

	// read initial state (we assume first object in the stream is the initial state)
	nRetVal = ReadInitialState(pSet);
	if (nRetVal != XN_STATUS_OK)
	{
		XnPropertySetDestroy(&pSet);
		return (nRetVal);
	}

	nRetVal = SetInitialState(pDeviceConfig, pSet);
	if (nRetVal != XN_STATUS_OK)
	{
		XnPropertySetDestroy(&pSet);
		return (nRetVal);
	}

	// destroy the property set (we don't need it anymore)
	nRetVal = XnPropertySetDestroy(&pSet);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadInitialState(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read an object from data packer
	XnPackedDataType nType;
	nRetVal = GetDataPacker()->ReadNextObject(&nType);
	XN_IS_STATUS_OK(nRetVal);

	if (nType != XN_PACKED_PROPERTY_SET)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_FILE_CORRUPTED, XN_MASK_DDK, "Stream does not start with a property set!");
	}

	nRetVal = GetDataPacker()->ReadPropertySet(pSet);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::SetInitialState(const XnDeviceConfig* pDeviceConfig, XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// Fix state (remove some properties that we don't wish to reflect in reader device)
	XnActualPropertiesHash* pDeviceModule = NULL;
	if (XN_STATUS_OK == pSet->pData->Get(XN_MODULE_NAME_DEVICE, pDeviceModule))
	{
		pDeviceModule->Remove(XN_MODULE_PROPERTY_READ_WRITE_MODE);
		pDeviceModule->Remove(XN_MODULE_PROPERTY_PRIMARY_STREAM);
	}

	// now init base using this state (this will also create module DEVICE)
	XnDeviceConfig initConfig;
	initConfig.cpConnectionString = pDeviceConfig->cpConnectionString;
	initConfig.DeviceMode = pDeviceConfig->DeviceMode;
	initConfig.pInitialValues = pSet;
	initConfig.SharingMode = pDeviceConfig->SharingMode;

	nRetVal = XnStreamDevice::InitImpl(&initConfig);
	XN_IS_STATUS_OK(nRetVal);

	// now create the rest of the modules and streams (DEVICE was already created)
	XnPropertySetData* pPropSetData = pSet->pData;
	for (XnPropertySetData::ConstIterator it = pPropSetData->begin(); it != pPropSetData->end(); ++it)
	{
		// ignore module DEVICE
		if (strcmp(XN_MODULE_NAME_DEVICE, it.Key()) == 0)
		{
			continue;
		}

		// check if this is a stream
		XnActualPropertiesHash::ConstIterator itProp = it.Value()->end();
		if (XN_STATUS_OK == it.Value()->Find(XN_STREAM_PROPERTY_TYPE, itProp))
		{
			XnActualStringProperty* pTypeProp = (XnActualStringProperty*)itProp.Value();
			nRetVal = HandleNewStream(pTypeProp->GetValue(), it.Key(), it.Value());
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			// this is module. create it
			XnDeviceModuleHolder* pHolder = NULL;
			nRetVal = CreateModule(it.Key(), &pHolder);
			XN_IS_STATUS_OK(nRetVal);

			// set its props
			nRetVal = pHolder->Init(it.Value());
			if (nRetVal != XN_STATUS_OK)
			{
				DestroyModule(pHolder);
				return (nRetVal);
			}

			// and add it
			nRetVal = AddModule(pHolder);
			if (nRetVal != XN_STATUS_OK)
			{
				DestroyModule(pHolder);
				return (nRetVal);
			}
		}
	} // modules loop
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnStreamReaderStream* pStream;
	XN_VALIDATE_NEW(pStream, XnStreamReaderStream, StreamType, StreamName);

	XnStreamReaderStreamHolder* pHolder = XN_NEW(XnStreamReaderStreamHolder, pStream);
	if (pHolder == NULL)
	{
		XN_DELETE(pStream);
		return XN_STATUS_ALLOC_FAILED;
	}

	*ppStreamHolder = pHolder;

	return (XN_STATUS_OK);
}

void XnStreamReaderDevice::DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder)
{
	XN_DELETE(pStreamHolder->GetModule());
	XN_DELETE(pStreamHolder);
}

XnStatus XnStreamReaderDevice::ReadNextEventFromStream(XnPackedDataType* pnObjectType /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnPackedDataType nObjectType;

	nRetVal = GetDataPacker()->ReadNextObject(&nObjectType);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandlePackedObject(nObjectType);
	XN_IS_STATUS_OK(nRetVal);

	if (pnObjectType != NULL)
	{
		*pnObjectType = nObjectType;
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandlePackedObject(XnPackedDataType nObjectType)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	switch (nObjectType)
	{
	case XN_PACKED_NEW_STREAM:
		nRetVal = ReadNewStream();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_STREAM_REMOVED:
		nRetVal = ReadStreamRemoved();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_INT_PROPERTY:
		nRetVal = ReadIntProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_REAL_PROPERTY:
		nRetVal = ReadRealProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_STRING_PROPERTY:
		nRetVal = ReadStringProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_GENERAL_PROPERTY:
		nRetVal = ReadGeneralProperty();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_STREAM_DATA:
		nRetVal = ReadStreamData();
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_PACKED_END:
		nRetVal = HandleEndOfStream();
		XN_IS_STATUS_OK(nRetVal);
		break;
	default:
		XN_LOG_ERROR_RETURN(XN_STATUS_DEVICE_FILE_CORRUPTED, XN_MASK_DDK, "Unexpected packed type: %d", nObjectType);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadNewStream()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];

	// create property set
	XnPropertySet* pPropertySet = NULL;
	nRetVal = XnPropertySetCreate(&pPropertySet);
	XN_IS_STATUS_OK(nRetVal);

	// read from stream
	nRetVal = GetDataPacker()->ReadNewStream(strType, strName, pPropertySet);

	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = ValidateOnlyModule(pPropertySet, strName);
	}

	if (nRetVal == XN_STATUS_OK)
	{
		// create it
		nRetVal = HandleNewStream(strType, strName, pPropertySet->pData->begin().Value());
	}

	XnPropertySetDestroy(&pPropertySet);

	return (nRetVal);
}

XnStatus XnStreamReaderDevice::HandleNewStream(const XnChar *strType, const XnChar *strName, const XnActualPropertiesHash *pInitialValues)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnStreamDevice::CreateStreamImpl(strType, strName, pInitialValues);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadStreamRemoved()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read stream name
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];

	nRetVal = GetDataPacker()->ReadStreamRemoved(strName);
	XN_IS_STATUS_OK(nRetVal);

	// remove it
	nRetVal = HandleStreamRemoved(strName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandleStreamRemoved(const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnStreamReaderDevice::DestroyStream(strName);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadIntProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnUInt64 nValue;

	// read change data
	nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, &nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleIntProperty(strModule, strProp, nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandleIntProperty(const XnChar *strModule, const XnChar *strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// ignore some properties
	if (strcmp(strModule, XN_MODULE_NAME_DEVICE) == 0 && strcmp(strName, XN_MODULE_PROPERTY_PRIMARY_STREAM) == 0)
	{
		return (XN_STATUS_OK);
	}

	// find module
	XnDeviceModule* pModule;
	nRetVal = FindModule(strModule, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// update prop
	nRetVal = pModule->UnsafeUpdateProperty(strName, nValue);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadRealProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnDouble dValue;

	// read change data
	nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, &dValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleRealProperty(strModule, strProp, dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandleRealProperty(const XnChar *strModule, const XnChar *strName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// find module
	XnDeviceModule* pModule;
	nRetVal = FindModule(strModule, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// update prop
	nRetVal = pModule->UnsafeUpdateProperty(strName, dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadStringProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];

	// read change data
	nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleStringProperty(strModule, strProp, strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandleStringProperty(const XnChar *strModule, const XnChar *strName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// find module
	XnDeviceModule* pModule;
	nRetVal = FindModule(strModule, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// update prop
	nRetVal = pModule->UnsafeUpdateProperty(strName, strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadGeneralProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnGeneralBuffer gbValue;

	// read change data
	nRetVal = GetDataPacker()->ReadProperty(strModule, strProp, &gbValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleGeneralProperty(strModule, strProp, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandleGeneralProperty(const XnChar* strModule, const XnChar* strName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// find module
	XnDeviceModule* pModule;
	nRetVal = FindModule(strModule, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// update prop
	nRetVal = pModule->UnsafeUpdateProperty(strName, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::ReadStreamData()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnStreamData props;
	XnCompressionFormats nCompression;
	XnUInt32 nCompressedSize;
	nRetVal = GetDataPacker()->ReadStreamDataProps(&props, &nCompression, &nCompressedSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = HandleStreamData(&props, nCompression, nCompressedSize);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandleStreamData(XnStreamData* pDataProps, XnCompressionFormats nCompression, XnUInt32 nCompressedSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// find the stream
	XnStreamDeviceStreamHolder* pStreamHolder;
	nRetVal = FindStream(pDataProps->StreamName, &pStreamHolder);
	XN_IS_STATUS_OK(nRetVal);

	XnStreamReaderStream* pStream = (XnStreamReaderStream*)pStreamHolder->GetStream();

	XnStreamData* pStreamData = pStream->GetStreamData();

	// check size
	nRetVal = XnStreamDataCheckSize(pStreamData, pStream->GetRequiredDataSize());
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetDataPacker()->ReadStreamData(pStreamData, pStreamHolder->GetCodec());
	XN_IS_STATUS_OK(nRetVal);

	pStream->NewDataAvailable(pStreamData->nTimestamp, pStreamData->nFrameID);
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamReaderDevice::HandleEndOfStream()
{
	return (XN_STATUS_OK);
}
