/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnServerSensorInvoker.h"
#include "XnSensorClientServer.h"
#include <XnDDK/XnStreamDataInternal.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_TERMINATE_READER_THREAD_TIMEOUT		5000
#define XN_SENSOR_DEFAULT_ENABLE_MULTI_USERS			FALSE
#define XN_SENSOR_DEFAULT_NUMBER_OF_BUFFERS				6

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnServerSensorInvoker::XnServerSensorInvoker() :
	m_hSensorLock(NULL),
	m_hReaderThread(NULL),
	m_hNewDataEvent(NULL),
	m_bShouldRun(TRUE),
	m_errorState(XN_STATUS_OK),
	m_numberOfBuffers(XN_MODULE_PROPERTY_NUMBER_OF_BUFFERS, XN_SENSOR_DEFAULT_NUMBER_OF_BUFFERS),
	m_allowOtherUsers(XN_MODULE_PROPERTY_ENABLE_MULTI_USERS, XN_SENSOR_DEFAULT_ENABLE_MULTI_USERS)
{
	m_numberOfBuffers.UpdateSetCallback(SetNumberOfBuffersCallback, this);
	m_allowOtherUsers.UpdateSetCallback(SetAllowOtherUsersCallback, this);
}

XnServerSensorInvoker::~XnServerSensorInvoker()
{
	Free();
}

XnStatus XnServerSensorInvoker::Init(const XnChar* strDevicePath, const XnChar* strGlobalConfigFile, XnUInt32 nAdditionalProps, XnProperty** aAdditionalProps)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_sensor.SetGlobalConfigFile(strGlobalConfigFile);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&m_hSensorLock);
	XN_IS_STATUS_OK(nRetVal);

	XnDeviceConfig config;
	config.DeviceMode = XN_DEVICE_MODE_READ;
	config.cpConnectionString = strDevicePath;
	config.pInitialValues = NULL;
	config.SharingMode = XN_DEVICE_SHARED;

	nRetVal = m_sensor.Init(&config);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_sensor.DeviceModule()->AddProperties(aAdditionalProps, nAdditionalProps);
	XN_IS_STATUS_OK(nRetVal);

	XnProperty* aInvokerAdditionalProps[] = { &m_numberOfBuffers, &m_allowOtherUsers };
	nRetVal = m_sensor.DeviceModule()->AddProperties(aInvokerAdditionalProps, sizeof(aInvokerAdditionalProps) / sizeof(aInvokerAdditionalProps[0]));
	XN_IS_STATUS_OK(nRetVal);

	// configure from global file
	nRetVal = m_sensor.ConfigureModuleFromGlobalFile(XN_MODULE_NAME_DEVICE, XN_SENSOR_SERVER_CONFIG_FILE_SECTION);
	XN_IS_STATUS_OK(nRetVal);

	// register to events
	XnCallbackHandle hDummy = NULL;
	nRetVal = m_sensor.OnStreamCollectionChangedEvent().Register(StreamCollectionChangedCallback, this, hDummy);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_sensor.OnNewStreamDataEvent().Register(NewStreamDataCallback, this, hDummy);
	XN_IS_STATUS_OK(nRetVal);

	// register to all properties
	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	nRetVal = m_sensor.DeviceModule()->GetAllProperties(&props);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = RegisterToProps(&props);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateEvent(&m_hNewDataEvent, FALSE);
	XN_IS_STATUS_OK(nRetVal);

	// start reader thread
	nRetVal = xnOSCreateThread(ReaderThread, this, &m_hReaderThread);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

void XnServerSensorInvoker::Free()
{
	m_bShouldRun = FALSE;

	if (m_hReaderThread != NULL)
	{
		xnOSWaitAndTerminateThread(&m_hReaderThread, XN_SENSOR_TERMINATE_READER_THREAD_TIMEOUT);
		m_hReaderThread = NULL;
	}

	XnStatus nRetVal = m_sensor.Destroy();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed to destroy sensor: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}

	if (m_hNewDataEvent != NULL)
	{
		xnOSCloseEvent(&m_hNewDataEvent);
		m_hNewDataEvent = NULL;
	}

	if (m_hSensorLock != NULL)
	{
		xnOSCloseCriticalSection(&m_hSensorLock);
		m_hSensorLock = NULL;
	}
}

const XnChar* XnServerSensorInvoker::GetDevicePath()
{
	return m_sensor.GetUSBPath();
}

XnStatus XnServerSensorInvoker::RegisterToProps(XnPropertySet* pProps)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnCallbackHandle hDummy = NULL;

	for (XnPropertySetData::Iterator itMod = pProps->pData->Begin(); itMod != pProps->pData->End(); ++itMod)
	{
		XnActualPropertiesHash* pHash = itMod->Value();

		XnDeviceModule* pModule;
		nRetVal = m_sensor.FindModule(itMod->Key(), &pModule);
		XN_IS_STATUS_OK(nRetVal);

		for (XnActualPropertiesHash::Iterator itProp = pHash->Begin(); itProp != pHash->End(); ++itProp)
		{
			XnProperty* pProp;
			nRetVal = pModule->GetProperty(itProp->Key(), &pProp);
			XN_IS_STATUS_OK(nRetVal);

			// no need to keep the handle. We only want to unregister when the stream is destroyed, and then
			// it happens anyway.
			nRetVal = pProp->OnChangeEvent().Register(PropertyChangedCallback, this, hDummy);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::GetIntProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64* pnValue)
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.GetProperty(strModule, strProperty, pnValue);
}

XnStatus XnServerSensorInvoker::GetRealProperty( const XnChar* strModule, const XnChar* strProperty, XnDouble* pdValue )
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.GetProperty(strModule, strProperty, pdValue);
}

XnStatus XnServerSensorInvoker::GetStringProperty( const XnChar* strModule, const XnChar* strProperty, XnChar* strValue )
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.GetProperty(strModule, strProperty, strValue);
}

XnStatus XnServerSensorInvoker::GetGeneralProperty( const XnChar* strModule, const XnChar* strProperty, XnGeneralBuffer& gbValue )
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.GetProperty(strModule, strProperty, gbValue);
}

XnStatus XnServerSensorInvoker::SetIntProperty( const XnChar* strModule, const XnChar* strProperty, XnUInt64 nValue )
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.SetProperty(strModule, strProperty, nValue);
}

XnStatus XnServerSensorInvoker::SetRealProperty( const XnChar* strModule, const XnChar* strProperty, XnDouble dValue )
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.SetProperty(strModule, strProperty, dValue);
}

XnStatus XnServerSensorInvoker::SetStringProperty( const XnChar* strModule, const XnChar* strProperty, const XnChar* strValue )
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.SetProperty(strModule, strProperty, strValue);
}

XnStatus XnServerSensorInvoker::SetGeneralProperty( const XnChar* strModule, const XnChar* strProperty, const XnGeneralBuffer& gbValue )
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.SetProperty(strModule, strProperty, gbValue);
}

XnStatus XnServerSensorInvoker::GetAllProperties(XnPropertySet* pSet, XnBool bNoStreams, const XnChar* strModule)
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.GetAllProperties(pSet, bNoStreams, strModule);
}

XnStatus XnServerSensorInvoker::LoadConfigFromFile(const XnChar* strFileName, const XnChar* strSectionName)
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.LoadConfigFromFile(strFileName, strSectionName);
}

XnStatus XnServerSensorInvoker::BatchConfig(const XnPropertySet* pChangeSet)
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.BatchConfig(pChangeSet);
}

XnStatus XnServerSensorInvoker::ConfigureModuleFromGlobalFile(const XnChar* strModule)
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.ConfigureModuleFromGlobalFile(strModule);
}

XnStatus XnServerSensorInvoker::CreateStreamData(const XnChar* strStreamName, XnStreamData** ppStreamData)
{
	XnAutoCSLocker lock(m_hSensorLock);
	return m_sensor.CreateStreamData(strStreamName, ppStreamData);
}

XnStatus XnServerSensorInvoker::GetStream(const XnChar* strType, const XnPropertySet* pInitialValues)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check if stream already exists
	XnAutoCSLocker locker(m_hSensorLock);
	SensorInvokerStream* pStream = NULL;
	nRetVal = m_streams.Get(strType, pStream);
	if (nRetVal == XN_STATUS_OK)
	{
		// stream already exists. add ref to it, and try to configure it according to request
		xnLogVerbose(XN_MASK_SENSOR_SERVER, "Stream %s already exists.", strType);

		// configure it
		if (pInitialValues != NULL)
		{
			nRetVal = m_sensor.BatchConfig(pInitialValues);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	else if (nRetVal == XN_STATUS_NO_MATCH)
	{
		// stream doesn't exist. create it (more happens in the OnStreamAdded event handler)
		nRetVal = m_sensor.CreateStream(strType, strType, pInitialValues);
		XN_IS_STATUS_OK(nRetVal);

		// now take it from the hash
		nRetVal = m_streams.Get(strType, pStream);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		return (nRetVal);
	}
	
	++pStream->nRefCount;
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Stream %s now has %u clients.", strType, pStream->nRefCount);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::SetStreamSharedMemory(SensorInvokerStream* pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// give shared memory a name (to make the name unique, we'll add process ID)
	XN_PROCESS_ID procID;
	xnOSGetCurrentProcessID(&procID);
	XnChar strSharedMemoryName[XN_FILE_MAX_PATH];
	sprintf(strSharedMemoryName, "%u_%s_%s", (XnUInt32)procID, m_sensor.GetUSBPath(), pStream->strType);
	nRetVal = pStream->pSharedMemoryName->UnsafeUpdateValue(strSharedMemoryName);
	XN_IS_STATUS_OK(nRetVal);

	XnUInt32 nBufferSize = 0;
	XnUInt32 nPixelSize = 0;

	if (strcmp(pStream->strType, XN_STREAM_TYPE_DEPTH) == 0)
	{
		// have space for depth and shift values
		nPixelSize = sizeof(XnDepthPixel) + sizeof(XnUInt16);
	}
	else if (strcmp(pStream->strType, XN_STREAM_TYPE_IMAGE) == 0)
	{
		// biggest pixel size is the RGB24
		nPixelSize = sizeof(XnRGB24Pixel);
	}
	else if (strcmp(pStream->strType, XN_STREAM_TYPE_IR) == 0)
	{
		// biggest pixel size is the RGB24
		nPixelSize = sizeof(XnIRPixel);
	}
	else
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_ERROR;
	}

	// find out max resolution
	XnUInt32 nMaxNumPixels = 0;
	nRetVal = GetStreamMaxResolution(pStream, nMaxNumPixels);
	XN_IS_STATUS_OK(nRetVal);

	nBufferSize = (XnUInt32)(nMaxNumPixels * nPixelSize * m_numberOfBuffers.GetValue());

	// allocate shared memory
	nRetVal = xnOSCreateSharedMemoryEx(strSharedMemoryName, nBufferSize, XN_OS_FILE_READ | XN_OS_FILE_WRITE, m_allowOtherUsers.GetValue() == TRUE, &pStream->hSharedMemory);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSSharedMemoryGetAddress(pStream->hSharedMemory, (void**)&pStream->pSharedMemoryAddress);
	XN_IS_STATUS_OK(nRetVal);

	// Set buffer pool for this stream
	XnGeneralBuffer* aBuffers = XN_NEW_ARR(XnGeneralBuffer, m_numberOfBuffers.GetValue());
	XnUInt32 nSingleBufferSize = nBufferSize / m_numberOfBuffers.GetValue();
	for (XnUInt32 i = 0; i < m_numberOfBuffers.GetValue(); ++i)
	{
		aBuffers[i].pData = pStream->pSharedMemoryAddress + (i * nSingleBufferSize);
		aBuffers[i].nDataSize = nSingleBufferSize;
	}

	nRetVal = m_sensor.SetProperty(pStream->strType, XN_STREAM_PROPERTY_EXTERNAL_BUFFER_POOL, XnGeneralBufferPack(aBuffers, m_numberOfBuffers.GetValue() * sizeof(XnGeneralBuffer)));
	XN_DELETE_ARR(aBuffers);

	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::GetStreamMaxResolution(SensorInvokerStream* pStream, XnUInt32& nMaxNumPixels)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt64 nCount = 0;
	nRetVal = m_sensor.GetProperty(pStream->strType, XN_STREAM_PROPERTY_SUPPORT_MODES_COUNT, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	XnCmosPreset* aPresets = XN_NEW_ARR(XnCmosPreset, nCount);
	nRetVal = m_sensor.GetProperty(pStream->strType, XN_STREAM_PROPERTY_SUPPORT_MODES, XnGeneralBufferPack(aPresets, nCount * sizeof(XnCmosPreset)));
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE_ARR(aPresets);
		return nRetVal;
	}

	XnUInt32 nMaxPixels = 0;
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		XnUInt32 nXRes;
		XnUInt32 nYRes;
		if (!XnDDKGetXYFromResolution((XnResolutions)aPresets[i].nResolution, &nXRes, &nYRes))
		{
			continue;
		}

		if (nXRes * nYRes > nMaxPixels)
		{
			nMaxPixels = nXRes * nYRes;
		}
	}

	XN_ASSERT(nMaxPixels > 0);

	XN_DELETE_ARR(aPresets);

	nMaxNumPixels = nMaxPixels;
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::ReleaseStream(const XnChar* strType)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnAutoCSLocker locker(m_hSensorLock);
	SensorInvokerStream* pStream = NULL;
	nRetVal = m_streams.Get(strType, pStream);
	XN_IS_STATUS_OK(nRetVal);

	--pStream->nRefCount;
	xnLogVerbose(XN_MASK_SENSOR_SERVER, "Stream %s now has %u clients", strType, pStream->nRefCount);

	if (pStream->nRefCount == 0)
	{
		m_sensor.CloseStream(strType);
		m_sensor.DestroyStream(strType);

		// the rest will be done in the OnStreamRemoved event handler...
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::OpenStream(const XnChar* strName, NewStreamDataHandler pNewDataHandler, void* pCookie, XnCallbackHandle& hCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnAutoCSLocker locker(m_hSensorLock);

	SensorInvokerStream* pStream = NULL;
	nRetVal = m_streams.Get(strName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	// register for new data event
	nRetVal = pStream->pNewDataEvent->Register(pNewDataHandler, pCookie, hCallback);
	XN_IS_STATUS_OK(nRetVal);

	// increase open ref count
	++pStream->nOpenRefCount;

	if (pStream->nOpenRefCount == 1) // first one to open
	{
		// open it
		nRetVal = m_sensor.OpenStream(strName);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_SERVER, "Failed to open stream: %s", xnGetStatusString(nRetVal));
			--pStream->nOpenRefCount;
			pStream->pNewDataEvent->Unregister(hCallback);
			return (nRetVal);
		}
	}

	xnLogInfo(XN_MASK_SENSOR_SERVER, "Stream %s is now open by %u clients.", strName, pStream->nOpenRefCount);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::CloseStream(const XnChar* strName, XnCallbackHandle hCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnAutoCSLocker locker(m_hSensorLock);

	SensorInvokerStream* pStream = NULL;
	nRetVal = m_streams.Get(strName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	// decrease open ref count
	--pStream->nOpenRefCount;

	xnLogInfo(XN_MASK_SENSOR_SERVER, "Stream %s is now open by %u clients.", strName, pStream->nOpenRefCount);

	// check if we actually need to close it
	if (pStream->nOpenRefCount == 0)
	{
		nRetVal = m_sensor.CloseStream(strName);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_SENSOR_SERVER, "Failed to close stream: %s", xnGetStatusString(nRetVal));
			++pStream->nOpenRefCount;
			return (nRetVal);
		}
	}

	// unregister from event
	pStream->pNewDataEvent->Unregister(hCallback);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::AddRefFrameBuffer(const XnChar* strStreamName, XnBuffer* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnBufferPool* pBufferPool = NULL;
	nRetVal = m_sensor.GetBufferPool(strStreamName, &pBufferPool);
	XN_IS_STATUS_OK(nRetVal);

	pBufferPool->AddRef(pBuffer);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::ReleaseFrameBuffer(const XnChar* strStreamName, XnBuffer* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnBufferPool* pBufferPool = NULL;
	nRetVal = m_sensor.GetBufferPool(strStreamName, &pBufferPool);
	XN_IS_STATUS_OK(nRetVal);

	pBufferPool->DecRef(pBuffer);
	
	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::ReadStream(XnStreamData* pStreamData, XnUInt32* pnOffset)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnAutoCSLocker locker(m_hSensorLock);
	SensorInvokerStream* pStream = NULL;
	nRetVal = m_streams.Get(pStreamData->StreamName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	XnBufferPool* pBufferPool = NULL;
	nRetVal = m_sensor.GetBufferPool(pStreamData->StreamName, &pBufferPool);
	XN_IS_STATUS_OK(nRetVal);

	// dec ref old data
	if (pStreamData->pInternal->pLockedBuffer != NULL)
	{
		pBufferPool->DecRef(pStreamData->pInternal->pLockedBuffer);
	}
	
	// "read"
	pStreamData->nDataSize = pStream->pStreamData->nDataSize;
	pStreamData->nFrameID = pStream->pStreamData->nFrameID;
	pStreamData->nTimestamp = pStream->pStreamData->nTimestamp;
	pStreamData->pData = pStream->pStreamData->pData;
	pStreamData->pInternal->pLockedBuffer = pStream->pStreamData->pInternal->pLockedBuffer;

	// add ref to new data
	if (pStreamData->pInternal->pLockedBuffer != NULL)
	{
		pBufferPool->AddRef(pStreamData->pInternal->pLockedBuffer);
	}

	*pnOffset = pStreamData->pInternal->pLockedBuffer->GetData() - pStream->pSharedMemoryAddress;

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::SetNumberOfBuffers(XnUInt32 nCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// This is a special property. It can only be changed BEFORE reading starts
	if (m_sensor.HasReadingStarted() && nCount != m_numberOfBuffers.GetValue())
	{
		return (XN_STATUS_DEVICE_PROPERTY_READ_ONLY);
	}

	if (nCount < 3)
	{
		return XN_STATUS_BAD_PARAM;
	}

	nRetVal = m_numberOfBuffers.UnsafeUpdateValue(nCount);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::SetAllowOtherUsers(XnBool bAllowOtherUsers)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// This is a special property. It can only be changed BEFORE reading starts
	if (m_sensor.HasReadingStarted() && m_allowOtherUsers.GetValue() != bAllowOtherUsers)
	{
		return (XN_STATUS_DEVICE_PROPERTY_READ_ONLY);
	}

	nRetVal = m_allowOtherUsers.UnsafeUpdateValue(bAllowOtherUsers);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::OnPropertyChanged(const XnProperty* pProp)
{
	// some special handling
	if (strcmp(pProp->GetName(), XN_STREAM_PROPERTY_STATE) == 0)
	{
		// ignore STATE property (every client has its own value)
		return XN_STATUS_OK;
	}
	else if (strcmp(pProp->GetName(), XN_MODULE_PROPERTY_ERROR_STATE) == 0)
	{
		XnActualIntProperty* pActualIntProp = (XnActualIntProperty*)pProp;
		XnStatus nOldErrorState = m_errorState;
		m_errorState = (XnStatus)pActualIntProp->GetValue();
		switch (m_errorState)
		{
		case XN_STATUS_DEVICE_NOT_CONNECTED:
			//TODO: Handle disconnection
			break;
		case XN_STATUS_OK:
			if (nOldErrorState == XN_STATUS_DEVICE_NOT_CONNECTED)
			{
				//TODO: Handle re-connection
				break;
			}
		}
	}

	// raise event
	m_propChangedEvent.Raise(pProp);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::OnStreamAdded(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get all props
	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	nRetVal = m_sensor.GetAllProperties(&props, FALSE, StreamName);
	XN_IS_STATUS_OK(nRetVal);

	// register to all props
	nRetVal = RegisterToProps(&props);
	XN_IS_STATUS_OK(nRetVal);

	XnActualPropertiesHash* pStreamProps = props.pData->Begin()->Value();

	// create stream data
	SensorInvokerStream serverStream;
	xnOSMemSet(&serverStream, 0, sizeof(serverStream));
	strcpy(serverStream.strType, StreamName);

	XN_VALIDATE_NEW(serverStream.pNewDataEvent, NewStreamDataEvent);

	// check if this is a frame stream
	XnProperty* pIsFrameBased;
	nRetVal = pStreamProps->Get(XN_STREAM_PROPERTY_IS_FRAME_BASED, pIsFrameBased);
	if (nRetVal == XN_STATUS_OK)
	{
		XnActualIntProperty* pIntProp = (XnActualIntProperty*)pIsFrameBased;
		serverStream.bFrameStream = (pIntProp->GetValue() == TRUE);
	}

	if (serverStream.bFrameStream)
	{
		// create the "shared memory name" property
		XN_VALIDATE_NEW(serverStream.pSharedMemoryName, XnActualStringProperty, XN_STREAM_PROPERTY_SHARED_BUFFER_NAME);

		// and add it to the stream
		XnDeviceStream* pStream;
		nRetVal = m_sensor.GetStream(StreamName, &pStream);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(serverStream.pNewDataEvent);
			XN_DELETE(serverStream.pSharedMemoryName);
			return nRetVal;
		}

		nRetVal = pStream->AddProperty(serverStream.pSharedMemoryName);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(serverStream.pNewDataEvent);
			XN_DELETE(serverStream.pSharedMemoryName);
			return nRetVal;
		}

		// create a shared memory buffer pool for it
		nRetVal = SetStreamSharedMemory(&serverStream);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(serverStream.pNewDataEvent);
			XN_DELETE(serverStream.pSharedMemoryName);
			return nRetVal;
		}
	}

	// create a stream data object for the stream
	nRetVal = m_sensor.CreateStreamData(StreamName, &serverStream.pStreamData);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(serverStream.pNewDataEvent);
		XN_DELETE(serverStream.pSharedMemoryName);
		return (nRetVal);
	}

	// and add it to our list of streams
	nRetVal = m_streams.Set(StreamName, serverStream);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(serverStream.pNewDataEvent);
		XN_DELETE(serverStream.pSharedMemoryName);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::OnStreamRemoved(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// no need to unregister from its props - they do not exist anymore.

	// remove stream data
	SensorInvokerStream* pServerStream = NULL;
	nRetVal = m_streams.Get(StreamName, pServerStream);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_sensor.DestroyStreamData(&pServerStream->pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	XN_DELETE(pServerStream->pNewDataEvent);

	if (pServerStream->pSharedMemoryName != NULL)
	{
		XN_DELETE(pServerStream->pSharedMemoryName);
	}

	// destroy shared memory
	xnOSCloseSharedMemory(pServerStream->hSharedMemory);

	nRetVal = m_streams.Remove(StreamName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::OnStreamCollectionChanged(const XnChar* StreamName, XnStreamsChangeEventType EventType)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (EventType)
	{
	case XN_DEVICE_STREAM_ADDED:
		{
			nRetVal = OnStreamAdded(StreamName);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	case XN_DEVICE_STREAM_DELETED:
		{
			nRetVal = OnStreamRemoved(StreamName);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_SENSOR_SERVER, "unknown event: %d", EventType);
	}

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::OnNewStreamData(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// no need to lock the sensor (this might cause a dead lock).
	// Instead, only lock the streams collection (so it wouldn't change while we search for the stream)
	SensorInvokerStream* pStream = NULL;
	nRetVal = m_streams.Get(StreamName, pStream);
	XN_IS_STATUS_OK(nRetVal);

	pStream->bNewData = TRUE;

	nRetVal = xnOSSetEvent(m_hNewDataEvent);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnServerSensorInvoker::ReadStreams()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// wait for new data to be available
	nRetVal = xnOSWaitEvent(m_hNewDataEvent, XN_NODE_WAIT_FOR_DATA_TIMEOUT);
	if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
	{
		return XN_STATUS_OK;
	}
	else if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SENSOR_SERVER, "Got error waiting for new data event: %s", xnGetStatusString(nRetVal));
		// but continue anyway
	}

	// lock sensor (we iterate over streams list. make sure no stream is added/removed from the list)
	{
		XnLockedServerStreamsHash lockedHash = m_streams.GetLockedHashForIterating();
		for (XnLockedServerStreamsHash::Iterator it = lockedHash.Begin(); it != lockedHash.End(); ++it)
		{
			SensorInvokerStream& stream = it->Value();

			if (stream.bNewData)
			{
				// ignore audio (it is read by every client)
				if (strcmp(stream.strType, XN_STREAM_NAME_AUDIO) != 0)
				{
					// read this data
					nRetVal = m_sensor.ReadStream(stream.pStreamData);
					if (nRetVal != XN_STATUS_OK)
					{
						xnLogWarning(XN_MASK_SENSOR_SERVER, "Failed reading from stream %s (though event was raised): %s", stream.strType, xnGetStatusString(nRetVal));
						stream.bNewData = FALSE;
						continue;
					}
				}

				stream.bNewData = FALSE;

				NewStreamDataEventArgs eventArgs;
				eventArgs.strStreamName = stream.strType;
				eventArgs.nTimestamp = stream.pStreamData->nTimestamp;
				eventArgs.nFrameID = stream.pStreamData->nFrameID;
				stream.pNewDataEvent->Raise(eventArgs);
			}
		} // streams loop
	} // lock

	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnServerSensorInvoker::SetNumberOfBuffersCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnServerSensorInvoker* pThis = (XnServerSensorInvoker*)pCookie;
	return pThis->SetNumberOfBuffers((XnUInt32)nValue);
}

XnStatus XN_CALLBACK_TYPE XnServerSensorInvoker::SetAllowOtherUsersCallback(XnActualIntProperty* /*pSender*/, XnUInt64 nValue, void* pCookie)
{
	XnServerSensorInvoker* pThis = (XnServerSensorInvoker*)pCookie;
	return pThis->SetAllowOtherUsers(nValue == TRUE);
}

XnStatus XN_CALLBACK_TYPE XnServerSensorInvoker::PropertyChangedCallback(const XnProperty* pProp, void* pCookie)
{
	XnServerSensorInvoker* pThis = (XnServerSensorInvoker*)pCookie;
	pThis->OnPropertyChanged(pProp);
	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE XnServerSensorInvoker::StreamCollectionChangedCallback(const XnStreamCollectionChangedEventArgs& args, void* pCookie)
{
	XnServerSensorInvoker* pThis = (XnServerSensorInvoker*)pCookie;
	pThis->OnStreamCollectionChanged(args.strStreamName, args.eventType);
}

void XN_CALLBACK_TYPE XnServerSensorInvoker::NewStreamDataCallback(const XnNewStreamDataEventArgs& args, void* pCookie)
{
	XnServerSensorInvoker* pThis = (XnServerSensorInvoker*)pCookie;
	pThis->OnNewStreamData(args.strStreamName);
}

XN_THREAD_PROC XnServerSensorInvoker::ReaderThread(XN_THREAD_PARAM pThreadParam)
{
	XnServerSensorInvoker* pThis = (XnServerSensorInvoker*)pThreadParam;
	while (pThis->m_bShouldRun)
	{
		pThis->ReadStreams();
	}
	XN_THREAD_PROC_RETURN(0);
}

