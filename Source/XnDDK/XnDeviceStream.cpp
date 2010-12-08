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
#include "XnDeviceStream.h"
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDeviceStream::XnDeviceStream(const XnChar* csType, const XnChar* csName) :
	XnDeviceModule(csName),
	m_IsStream(XN_STREAM_PROPERTY_IS_STREAM, TRUE),
	m_Type(XN_STREAM_PROPERTY_TYPE, csType),
	m_IsOpen(XN_STREAM_PROPERTY_STATE, FALSE),
	m_RequiredSize(XN_STREAM_PROPERTY_REQUIRED_DATA_SIZE),
	m_OutputFormat(XN_STREAM_PROPERTY_OUTPUT_FORMAT),
	m_IsMirrored(XN_MODULE_PROPERTY_MIRROR),
	m_bNewDataAvailable(FALSE),
	m_nTimestamp(0),
	m_nFrameID(0),
	m_pNewDataCallback(NULL)
{
}

XnStatus XnDeviceStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init module
	nRetVal = XnDeviceModule::Init();
	XN_IS_STATUS_OK(nRetVal);

	// cs
	nRetVal = xnOSCreateCriticalSection(&m_hCriticalSection);
	XN_IS_STATUS_OK(nRetVal);

	m_IsOpen.UpdateSetCallback(SetIsOpenCallback, this);
	m_OutputFormat.UpdateSetCallback(SetOutputFormatCallback, this);
	m_IsMirrored.UpdateSetCallback(SetIsMirrorCallback, this);

	XN_VALIDATE_ADD_PROPERTIES(this, &m_IsStream, &m_Type, &m_IsOpen, &m_OutputFormat, &m_RequiredSize, &m_IsMirrored);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::Free()
{
	xnOSCloseCriticalSection(&m_hCriticalSection);
	XnDeviceModule::Free();

	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::Open()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_IsOpen.UnsafeUpdateValue(TRUE);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::Close()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_IsOpen.UnsafeUpdateValue(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::SetOutputFormat(XnOutputFormats nOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_OutputFormat.UnsafeUpdateValue(nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::SetMirror(XnBool bIsMirrored)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_IsMirrored.UnsafeUpdateValue(bIsMirrored);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void XnDeviceStream::NewDataAvailable(XnUInt64 nTimestamp, XnUInt32 nFrameID)
{
	m_bNewDataAvailable = TRUE;
	m_nTimestamp = nTimestamp;
	m_nFrameID = nFrameID;

	if (m_pNewDataCallback != NULL)
	{
		m_pNewDataCallback(this, nTimestamp, nFrameID, m_pNewDataCallbackCookie);
	}
}

void XnDeviceStream::ResetLastTimestampAndFrameID()
{
	m_nFrameID = 0;
	m_nTimestamp = 0;
}

void XnDeviceStream::SetNewDataCallback(NewDataCallbackPtr pFunc, void* pCookie)
{
	m_pNewDataCallback = pFunc;
	m_pNewDataCallbackCookie = pCookie;
}

XnStatus XnDeviceStream::CreateStreamData(XnStreamData** ppStreamData)
{
	return XnStreamDataCreate(ppStreamData, GetName(), GetRequiredDataSize());
}

XnStatus XnDeviceStream::Read(XnStreamData* pStreamOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check the size of the stream output object
	nRetVal = XnStreamDataCheckSize(pStreamOutput, GetRequiredDataSize());
	XN_IS_STATUS_OK(nRetVal);

	// first mark old data as old
	pStreamOutput->bIsNew = FALSE;

	// now check if we have some new data
	if (m_bNewDataAvailable)
	{
		// copy data
		nRetVal = ReadImpl(pStreamOutput);
		XN_IS_STATUS_OK(nRetVal);

		xnOSEnterCriticalSection(&m_hCriticalSection);
		XnBool bMirror = IsMirrored();
		xnOSLeaveCriticalSection(&m_hCriticalSection);

		// mirror it if needed
		if (bMirror)
		{
			nRetVal = Mirror(pStreamOutput);
			XN_IS_STATUS_OK(nRetVal);
		}

		// mark that data is new
		pStreamOutput->bIsNew = TRUE;

		// and that we don't have new info
		m_bNewDataAvailable = FALSE;
	}
	else
	{
		nRetVal = HandleNoNewData(pStreamOutput);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::HandleNoNewData(XnStreamData* pStreamOutput)
{
	return XN_STATUS_OK;
}

XnStatus XnDeviceStream::Write(XnStreamData* pStreamData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// make sure data is new
	if (pStreamData->bIsNew)
	{
		nRetVal = WriteImpl(pStreamData);
		XN_IS_STATUS_OK(nRetVal);

		NewDataAvailable(pStreamData->nTimestamp, pStreamData->nFrameID);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::RegisterRequiredSizeProperty(XnProperty* pProperty)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// make sure the property belongs to this module (because we never unregister from it)
	XN_ASSERT(strcmp(pProperty->GetModule(), GetName()) == 0);

	XnCallbackHandle hCallbackDummy;
	nRetVal = pProperty->OnChangeEvent().Register(UpdateRequiredSizeCallback, this, &hCallbackDummy);
	XN_IS_STATUS_OK(nRetVal);

	// recalculate it
	nRetVal = UpdateRequiredSize();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDeviceStream::UpdateRequiredSize()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt32 nRequiredSize;
	nRetVal = CalcRequiredSize(&nRequiredSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_RequiredSize.UnsafeUpdateValue(nRequiredSize);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XN_CALLBACK_TYPE XnDeviceStream::UpdateRequiredSizeCallback(const XnProperty* pSenser, void* pCookie)
{
	XnDeviceStream* pStream = (XnDeviceStream*)pCookie;
	return pStream->UpdateRequiredSize();
}

XnStatus XN_CALLBACK_TYPE XnDeviceStream::SetIsOpenCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDeviceStream* pStream = (XnDeviceStream*)pCookie;
	if (nValue == TRUE)
	{
		return pStream->Open();
	}
	else
	{
		return pStream->Close();
	}
}

XnStatus XN_CALLBACK_TYPE XnDeviceStream::SetOutputFormatCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDeviceStream* pStream = (XnDeviceStream*)pCookie;
	return pStream->SetOutputFormat((XnOutputFormats)nValue);
}

XnStatus XN_CALLBACK_TYPE XnDeviceStream::SetIsMirrorCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnDeviceStream* pStream = (XnDeviceStream*)pCookie;
	return pStream->SetMirror((XnBool)nValue);
}
