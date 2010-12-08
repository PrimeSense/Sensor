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
#include "XnDeviceBase.h"
#include <XnOS.h>
#include <XnLog.h>
#include "XnIntProperty.h"
#include "XnRealProperty.h"
#include "XnStringProperty.h"
#include "XnGeneralProperty.h"
#include "XnPropertySetInternal.h"
#include <XnPsVersion.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DUMP_STREAMS_DATA	"StreamsData"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnWaitForPrimaryData
{
	XnDeviceBase* pThis;
	XnStreamDataSet* pSet;
} XnWaitForPrimaryData;

typedef struct XnWaitForStreamData
{
	XnDeviceBase* pThis;
	XnDeviceStream* pStream;
} XnWaitForStreamData;

typedef struct XnPropertyCallback
{
	XnPropertyCallback(XnDeviceHandle DeviceHandle, const XnChar* strModule, const XnChar* strProp, XnDeviceOnPropertyChangedEventHandler pHandler, void* pCookie) :
		DeviceHandle(DeviceHandle),
		pHandler(pHandler),
		pCookie(pCookie)
	{
		strcpy(this->strModule, strModule);
		strcpy(this->strProp, strProp);
	}

	XnDeviceHandle DeviceHandle;
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnChar strProp[XN_DEVICE_MAX_STRING_LENGTH];
	XnDeviceOnPropertyChangedEventHandler pHandler;
	void* pCookie;
	XnCallbackHandle hCallback;

} XnPropertyCallback;

//---------------------------------------------------------------------------
// Public Methods
//---------------------------------------------------------------------------
XnDeviceBase::XnDeviceBase(const XnChar* csName, XnBool bStrictProperties) :
	m_bStrictProperties(bStrictProperties),
	m_pDevicePropertiesHolder(NULL),
	m_DeviceName(XN_MODULE_PROPERTY_DEVICE_NAME, csName),
	m_ReadWriteMode(XN_MODULE_PROPERTY_READ_WRITE_MODE, XN_DEVICE_MODE_READ),
	m_PrimaryStream(XN_MODULE_PROPERTY_PRIMARY_STREAM, XN_PRIMARY_STREAM_ANY),
	m_DeviceMirror(XN_MODULE_PROPERTY_MIRROR, FALSE),
	m_HighResTimestamps(XN_MODULE_PROPERTY_HIGH_RES_TIMESTAMPS, TRUE),
	m_SDKVersionProp(XN_MODULE_PROPERTY_SDK_VERSION, &m_SDKVersion, sizeof(XnSDKVersion)),
	m_SharingMode(XN_MODULE_PROPERTY_SHARE_MODE, XN_DEVICE_EXCLUSIVE),
	m_hNewDataEvent(NULL),
	m_nLastReadTimestamp(0),
	m_nLastReadFrameID(0),
	m_nLastTimestamp(0),
	m_nLastFrameID(0),
	m_StreamsDataDump(XN_DUMP_CLOSED)
{
	// update set callbacks
	m_PrimaryStream.UpdateSetCallback(SetPrimaryStreamCallback, this);
	m_DeviceMirror.UpdateSetCallback(SetMirrorCallback, this);
	m_HighResTimestamps.UpdateSetCallback(SetHighresTimestampsCallback, this);

	m_SDKVersion.nMajor = XN_PS_MAJOR_VERSION;
	m_SDKVersion.nMinor = XN_PS_MINOR_VERSION;
	m_SDKVersion.nMaintenance = XN_PS_MAINTENANCE_VERSION;
	m_SDKVersion.nBuild = XN_PS_BUILD_VERSION;
}

XnDeviceBase::~XnDeviceBase()
{
}

XnStatus XnDeviceBase::Init(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// first init the device
	nRetVal = InitImpl(pDeviceConfig);
	XN_IS_STATUS_OK(nRetVal);

	// and now create streams
	if (pDeviceConfig->pInitialValues != NULL)
	{
		nRetVal = CreateStreams(pDeviceConfig->pInitialValues);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::InitImpl(const XnDeviceConfig* pDeviceConfig)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDeviceConfig);

	// create device module
	nRetVal = CreateDeviceModule(&m_pDevicePropertiesHolder);
	XN_IS_STATUS_OK(nRetVal);

	// check if we have initial values for device modules
	XnActualPropertiesHash* pDeviceModuleInitialProps = NULL;
	if (pDeviceConfig->pInitialValues != NULL)
	{
		pDeviceConfig->pInitialValues->pData->Get(XN_MODULE_NAME_DEVICE, pDeviceModuleInitialProps);
	}

	// init device module
	nRetVal = m_pDevicePropertiesHolder->Init(pDeviceModuleInitialProps);
	XN_IS_STATUS_OK(nRetVal);

	// set read/write mode (we need to do it AFTER module init to override original value)
	nRetVal = m_ReadWriteMode.UnsafeUpdateValue(pDeviceConfig->DeviceMode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_SharingMode.UnsafeUpdateValue(pDeviceConfig->SharingMode);
	XN_IS_STATUS_OK(nRetVal);

	// add the device module
	nRetVal = AddModule(m_pDevicePropertiesHolder);
	XN_IS_STATUS_OK(nRetVal);

	// create the new data event
	nRetVal = xnOSCreateEvent(&m_hNewDataEvent, FALSE);
	XN_IS_STATUS_OK(nRetVal);

	// init dump
	xnDumpInit(&m_StreamsDataDump, XN_DUMP_STREAMS_DATA, "", "%s.csv", XN_DUMP_STREAMS_DATA);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::Destroy()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// free all modules
	while (m_Modules.Size() != 0)
	{
		XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)m_Modules.begin().Value();
		if (IsStream(pModuleHolder->GetModule()))
		{
			XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];
			strcpy(strName, pModuleHolder->GetModule()->GetName());
			nRetVal = DestroyStream(strName);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			// free memory of registered properties to this module
			FreeModuleRegisteredProperties(m_Modules.begin().Key());

			pModuleHolder->GetModule()->Free();
			DestroyModule(pModuleHolder);
			m_Modules.Remove(m_Modules.begin());
		}
	}

	m_pDevicePropertiesHolder = NULL;

	m_Modules.Clear();

	// close event
	xnOSCloseEvent(&m_hNewDataEvent);

	// close dump
	xnDumpClose(&m_StreamsDataDump);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::CreateModule(const XnChar* strName, XnDeviceModuleHolder** ppModuleHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnDeviceModule* pModule;
	XnDeviceModuleHolder* pHolder;

	// create module
	XN_VALIDATE_NEW(pModule, XnDeviceModule, strName);

	// create holder
	pHolder = XN_NEW(XnDeviceModuleHolder, pModule, !m_bStrictProperties);
	if (pHolder == NULL)
	{
		XN_DELETE(pModule);
		return XN_STATUS_ALLOC_FAILED;
	}

	*ppModuleHolder = pHolder;

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// create module
	nRetVal = CreateModule(XN_MODULE_NAME_DEVICE, ppModuleHolder);
	XN_IS_STATUS_OK(nRetVal);

	XnDeviceModule* pModule = (*ppModuleHolder)->GetModule();

	// add device properties
	XnProperty* pProps[] = { &m_ReadWriteMode, &m_SharingMode, &m_PrimaryStream, &m_DeviceMirror, 
		&m_SDKVersionProp, &m_HighResTimestamps, &m_DeviceName };

	nRetVal = pModule->AddProperties(pProps, sizeof(pProps)/sizeof(XnProperty*));
	if (nRetVal != XN_STATUS_OK)
	{
		DestroyModule(*ppModuleHolder);
		*ppModuleHolder = NULL;
		return (nRetVal);
	}

	return XN_STATUS_OK;
}

void XnDeviceBase::DestroyModule(XnDeviceModuleHolder* pModuleHolder)
{
	XN_DELETE(pModuleHolder->GetModule());
	XN_DELETE(pModuleHolder);
}

XnStatus XnDeviceBase::SetPrimaryStream(const XnChar* strPrimaryStream)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strcmp(strPrimaryStream, XN_PRIMARY_STREAM_ANY) != 0 &&
		strcmp(strPrimaryStream, XN_PRIMARY_STREAM_NONE) != 0)
	{
		// specific stream. check it exists
		XnDeviceStream* pStream;
		nRetVal = FindStream(strPrimaryStream, &pStream);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_UNSUPPORTED_STREAM;
		}
	}

	// OK. set the value
	nRetVal = m_PrimaryStream.UnsafeUpdateValue(strPrimaryStream);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::SetMirror(XnBool bMirror)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// change all streams
	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)it.Value();
		if (IsStream(pModuleHolder->GetModule()))
		{
			XnDeviceStream* pStream = (XnDeviceStream*)pModuleHolder->GetModule();
			nRetVal = pStream->SetMirror(bMirror);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	// and set property
	nRetVal = m_DeviceMirror.UnsafeUpdateValue(bMirror);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::SetHighresTimestamps(XnBool bHighRes)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_HighResTimestamps.UnsafeUpdateValue(bHighRes);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::GetSupportedStreams(const XnChar** aStreamNames, XnUInt32* pnStreamNamesCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OUTPUT_PTR(pnStreamNamesCount);
	// NOTE: we allow aStreamName to be NULL

	// first of all count streams
	XnUInt32 nStreamsCount = m_SupportedStreams.Size();

	// now check if we have enough room
	if (nStreamsCount > *pnStreamNamesCount)
	{
		*pnStreamNamesCount = nStreamsCount;
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	// now copy values
	nStreamsCount = 0;
	for (XnStringsHash::Iterator it = m_SupportedStreams.begin(); it != m_SupportedStreams.end(); ++it)
	{
		aStreamNames[nStreamsCount] = it.Key();
		nStreamsCount++;
	}

	*pnStreamNamesCount = nStreamsCount;
	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::OpenStream(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(StreamName);

	xnLogVerbose(XN_MASK_DDK, "Opening stream %s...", StreamName);

	// find this stream
	XnDeviceStream* pStream;
	nRetVal = FindStream(StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);

	// open it
	nRetVal = pStream->Open();
	XN_IS_STATUS_OK(nRetVal);

	xnLogInfo(XN_MASK_DDK, "Stream %s is open.", StreamName);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::CloseStream(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(StreamName);

	xnLogVerbose(XN_MASK_DDK, "Closing stream %s...", StreamName);

	// find this stream
	XnDeviceStream* pStream;
	nRetVal = FindStream(StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);

	// close it
	nRetVal = pStream->Close();
	XN_IS_STATUS_OK(nRetVal);

	xnLogInfo(XN_MASK_DDK, "Stream %s is closed.", StreamName);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::OpenAllStreams()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_DDK, "Opening all streams...");

	// go over modules list, and look for closed streams
	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)it.Value();
		if (IsStream(pModuleHolder->GetModule()))
		{
			XnDeviceStream* pStream = (XnDeviceStream*)pModuleHolder->GetModule();
			if (!pStream->IsOpen())
			{
				nRetVal = pStream->Open();
				XN_IS_STATUS_OK(nRetVal);
			}
		}
	}

	xnLogInfo(XN_MASK_DDK, "All streams are open.");

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::CloseAllStreams()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_DDK, "Closing all streams...");

	// go over modules list, and look for closed streams
	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		const XnChar* Name = it.Key();
		XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)it.Value();
		if (IsStream(pModuleHolder->GetModule()))
		{
			XnDeviceStream* pStream = (XnDeviceStream*)pModuleHolder->GetModule();
			if (pStream->IsOpen())
			{
				nRetVal = pStream->Close();
				XN_IS_STATUS_OK(nRetVal);
			}
		}
	}

	xnLogInfo(XN_MASK_DDK, "All streams are closed.");

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::GetStreamNames(const XnChar** pstrNames, XnUInt32* pnNamesCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// first we need to count them
	XnUInt32 nCount = 0;

	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)it.Value();
		if (IsStream(pModuleHolder->GetModule()))
		{
			nCount++;
		}
	}

	if (nCount > *pnNamesCount)
	{
		*pnNamesCount = nCount;
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	// OK. we have enough space. Copy into it
	nCount = 0;
	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)it.Value();
		if (IsStream(pModuleHolder->GetModule()))
		{
			pstrNames[nCount] = it.Key();
			nCount++;
		}
	}

	*pnNamesCount = nCount;

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::DoesModuleExist(const XnChar* ModuleName, XnBool* pbDoesExist)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(ModuleName);
	XN_VALIDATE_OUTPUT_PTR(pbDoesExist);

	*pbDoesExist = FALSE;

	XnDeviceModuleHolder* pModuleHolder;
	nRetVal = FindModule(ModuleName, &pModuleHolder);
	if (nRetVal == XN_STATUS_OK)
	{
		*pbDoesExist = TRUE;
	}
	else if (nRetVal != XN_STATUS_DEVICE_MODULE_NOT_FOUND)
	{
		return nRetVal;
	}

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::RegisterToStreamsChange(XnDeviceOnStreamsChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(Handler);

	return m_OnStreamsChangeEvent.Register((StreamCollectionChangedEvent::HandlerPtr)Handler, pCookie, phCallback);
}

XnStatus XnDeviceBase::UnregisterFromStreamsChange(XnCallbackHandle hCallback)
{
	XN_VALIDATE_INPUT_PTR(hCallback);

	return m_OnStreamsChangeEvent.Unregister(hCallback);
}

XnStatus XnDeviceBase::CreateStreamData(const XnChar* StreamName, XnStreamData** ppStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(StreamName);
	XN_VALIDATE_OUTPUT_PTR(ppStreamData);

	// find stream
	XnDeviceStream* pStream;
	nRetVal = FindStream(StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);

	// and create stream output
	nRetVal = pStream->CreateStreamData(ppStreamData);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::DestroyStreamData(XnStreamData** ppStreamData)
{
	XN_VALIDATE_INPUT_PTR(ppStreamData);

	return XnStreamDataDestroy(ppStreamData);
}

XnStatus XnDeviceBase::RegisterToNewStreamData(XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(Handler);

	return m_OnNewStreamDataEvent.Register(Handler, pCookie, phCallback);
}

XnStatus XnDeviceBase::UnregisterFromNewStreamData(XnCallbackHandle hCallback)
{
	XN_VALIDATE_INPUT_PTR(hCallback);

	return m_OnNewStreamDataEvent.Unregister(hCallback);
}

XnStatus XnDeviceBase::IsNewDataAvailable(const XnChar* StreamName, XnBool* pbNewDataAvailable, XnUInt64* pnTimestamp)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(StreamName);
	XN_VALIDATE_OUTPUT_PTR(pbNewDataAvailable);

	*pbNewDataAvailable = FALSE;

	if (strcmp(StreamName, XN_PRIMARY_STREAM_ANY) == 0)
	{
		const XnChar* aStreamNames[100];
		XnUInt32 nCount = 100;

		nRetVal = GetStreamNames(aStreamNames, &nCount);
		XN_IS_STATUS_OK(nRetVal);

		for (XnUInt32 i = 0; i < nCount; ++i)
		{
			// find stream
			XnDeviceStream* pStream = NULL;
			nRetVal = FindStream(StreamName, &pStream);
			XN_IS_STATUS_OK(nRetVal);

			if (pStream->IsNewDataAvailable())
			{
				*pbNewDataAvailable = TRUE;
				*pnTimestamp = pStream->GetLastTimestamp();
				break;
			}
		}
	}
	else
	{
		// find stream
		XnDeviceStream* pStream = NULL;
		nRetVal = FindStream(StreamName, &pStream);
		XN_IS_STATUS_OK(nRetVal);

		if (pStream->IsNewDataAvailable())
		{
			*pbNewDataAvailable = TRUE;
			*pnTimestamp = pStream->GetLastTimestamp();
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::ReadFromStreamImpl(XnDeviceStream* pStream, XnStreamData* pStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read
	nRetVal = pStream->Read(pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	if (pStreamData->bIsNew)
	{
		if (strcmp(m_PrimaryStream.GetValue(), XN_PRIMARY_STREAM_ANY) == 0 ||
			strcmp(m_PrimaryStream.GetValue(), XN_PRIMARY_STREAM_NONE) == 0)
		{
			// when primary stream is any, any stream read makes the device advance
			m_nLastReadTimestamp = XN_MAX(pStreamData->nTimestamp, m_nLastReadTimestamp);
			m_nLastReadFrameID = XN_MAX(pStreamData->nFrameID, m_nLastReadFrameID);
		}
		else if (strcmp(m_PrimaryStream.GetValue(), pStream->GetName()) == 0)
		{
			// this is the primary stream
			m_nLastReadTimestamp = pStreamData->nTimestamp;
			m_nLastReadFrameID = pStreamData->nFrameID;
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::ReadStream(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pStreamOutput);

	if (m_ReadWriteMode.GetValue() == XN_DEVICE_MODE_WRITE)
	{
		return XN_STATUS_IO_DEVICE_WRONG_MODE;
	}

	// take the stream to read
	XnDeviceStream* pStream;
	nRetVal = FindStream(pStreamOutput->StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);

	// make sure it is open
	if (!pStream->IsNewDataAvailable() && !pStream->IsOpen())
	{
		return XN_STATUS_STREAM_NOT_OPEN;
	}

	// wait for it to advance
	nRetVal = WaitForStream(m_hNewDataEvent, pStream);
	XN_IS_STATUS_OK(nRetVal);

	// and read
	nRetVal = ReadFromStreamImpl(pStream, pStreamOutput);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::Read(XnStreamDataSet* pStreamOutputSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);

	if (m_ReadWriteMode.GetValue() == XN_DEVICE_MODE_WRITE)
	{
		return XN_STATUS_IO_DEVICE_WRONG_MODE;
	}

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpWriteString(m_StreamsDataDump, "%llu,Read Called\n", nNow);

	// First thing to do is wait for primary stream to advance. We do this BEFORE checking streams
	// because device streams might change during this wait.
	nRetVal = WaitForPrimaryStream(m_hNewDataEvent, pStreamOutputSet);
	XN_IS_STATUS_OK(nRetVal);

	xnOSGetHighResTimeStamp(&nNow);
	xnDumpWriteString(m_StreamsDataDump, "%llu,Read Condition Met\n", nNow);

	// take the list of stream output objects
	XnStreamData* apStreamOutputs[XN_DEVICE_BASE_MAX_STREAMS_COUNT];
	XnUInt32 nOutputsCount = XN_DEVICE_BASE_MAX_STREAMS_COUNT;

	nRetVal = XnStreamDataSetCopyToArray(pStreamOutputSet, apStreamOutputs, &nOutputsCount);
	XN_IS_STATUS_OK(nRetVal);

	// now read
	for (XnUInt32 nIndex = 0; nIndex < nOutputsCount; ++nIndex)
	{
		// find its corresponding stream
		XnDeviceStream* pStream;
		nRetVal = FindStream(apStreamOutputs[nIndex]->StreamName, &pStream);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = ReadFromStreamImpl(pStream, apStreamOutputs[nIndex]);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::WriteStream(XnStreamData* pStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pStreamData);

	if (m_ReadWriteMode.GetValue() != XN_DEVICE_MODE_WRITE)
	{
		return XN_STATUS_IO_DEVICE_WRONG_MODE;
	}

	// take the stream to write
	XnDeviceStream* pStream;
	nRetVal = FindStream(pStreamData->StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);

	// make sure it is open
	if (!pStream->IsOpen())
	{
		return XN_STATUS_STREAM_NOT_OPEN;
	}

	// and write
	nRetVal = pStream->Write(pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::Write(XnStreamDataSet* pStreamDataSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pStreamDataSet);

	if (m_ReadWriteMode.GetValue() != XN_DEVICE_MODE_WRITE)
	{
		return XN_STATUS_IO_DEVICE_WRONG_MODE;
	}

	// take the list of stream output objects
	XnStreamData* apStreamOutputs[XN_DEVICE_BASE_MAX_STREAMS_COUNT];
	XnUInt32 nOutputsCount = XN_DEVICE_BASE_MAX_STREAMS_COUNT;

	nRetVal = XnStreamDataSetCopyToArray(pStreamDataSet, apStreamOutputs, &nOutputsCount);
	XN_IS_STATUS_OK(nRetVal);

	// find the stream for each one
	XnDeviceStream* apStreams[XN_DEVICE_BASE_MAX_STREAMS_COUNT];

	for (XnUInt32 nIndex = 0; nIndex < nOutputsCount; ++nIndex)
	{
		// find its corresponding stream
		nRetVal = FindStream(apStreamOutputs[nIndex]->StreamName, &apStreams[nIndex]);
		XN_IS_STATUS_OK(nRetVal);

		// make sure it is open
		if (!apStreams[nIndex]->IsOpen())
		{
			return XN_STATUS_STREAM_NOT_OPEN;
		}
	}

	// go over them, and write into each one
	for (XnUInt32 nIndex = 0; nIndex < nOutputsCount; ++nIndex)
	{
		nRetVal = apStreams[nIndex]->Write(apStreamOutputs[nIndex]);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::Tell(XnUInt64* pnTimestamp)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OUTPUT_PTR(pnTimestamp);

	*pnTimestamp = m_nLastTimestamp;

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::TellFrame(XnUInt32* pnFrameID)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_OUTPUT_PTR(pnFrameID);

	*pnFrameID = m_nLastFrameID;
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::DoesPropertyExist(const XnChar* ModuleName, const XnChar* PropertyName, XnBool* pbDoesExist)
{
	XnStatus nRetVal = XN_STATUS_OK;

	*pbDoesExist = FALSE;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	if (nRetVal == XN_STATUS_DEVICE_MODULE_NOT_FOUND)
	{
		return XN_STATUS_OK;
	}
	else
	{
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = pModule->DoesPropertyExist(PropertyName, pbDoesExist);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::GetPropertyType(const XnChar* ModuleName, const XnChar* PropertyName, XnPropertyType* pnType)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->GetPropertyType(PropertyName, pnType);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->SetProperty(PropertyName, nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->SetProperty(PropertyName, dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->SetProperty(PropertyName, csValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->SetProperty(PropertyName, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->GetProperty(PropertyName, pnValue);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->GetProperty(PropertyName, pdValue);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnChar* csValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->GetProperty(PropertyName, csValue);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModule* pModule;
	nRetVal = FindModule(ModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = pModule->GetProperty(PropertyName, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::CreateStreamsFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// go through numbers until no stream is found
	XnUInt32 nStreamIndex = 0;
	XnChar csKeyName[XN_INI_MAX_LEN];

	while (TRUE)
	{
		sprintf(csKeyName, "Stream%d_Type", nStreamIndex);

		XnChar csStreamType[XN_INI_MAX_LEN];
		XnChar csStreamName[XN_INI_MAX_LEN];

		nRetVal = xnOSReadStringFromINI(csINIFilePath, csSectionName, csKeyName, csStreamType, XN_INI_MAX_LEN);
		if (nRetVal != XN_STATUS_OK) // no more streams
		{
			break;
		}

		// check if the stream has a name
		sprintf(csKeyName, "Stream%d_Name", nStreamIndex);
		nRetVal = xnOSReadStringFromINI(csINIFilePath, csSectionName, csKeyName, csStreamName, XN_INI_MAX_LEN);
		if (nRetVal != XN_STATUS_OK)
		{
			// use its type as a name
			strcpy(csStreamName, csStreamType);
		}

		nRetVal = CreateStream(csStreamType, csStreamName);
		XN_IS_STATUS_OK(nRetVal);

		nStreamIndex++;
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = DeviceModule()->LoadConfigFromFile(csINIFilePath, csSectionName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::StartTransaction()
{
	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::CommitTransaction()
{
	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::RollbackTransaction()
{
	return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED;
}

#define XN_CHECK_RC_ROLLBACK(rc)	\
	if (rc != XN_STATUS_OK)			\
	{								\
		RollbackTransaction();		\
		return (rc);				\
	}

XnStatus XnDeviceBase::BatchConfig(const XnPropertySet* pChangeSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pChangeSet);

	// start a transaction
	nRetVal = StartTransaction();
	XN_IS_STATUS_OK(nRetVal);

	for (XnPropertySetData::ConstIterator itModule = pChangeSet->pData->begin(); itModule != pChangeSet->pData->end(); ++itModule)
	{
		// find this module
		XnDeviceModule* pModule = NULL;
		nRetVal = FindModule(itModule.Key(), &pModule);
		XN_CHECK_RC_ROLLBACK(nRetVal);

		nRetVal = pModule->BatchConfig(*itModule.Value());
		XN_CHECK_RC_ROLLBACK(nRetVal);
	}

	nRetVal = CommitTransaction();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::GetAllProperties(XnPropertySet* pSet, XnBool bNoStreams /* = FALSE */, const XnChar* strModule /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSet);

	// clear the set
	nRetVal = XnPropertySetClear(pSet);
	XN_IS_STATUS_OK(nRetVal);

	if (strModule != NULL)
	{
		XnDeviceModule* pModule;
		nRetVal = FindModule(strModule, &pModule);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = pModule->GetAllProperties(pSet);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// enumerate over modules
		for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
		{
			XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)it.Value();

			if (bNoStreams && IsStream(pModuleHolder->GetModule()))
				continue;

			nRetVal = pModuleHolder->GetModule()->GetAllProperties(pSet);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::RegisterToPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDeviceModule* pModule;
	nRetVal = FindModule(Module, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	XnPropertyCallback* pRealCookie;
	XN_VALIDATE_NEW(pRealCookie, XnPropertyCallback, GetDeviceHandle(), Module, PropertyName, Handler, pCookie);

	// register
	nRetVal = pModule->RegisterForOnPropertyValueChanged(PropertyName, PropertyValueChangedCallback, pRealCookie, &pRealCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pRealCookie);
		return (nRetVal);
	}

	m_PropertyCallbacks.AddLast(pRealCookie);

	*phCallback = pRealCookie;
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::UnregisterFromPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnCallbackHandle hCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(Module);
	XN_VALIDATE_INPUT_PTR(PropertyName);
	XN_VALIDATE_INPUT_PTR(hCallback);

	XnPropertyCallback* pRealCookie = (XnPropertyCallback*)hCallback;

	XnDeviceModule* pModule;
	nRetVal = FindModule(Module, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// first unregister it from property
	nRetVal = pModule->UnregisterFromOnPropertyValueChanged(PropertyName, pRealCookie->hCallback);
	XN_IS_STATUS_OK(nRetVal);

	XnValue val = pRealCookie;
	XnList::Iterator it = m_PropertyCallbacks.Find(val);
	if (it != m_PropertyCallbacks.end())
	{
		m_PropertyCallbacks.Remove(it);
	}

	// now free the memory
	XN_DELETE(pRealCookie);

	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Protected Helper Methods
//---------------------------------------------------------------------------

XnStatus XnDeviceBase::AddModule(XnDeviceModuleHolder* pModuleHolder)
{
	XnDeviceModule* pModule = pModuleHolder->GetModule();

	// make sure module doesn't exist yet
	XnStringsHash::Iterator it = m_Modules.end();
	if (XN_STATUS_OK == m_Modules.Find(pModule->GetName(), it))
	{
		xnLogError(XN_MASK_DEVICE, "A module with the name %s already exists!", pModule->GetName());
		return XN_STATUS_ERROR;
	}

	// add it to the list
	XnStatus nRetVal = m_Modules.Set(pModule->GetName(), pModuleHolder);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::RemoveModule(const XnChar* ModuleName)
{
	// remove it
	XnValue props;
	XnStatus nRetVal = m_Modules.Remove(ModuleName, props);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::FindModule(const XnChar* ModuleName, XnDeviceModule** ppModule)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnDeviceModuleHolder* pHolder;
	nRetVal = FindModule(ModuleName, &pHolder);
	XN_IS_STATUS_OK(nRetVal);

	*ppModule = pHolder->GetModule();
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::FindModule(const XnChar* ModuleName, XnDeviceModuleHolder** ppModuleHolder)
{
	XnStringsHash::Iterator it = m_Modules.end();
	XnStatus nRetVal = m_Modules.Find(ModuleName, it);
	if (nRetVal == XN_STATUS_NO_MATCH)
	{
		return (XN_STATUS_DEVICE_MODULE_NOT_FOUND);
	}
	XN_IS_STATUS_OK(nRetVal);

	*ppModuleHolder = (XnDeviceModuleHolder*)it.Value();

	return XN_STATUS_OK;
}

XnBool XnDeviceBase::IsStream(XnDeviceModule* pModule)
{
	XnProperty* pProperty;
	XnStatus nRetVal = pModule->GetProperty(XN_STREAM_PROPERTY_IS_STREAM, &pProperty);
	if (nRetVal != XN_STATUS_OK)
		return FALSE;

	if (pProperty->GetType() != XN_PROPERTY_TYPE_INTEGER)
		return FALSE;

	XnIntProperty* pIntProperty = (XnIntProperty*)pProperty;

	XnUInt64 nValue;
	nRetVal = pIntProperty->GetValue(&nValue);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_DDK, "Failed getting the value of the IsStream property: %s", xnGetStatusString(nRetVal));
		return FALSE;
	}

	return (XnBool)nValue;
}

XnStatus XnDeviceBase::FindStream(const XnChar* StreamName, XnDeviceStream** ppStream)
{
	// find the module
	XnDeviceModuleHolder* pStreamHolder = NULL;
	XnStatus nRetVal = FindStream(StreamName, &pStreamHolder);
	XN_IS_STATUS_OK(nRetVal);

	*ppStream = (XnDeviceStream*)pStreamHolder->GetModule();

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::FindStream(const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder)
{
	// find the module
	XnDeviceModuleHolder* pModuleHolder = NULL;
	XnStatus nRetVal = FindModule(StreamName, &pModuleHolder);
	XN_IS_STATUS_OK(nRetVal);

	// check if this is a stream
	if (!IsStream(pModuleHolder->GetModule()))
		return XN_STATUS_MODULE_IS_NOT_STREAM;

	*ppStreamHolder = pModuleHolder;

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::AddSupportedStream(const XnChar* StreamType)
{
	// make sure stream doesn't exist yet
	XnStringsHash::Iterator it = m_SupportedStreams.end();
	if (XN_STATUS_OK == m_SupportedStreams.Find(StreamType, it))
	{
		xnLogError(XN_MASK_DEVICE, "A stream with the name %s already exists!", StreamType);
		return XN_STATUS_STREAM_ALREADY_EXISTS;
	}

	// add it to the list
	XnStatus nRetVal = m_SupportedStreams.Set(StreamType, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::GetStreamRequiredDataSize(const XnChar* StreamName, XnUInt32* pnRequiredSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// find stream
	XnDeviceStream* pStream;
	nRetVal = FindStream(StreamName, &pStream);
	XN_IS_STATUS_OK(nRetVal);

	*pnRequiredSize = pStream->GetRequiredDataSize();

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::CreateStreams(const XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	for (XnPropertySetData::ConstIterator it = pSet->pData->begin(); it != pSet->pData->end(); ++it)
	{
		// check if this module is a stream
		XnActualPropertiesHash* pModule = it.Value();

		XnActualPropertiesHash::ConstIterator itProp = pModule->end();
		if (XN_STATUS_OK == pModule->Find(XN_STREAM_PROPERTY_TYPE, itProp))
		{
			// create a copy of the properties
			XnActualPropertiesHash streamProps(it.Key());
			nRetVal = streamProps.CopyFrom(*pModule);
			XN_IS_STATUS_OK(nRetVal);

			// remove the type property
			nRetVal = streamProps.Remove(XN_STREAM_PROPERTY_TYPE);
			XN_IS_STATUS_OK(nRetVal);

			// and create the stream
			XnActualStringProperty* pActualProp = (XnActualStringProperty*)itProp.Value();
			nRetVal = CreateStreamImpl(pActualProp->GetValue(), it.Key(), &streamProps);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::ValidateOnlyModule(const XnPropertySet* pSet, const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnPropertySetData::ConstIterator it = pSet->pData->begin();
	if (it == pSet->pData->end())
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DDK, "Property set did not contain any stream!");
	}

	if (strcmp(it.Key(), StreamName) != 0)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DDK, "Property set module name does not match stream name!");
	}

	if (++it != pSet->pData->end())
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DDK, "Property set contains more than one module!");
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::CreateStream(const XnChar* StreamType, const XnChar* StreamName /* = NULL */, const XnPropertySet* pInitialValues /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check for name
	if (StreamName == NULL)
		StreamName = StreamType;

	XnActualPropertiesHash* pInitialValuesHash = NULL;

	if (pInitialValues != NULL)
	{
		// validate property set
		nRetVal = ValidateOnlyModule(pInitialValues, StreamName);
		XN_IS_STATUS_OK(nRetVal);

		pInitialValuesHash = pInitialValues->pData->begin().Value();
	}

	nRetVal = CreateStreamImpl(StreamType, StreamName, pInitialValuesHash);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::CreateStreamImpl(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogInfo(XN_MASK_DDK, "Creating stream '%s' of type '%s'...", strName, strType);

	XnDeviceModule* pModule;
	if (FindModule(strName, &pModule) == XN_STATUS_OK)
	{
		// already exists. check sharing mode (when shared, we allow "creating" the same stream)
		if (GetSharingMode() != XN_DEVICE_SHARED ||
			!IsStream(pModule) ||
			strcmp(strType, ((XnDeviceStream*)pModule)->GetType()) != 0)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_STREAM_ALREADY_EXISTS, XN_MASK_DDK, "A stream with this name already exists!");
		}

		// OK, we'll allow this. Just set new configuration
		if (pInitialSet != NULL)
		{
			nRetVal = pModule->BatchConfig(*pInitialSet);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	else
	{
		// create stream
		XnDeviceModuleHolder* pNewStreamHolder = NULL;

		nRetVal = CreateStreamModule(strType, strName, &pNewStreamHolder);
		XN_IS_STATUS_OK(nRetVal);

		XnDeviceStream* pNewStream = (XnDeviceStream*)(pNewStreamHolder->GetModule());
		if (pNewStream == NULL)
		{
			DestroyStreamModule(pNewStreamHolder);
			XN_LOG_ERROR_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Internal Error: Invalid new stream!");
		}

		// initialize the stream
		xnLogVerbose(XN_MASK_DDK, "Initializing stream '%s'...", strName);

		nRetVal = pNewStreamHolder->Init(pInitialSet);
		if (nRetVal != XN_STATUS_OK)
		{
			DestroyStreamModule(pNewStreamHolder);
			return (nRetVal);
		}

		// set it's mirror value (if not requested otherwise)
		XnBool bSetMirror = TRUE;

		if (pInitialSet != NULL)
		{
			XnActualPropertiesHash::ConstIterator it = pInitialSet->end();
			if (XN_STATUS_OK == pInitialSet->Find(XN_MODULE_PROPERTY_MIRROR, it))
			{
				bSetMirror = FALSE;
			}
		}

		if (bSetMirror)
		{
			nRetVal = pNewStream->SetMirror((XnBool)m_DeviceMirror.GetValue());
			if (nRetVal != XN_STATUS_OK)
			{
				DestroyStreamModule(pNewStreamHolder);
				return (nRetVal);
			}
		}

		// add it to the list of existing modules
		nRetVal = AddModule(pNewStreamHolder);
		if (nRetVal != XN_STATUS_OK)
		{
			DestroyStreamModule(pNewStreamHolder);
			return (nRetVal);
		}

		xnLogInfo(XN_MASK_DDK, "Stream '%s' was initialized.", strName);

		nRetVal = StreamAdded(pNewStream);
		XN_IS_STATUS_OK(nRetVal);

		xnLogInfo(XN_MASK_DDK, "'%s' stream was created.", strName);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::StreamAdded(XnDeviceStream* pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// register to the NewData event
	if (m_ReadWriteMode.GetValue() == XN_DEVICE_MODE_READ)
	{
		pStream->SetNewDataCallback(NewStreamDataCallback, this);
	}

	// raise the change event
	m_OnStreamsChangeEvent.Raise(GetDeviceHandle(), pStream->GetName(), XN_DEVICE_STREAM_ADDED);
	
	return (XN_STATUS_OK);
}

void XnDeviceBase::FreeModuleRegisteredProperties(const XnChar* strModule)
{
	// free memory of registered properties to this stream
	XnList::Iterator it = m_PropertyCallbacks.begin();
	while (it != m_PropertyCallbacks.end())
	{
		XnList::Iterator cur = it;
		it++;

		XnPropertyCallback* pRealCallback = (XnPropertyCallback*)*cur;
		if (strcmp(pRealCallback->strModule, strModule) == 0)
		{
			m_PropertyCallbacks.Remove(cur);
			XN_DELETE(pRealCallback);
		}
	}
}

XnStatus XnDeviceBase::DestroyStream(const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogInfo(XN_MASK_DDK, "Destroying stream '%s'...", StreamName);

	// keep the stream name (we now delete the module, so the name will be lost)
	XnChar strStreamName[XN_DEVICE_MAX_STRING_LENGTH];
	strncpy(strStreamName, StreamName, XN_DEVICE_MAX_STRING_LENGTH);

	// Find the stream
	XnDeviceModuleHolder* pStreamHolder;
	nRetVal = FindStream(strStreamName, &pStreamHolder);
	XN_IS_STATUS_OK(nRetVal);

	// remove it from map
	nRetVal = RemoveModule(strStreamName);
	XN_IS_STATUS_OK(nRetVal);

	// and free it's memory
	DestroyStreamModule(pStreamHolder);

	// free memory of registered properties to this stream
	FreeModuleRegisteredProperties(StreamName);

	// raise event
	m_OnStreamsChangeEvent.Raise(GetDeviceHandle(), strStreamName, XN_DEVICE_STREAM_DELETED);

	xnLogVerbose(XN_MASK_DDK, "'%s' stream destroyed.", strStreamName);

	return XN_STATUS_OK;
}

XnStatus XnDeviceBase::GetModulesList(XnDeviceModuleHolder** apModules, XnUInt32* pnCount)
{
	XnUInt32 nCount = 0;

	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		apModules[nCount] = (XnDeviceModuleHolder*)it.Value();
		nCount++;
	}

	*pnCount = nCount;

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::GetModulesList(XnDeviceModuleHolderList& list)
{
	XnStatus nRetVal = XN_STATUS_OK;

	list.Clear();

	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		list.AddLast((XnDeviceModuleHolder*)it.Value());
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::GetStreamsList(XnDeviceModuleHolderList& list)
{
	XnStatus nRetVal = XN_STATUS_OK;

	list.Clear();

	for (XnStringsHash::Iterator it = m_Modules.begin(); it != m_Modules.end(); ++it)
	{
		XnDeviceModuleHolder* pModuleHolder = (XnDeviceModuleHolder*)it.Value();
		if (IsStream(pModuleHolder->GetModule()))
		{
			list.AddLast(pModuleHolder);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::RaiseNewStreamDataEvent(const XnChar* StreamName)
{
	m_OnNewStreamDataEvent.Raise(GetDeviceHandle(), StreamName);

	return XN_STATUS_OK;
}

void XnDeviceBase::OnNewStreamData(XnDeviceStream* pStream, XnUInt64 nTimestamp, XnUInt32 nFrameID)
{
	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpWriteString(m_StreamsDataDump, "%llu,%s,%llu,%u\n", nNow, pStream->GetName(), nTimestamp, nFrameID);

	if (strcmp(m_PrimaryStream.GetValue(), XN_PRIMARY_STREAM_ANY) == 0 ||
		strcmp(m_PrimaryStream.GetValue(), XN_PRIMARY_STREAM_NONE) == 0)
	{
		// any stream makes us advance
		m_nLastTimestamp = XN_MAX(m_nLastTimestamp, nTimestamp);
		m_nLastFrameID = XN_MAX(m_nLastFrameID, nFrameID);
	}
	else if (strcmp(m_PrimaryStream.GetValue(), pStream->GetName()) == 0) // this stream is the primary
	{
		m_nLastTimestamp = nTimestamp;
		m_nLastFrameID = nFrameID;
	}

	XnStatus nRetVal = xnOSSetEvent(m_hNewDataEvent);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_DDK, "Failed setting the new data event: %s", xnGetStatusString(nRetVal));
	}

	RaiseNewStreamDataEvent(pStream->GetName());
}

void XnDeviceBase::ResetLastTimestampAndFrame()
{
	m_nLastFrameID = 0;
	m_nLastTimestamp = 0;
	m_nLastReadFrameID = 0;
	m_nLastReadTimestamp = 0;
}

void XnDeviceBase::NewStreamDataCallback(XnDeviceStream* pSender, XnUInt64 nTimestamp, XnUInt32 nFrameID, void* pCookie)
{
	XnDeviceBase* pThis = (XnDeviceBase*)pCookie;
	pThis->OnNewStreamData(pSender, nTimestamp, nFrameID);
}

XnBool XnDeviceBase::HasPrimaryStreamAdvanced(XnStreamDataSet* pOutputSet)
{
	if (strcmp(m_PrimaryStream.GetValue(), XN_PRIMARY_STREAM_NONE) == 0)
	{
		// special case of None: condition is always true
		return TRUE;
	}

	const XnChar* astrNames[XN_DEVICE_BASE_MAX_STREAMS_COUNT];
	XnUInt32 nArraySize = XN_DEVICE_BASE_MAX_STREAMS_COUNT;

	// take a list of streams to check for new data
	if (strcmp(m_PrimaryStream.GetValue(), XN_PRIMARY_STREAM_ANY) != 0)
	{
		// we have a specific stream. Add it to the list
		astrNames[0] = m_PrimaryStream.GetValue();
		nArraySize = 1;
	}
	else
	{
		// special case of ANY. we need to check every one of requested streams
		XnStreamData* apStreamOutputs[XN_DEVICE_BASE_MAX_STREAMS_COUNT];
		if (XN_STATUS_OK != XnStreamDataSetCopyToArray(pOutputSet, apStreamOutputs, &nArraySize))
		{
			return FALSE;
		}

		for (XnUInt32 i = 0; i < nArraySize; ++i)
		{
			astrNames[i] = apStreamOutputs[i]->StreamName;
		}
	}

	// now check if we have new data
	for (XnUInt32 i = 0; i < nArraySize; ++i)
	{
		XnDeviceStream* pStream = NULL;
		if (XN_STATUS_OK == FindStream(astrNames[i], &pStream))
		{
			if (pStream->IsNewDataAvailable())
				return TRUE;
		}
	}

	return FALSE;
}

XnBool XnDeviceBase::HasStreamAdvanced(XnDeviceStream* pStream)
{
	return pStream->IsNewDataAvailable();
}

XnBool XnDeviceBase::HasPrimaryStreamAdvancedCallback(void* pCookie)
{
	XnWaitForPrimaryData* pWaitData = (XnWaitForPrimaryData*)pCookie;
	return pWaitData->pThis->HasPrimaryStreamAdvanced(pWaitData->pSet);
}

XnBool XnDeviceBase::HasStreamAdvancedCallback(void* pCookie)
{
	XnWaitForStreamData* pWaitData = (XnWaitForStreamData*)pCookie;
	return pWaitData->pThis->HasStreamAdvanced(pWaitData->pStream);
}

XnStatus XnDeviceBase::WaitForStream(XN_EVENT_HANDLE hNewDataEvent, XnDeviceStream* pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnWaitForStreamData WaitData;
	WaitData.pThis = this;
	WaitData.pStream = pStream;
	nRetVal = xnOSWaitForCondition(hNewDataEvent, XN_DEVICE_READ_FRAME_TIMEOUT, &XnDeviceBase::HasStreamAdvancedCallback, &WaitData);
	if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
	{
		xnLogError(XN_MASK_DDK, "Not responding - stream did not advance!");
		return (XN_STATUS_IO_DEVICE_NOT_RESPONDING);
	}
	else
	{
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceBase::WaitForPrimaryStream(XN_EVENT_HANDLE hNewDataEvent, XnStreamDataSet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnWaitForPrimaryData WaitData;
	WaitData.pThis = this;
	WaitData.pSet = pSet;
	nRetVal = xnOSWaitForCondition(hNewDataEvent, XN_DEVICE_READ_FRAME_TIMEOUT, &XnDeviceBase::HasPrimaryStreamAdvancedCallback, &WaitData);
	if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
	{
		xnLogError(XN_MASK_DDK, "Not responding - primary stream did not advance!");
		return (XN_STATUS_IO_DEVICE_NOT_RESPONDING);
	}
	else
	{
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnDeviceBase::PropertyValueChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnPropertyCallback* pUserCallback = (XnPropertyCallback*)pCookie;

	// TODO: consider catching exceptions (if user does some stupid things)
	pUserCallback->pHandler(pUserCallback->DeviceHandle, pUserCallback->strModule, pUserCallback->strProp, pUserCallback->pCookie);

	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnDeviceBase::SetPrimaryStreamCallback(XnActualStringProperty* pSender, const XnChar* strValue, void* pCookie)
{
	XnDeviceBase* pThis = (XnDeviceBase*)pCookie;
	return pThis->SetPrimaryStream(strValue);
}

XnStatus XN_CALLBACK_TYPE XnDeviceBase::SetMirrorCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDeviceBase* pThis = (XnDeviceBase*)pCookie;
	return pThis->SetMirror((XnBool)nValue);
}

XnStatus XN_CALLBACK_TYPE XnDeviceBase::SetHighresTimestampsCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDeviceBase* pThis = (XnDeviceBase*)pCookie;
	return pThis->SetHighresTimestamps((XnBool)nValue);
}

