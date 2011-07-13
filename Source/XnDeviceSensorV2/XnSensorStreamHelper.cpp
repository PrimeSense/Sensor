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
#include "XnSensorStreamHelper.h"
#include "XnStreamProcessor.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnSensorStreamHelperCookie
{
public:
	XnSensorStreamHelperCookie(XnActualIntProperty* pStreamProp, XnActualIntProperty* pFirmwareProp, XnBool bAllowWhileOpen, XnSensorStreamHelper::ConvertCallback pStreamToFirmwareFunc) :
		pStreamProp(pStreamProp), pFirmwareProp(pFirmwareProp), bAllowWhileOpen(bAllowWhileOpen), pStreamToFirmwareFunc(pStreamToFirmwareFunc), bProcessorProp(FALSE)
	{}

	XnActualIntProperty* pStreamProp;
	XnActualIntProperty* pFirmwareProp;
	XnBool bAllowWhileOpen;
	XnSensorStreamHelper::ConvertCallback pStreamToFirmwareFunc;
	XnBool bProcessorProp;

	struct
	{
		XnBool bShouldOpen;
		XnBool bChooseProcessor;
	} CurrentTransaction;
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnSensorStreamHelper::XnSensorStreamHelper(XnSensorObjects* pObjects) : 
	m_pSensorStream(NULL),
	m_pStream(NULL),
	m_pObjects(pObjects)
{
}

XnSensorStreamHelper::~XnSensorStreamHelper()
{
	Free();
}

XnStatus XnSensorStreamHelper::Init(IXnSensorStream* pSensorStream, XnDeviceStream* pStream)
{
	XnStatus nRetVal = XN_STATUS_OK;

	m_pSensorStream = pSensorStream;
	m_pStream = pStream;
	
	nRetVal = m_pSensorStream->MapPropertiesToFirmware();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::Free()
{
	if (m_pStream != NULL)
	{
		GetFirmware()->GetStreams()->ReleaseStream(m_pStream->GetType(), m_pStream);
	}

	for (XnHash::Iterator it = m_FirmwareProperties.begin(); it != m_FirmwareProperties.end(); ++it)
	{
		XnSensorStreamHelperCookie* pCookie = (XnSensorStreamHelperCookie*)it.Value();
		XN_DELETE(pCookie);
	}

	m_FirmwareProperties.Clear();

	return XN_STATUS_OK;
}

XnStatus XnSensorStreamHelper::Configure()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnResolutions nRes;
	XnUInt32 nFPS;
	m_pSensorStream->GetFirmwareStreamConfig(&nRes, &nFPS);

	// claim the stream
	nRetVal = GetFirmware()->GetStreams()->ClaimStream(m_pStream->GetType(), nRes, nFPS, m_pStream);
	XN_IS_STATUS_OK(nRetVal);

	// configure the stream
	nRetVal = m_pSensorStream->ConfigureStreamImpl();
	if (nRetVal != XN_STATUS_OK)
	{
		GetFirmware()->GetStreams()->ReleaseStream(m_pStream->GetType(), m_pStream);
		return (nRetVal);
	}

	// create data processor
	XnDataProcessor* pProcessor;
	nRetVal = m_pSensorStream->CreateDataProcessor(&pProcessor);
	if (nRetVal != XN_STATUS_OK)
	{
		GetFirmware()->GetStreams()->ReleaseStream(m_pStream->GetType(), m_pStream);
		return (nRetVal);
	}

	// and register it
	nRetVal = GetFirmware()->GetStreams()->ReplaceStreamProcessor(m_pStream->GetType(), m_pStream, pProcessor);
	if (nRetVal != XN_STATUS_OK)
	{
		GetFirmware()->GetStreams()->ReleaseStream(m_pStream->GetType(), m_pStream);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::FinalOpen()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_pSensorStream->OpenStreamImpl();
	if (nRetVal != XN_STATUS_OK)
	{
		GetFirmware()->GetStreams()->ReleaseStream(m_pStream->GetType(), m_pStream);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::Open()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// configure the stream
	nRetVal = Configure();
	XN_IS_STATUS_OK(nRetVal);

	// and now turn it on
	nRetVal = FinalOpen();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::Close()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (GetFirmware()->GetStreams()->IsClaimed(m_pStream->GetType(), m_pStream))
	{
		nRetVal = m_pSensorStream->CloseStreamImpl();
		XN_IS_STATUS_OK(nRetVal);

		GetFirmware()->GetStreams()->ReleaseStream(m_pStream->GetType(), m_pStream);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::RegisterDataProcessorProperty(XnActualIntProperty& Property)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// mark it so
	XnValue val;
	nRetVal = m_FirmwareProperties.Get(&Property, val);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorStreamHelperCookie* pCookie = (XnSensorStreamHelperCookie*)val;
	pCookie->bProcessorProp = TRUE;

	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::MapFirmwareProperty(XnActualIntProperty& Property, XnActualIntProperty& FirmwareProperty, XnBool bAllowChangeWhileOpen, XnSensorStreamHelper::ConvertCallback pStreamToFirmwareFunc /* = 0 */)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// init data
	XnSensorStreamHelperCookie* pCookie;
	XN_VALIDATE_NEW(pCookie, XnSensorStreamHelperCookie, &Property, &FirmwareProperty, bAllowChangeWhileOpen, pStreamToFirmwareFunc);

	// add it to the list
	nRetVal = m_FirmwareProperties.Set(&Property, pCookie);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pCookie);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::ConfigureFirmware(XnActualIntProperty& Property)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnHash::Iterator it = m_FirmwareProperties.end();
	nRetVal = m_FirmwareProperties.Find(&Property, it);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorStreamHelperCookie* pPropData = (XnSensorStreamHelperCookie*)it.Value();
	
	XnUInt64 nFirmwareValue = Property.GetValue();

	if (pPropData->pStreamToFirmwareFunc != NULL)
	{
		nRetVal = pPropData->pStreamToFirmwareFunc(Property.GetValue(), &nFirmwareValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = pPropData->pFirmwareProp->SetValue(nFirmwareValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::BeforeSettingFirmwareParam(XnActualIntProperty& Property, XnUInt16 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnHash::Iterator it = m_FirmwareProperties.end();
	nRetVal = m_FirmwareProperties.Find(&Property, it);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorStreamHelperCookie* pPropData = (XnSensorStreamHelperCookie*)it.Value();

	pPropData->CurrentTransaction.bShouldOpen = FALSE;
	pPropData->CurrentTransaction.bChooseProcessor = FALSE;

	// if stream is closed, we can just update the prop.
	if (m_pStream->IsOpen())
	{
		// check if we need to close the stream first
		if (pPropData->bAllowWhileOpen)
		{
			// before actual changing it, check if this is a processor property
			if (pPropData->bProcessorProp)
			{
				// lock processor
				nRetVal = GetFirmware()->GetStreams()->LockStreamProcessor(m_pStream->GetType(), m_pStream);
				XN_IS_STATUS_OK(nRetVal);
				pPropData->CurrentTransaction.bChooseProcessor = TRUE;
			}

			// OK. change the value
			XnUInt64 nFirmwareValue = nValue;

			if (pPropData->pStreamToFirmwareFunc != NULL)
			{
				nRetVal = pPropData->pStreamToFirmwareFunc(nValue, &nFirmwareValue);
				XN_IS_STATUS_OK(nRetVal);
			}

			// set the param in firmware
			nRetVal = pPropData->pFirmwareProp->SetValue(nFirmwareValue);
			XN_IS_STATUS_OK(nRetVal);

			// no need to do anything after property will be set
			pPropData->CurrentTransaction.bShouldOpen = FALSE;
		}
		else
		{
			// we can't change the firmware param. We should first close the stream
			nRetVal = m_pStream->Close();
			XN_IS_STATUS_OK(nRetVal);

			// after property will be set, we need to reopen the stream
			pPropData->CurrentTransaction.bShouldOpen = TRUE;
		}
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::AfterSettingFirmwareParam(XnActualIntProperty& Property)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnHash::Iterator it = m_FirmwareProperties.end();
	nRetVal = m_FirmwareProperties.Find(&Property, it);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorStreamHelperCookie* pPropData = (XnSensorStreamHelperCookie*)it.Value();
	
	if (pPropData->CurrentTransaction.bShouldOpen)
	{
		nRetVal = m_pStream->Open();
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pPropData->CurrentTransaction.bChooseProcessor)
	{
		XnDataProcessor* pProcessor = NULL;
		nRetVal = m_pSensorStream->CreateDataProcessor(&pProcessor);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetFirmware()->GetStreams()->ReplaceStreamProcessor(m_pStream->GetType(), m_pStream, pProcessor);
		XN_IS_STATUS_OK(nRetVal);

		// and unlock
		nRetVal = GetFirmware()->GetStreams()->UnlockStreamProcessor(m_pStream->GetType(), m_pStream);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::SimpleSetFirmwareParam(XnActualIntProperty& Property, XnUInt16 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = BeforeSettingFirmwareParam(Property, nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = Property.UnsafeUpdateValue(nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = AfterSettingFirmwareParam(Property);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::BeforeSettingDataProcessorProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_pStream->IsOpen())
	{
		nRetVal = GetFirmware()->GetStreams()->LockStreamProcessor(m_pStream->GetType(), m_pStream);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::AfterSettingDataProcessorProperty()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_pStream->IsOpen())
	{
		XnDataProcessor* pProcessor = NULL;
		nRetVal = m_pSensorStream->CreateDataProcessor(&pProcessor);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = GetFirmware()->GetStreams()->ReplaceStreamProcessor(m_pStream->GetType(), m_pStream, pProcessor);
		XN_IS_STATUS_OK(nRetVal);

		// and unlock
		nRetVal = GetFirmware()->GetStreams()->UnlockStreamProcessor(m_pStream->GetType(), m_pStream);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::UpdateFromFirmware(XnActualIntProperty& Property)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnHash::Iterator it = m_FirmwareProperties.end();
	nRetVal = m_FirmwareProperties.Find(&Property, it);
	XN_IS_STATUS_OK(nRetVal);

	XnSensorStreamHelperCookie* pPropData = (XnSensorStreamHelperCookie*)it.Value();
	nRetVal = pPropData->pStreamProp->UnsafeUpdateValue(pPropData->pFirmwareProp->GetValue());
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorStreamHelper::BatchConfig(const XnActualPropertiesHash& props)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnBool bShouldClose = FALSE;

	if (m_pStream->IsOpen())
	{
		// check if one of the properties requires to close the stream
		for (XnHash::Iterator it = m_FirmwareProperties.begin(); it != m_FirmwareProperties.end(); ++it)
		{
			XnSensorStreamHelperCookie* pPropData = (XnSensorStreamHelperCookie*)it.Value();
			if (!pPropData->bAllowWhileOpen)
			{
				XnProperty* pProp;
				if (XN_STATUS_OK == props.Get(pPropData->pStreamProp->GetName(), pProp))
				{
					bShouldClose = TRUE;
					break;
				}
			}
		}
	}

	if (bShouldClose)
	{
		xnLogVerbose(XN_MASK_DEVICE_SENSOR, "closing stream before batch config...");
		nRetVal = m_pStream->Close();
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = m_pStream->XnDeviceStream::BatchConfig(props);
	XN_IS_STATUS_OK(nRetVal);

	if (bShouldClose)
	{
		xnLogVerbose(XN_MASK_DEVICE_SENSOR, "re-opening stream after batch config...");
		nRetVal = m_pStream->Open();
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}
