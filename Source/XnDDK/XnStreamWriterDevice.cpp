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
#include "XnStreamWriterDevice.h"
#include "XnStreamWriterStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamWriterDevice::XnStreamWriterDevice(const XnChar* strName, XnUInt32 nInternalBufferSize) :
	XnStreamDevice(strName, nInternalBufferSize)
{
}

XnStreamWriterDevice::~XnStreamWriterDevice()
{
}

XnStatus XnStreamWriterDevice::InitImpl(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// first init self
	nRetVal = XnDeviceBase::InitImpl(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = InitPacker(pDeviceConfig->cpConnectionString);
	XN_IS_STATUS_OK(nRetVal);

	// now take initial state
	XnPropertySet* pSet;
	nRetVal = XnPropertySetCreate(&pSet);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = GetAllProperties(pSet);

	// and write it down to stream
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = GetDataPacker()->WritePropertySet(pSet);
	}

	XnPropertySetDestroy(&pSet);

	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::Destroy()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = GetDataPacker()->WriteEnd();
	XN_IS_STATUS_OK(nRetVal);

	XnStreamDevice::Destroy();

	XnStreamDevice::Destroy();
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::CreateStream(const XnChar* StreamType, const XnChar* StreamName /* = NULL */, const XnPropertySet* pInitialValues /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// create the stream
	nRetVal = XnDeviceBase::CreateStream(StreamType, StreamName, pInitialValues);
	XN_IS_STATUS_OK(nRetVal);

	XnStreamDeviceStreamHolder* pStreamHolder;
	nRetVal = FindStream(StreamName, &pStreamHolder);
	XN_IS_STATUS_OK(nRetVal);

	// now set default compression
	nRetVal = pStreamHolder->GetStream()->SetProperty(XN_STREAM_PROPERTY_COMPRESSION, (XnUInt64)GetDefaultCompression(StreamType));
	XN_IS_STATUS_OK(nRetVal);

	// get a list of this stream properties
	XnPropertySet* pStreamProps;
	nRetVal = XnPropertySetCreate(&pStreamProps);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pStreamHolder->GetStream()->GetAllProperties(pStreamProps);
	if (nRetVal != XN_STATUS_OK)
	{
		XnPropertySetDestroy(&pStreamProps);
		return (nRetVal);
	}

	// and write it to file
	nRetVal = GetDataPacker()->WriteNewStream(StreamType, StreamName, pStreamProps);
	if (nRetVal != XN_STATUS_OK)
	{
		XnPropertySetDestroy(&pStreamProps);
		XnDeviceBase::DestroyStream(StreamName);
		return (nRetVal);
	}

	nRetVal = XnPropertySetDestroy(&pStreamProps);
	if (nRetVal != XN_STATUS_OK)
	{
		XnDeviceBase::DestroyStream(StreamName);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::DestroyStream(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// destroy it
	nRetVal = XnDeviceBase::DestroyStream(StreamName);
	XN_IS_STATUS_OK(nRetVal);

	// write down that the stream was removed.
	nRetVal = GetDataPacker()->WriteStreamRemoved(StreamName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::WriteStream(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pStreamOutput);

	// find the stream
	XnStreamDeviceStreamHolder* pStreamHolder;
	nRetVal = FindStream(pStreamOutput->StreamName, &pStreamHolder);
	XN_IS_STATUS_OK(nRetVal);

	if (!pStreamOutput->bIsNew)
	{
		// no need to write down the data
		return XN_STATUS_OK;
	}

	// write it to stream
	nRetVal = XnDeviceBase::WriteStream(pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	XnStreamWriterStream* pStream = (XnStreamWriterStream*)pStreamHolder->GetStream();

	// and to file
	nRetVal = pStream->GetDataPacker()->WriteStreamData(pStreamOutput, pStreamHolder->GetCodec());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnStreamWriterDevice::SortStreamOutputsByTimestamp(XnStreamData* apOutputs[], XnUInt32 nCount)
{
	// use bubble sort
	XnUInt32 n = nCount;
	XnBool bSwapped;
	XnStreamData* pTemp;

	if (nCount == 0)
		return;

	do
	{
		bSwapped = FALSE;
		for (XnUInt32 i = 0; i < n - 1; ++i)
		{
			if (apOutputs[i]->nTimestamp > apOutputs[i+1]->nTimestamp)
			{
				// swap
				pTemp = apOutputs[i];
				apOutputs[i] = apOutputs[i+1];
				apOutputs[i+1] = pTemp;

				bSwapped = TRUE;
			}
		}

		n -= 1;

	} while (bSwapped);
}

XnStatus XnStreamWriterDevice::Write(XnStreamDataSet* pStreamOutputSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);

	// get a list of objects in the set
	XnStreamData* aOutputs[XN_DEVICE_BASE_MAX_STREAMS_COUNT];
	XnUInt32 nCount = XN_DEVICE_BASE_MAX_STREAMS_COUNT;
	nRetVal = XnStreamDataSetCopyToArray(pStreamOutputSet, aOutputs, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	// sort them out by timestamp
	SortStreamOutputsByTimestamp(aOutputs, nCount);

	// now write them one by one
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		nRetVal = WriteStream(aOutputs[i]);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// this is just a writer device. It does what it's told (no checks)
	nRetVal = pModule->UnsafeUpdateProperty(PropertyName, nValue);
	XN_IS_STATUS_OK(nRetVal);

	GetDataPacker()->WriteProperty(ModuleName, PropertyName, nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// this is just a writer device. It does what it's told (no checks)
	nRetVal = pModule->UnsafeUpdateProperty(PropertyName, dValue);
	XN_IS_STATUS_OK(nRetVal);

	GetDataPacker()->WriteProperty(ModuleName, PropertyName, dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// this is just a writer device. It does what it's told (no checks)
	nRetVal = pModule->UnsafeUpdateProperty(PropertyName, strValue);
	XN_IS_STATUS_OK(nRetVal);

	GetDataPacker()->WriteProperty(ModuleName, PropertyName, strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// this is just a writer device. It does what it's told (no checks)
	nRetVal = pModule->UnsafeUpdateProperty(PropertyName, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	GetDataPacker()->WriteProperty(ModuleName, PropertyName, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamWriterDevice::CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnStreamWriterStream* pStream;
	XN_VALIDATE_NEW(pStream, XnStreamWriterStream, StreamType, StreamName, GetDataPacker());

	XnStreamDeviceStreamHolder* pHolder = XN_NEW(XnStreamDeviceStreamHolder, pStream, FALSE);
	if (pHolder == NULL)
	{
		XN_DELETE(pStream);
		return XN_STATUS_ALLOC_FAILED;
	}

	*ppStreamHolder = pHolder;

	return (XN_STATUS_OK);
}

void XnStreamWriterDevice::DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder)
{
	XN_DELETE(pStreamHolder->GetModule());
	XN_DELETE(pStreamHolder);
}

XnCompressionFormats XnStreamWriterDevice::GetDefaultCompression(const XnChar* strType)
{
	if (strcmp(strType, XN_STREAM_TYPE_DEPTH) == 0)
		return XN_COMPRESSION_16Z_EMB_TABLE;
	else if (strcmp(strType, XN_STREAM_TYPE_IMAGE) == 0)
		return XN_COMPRESSION_JPEG;
	else
		return XN_COMPRESSION_NONE;
}
