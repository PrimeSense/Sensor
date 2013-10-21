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
#include "XnSensorGenerator.h"

//---------------------------------------------------------------------------
// XnSensorGenerator class
//---------------------------------------------------------------------------
XnSensorGenerator::XnSensorGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) : 
	XnSensorProductionNode(context, strStreamName, pSensor, strStreamName),
	m_pStreamData(NULL),
	m_device(sensor)
{}

XnSensorGenerator::~XnSensorGenerator()
{
	m_pSensor->DestroyStreamData(&m_pStreamData);
}

XnStatus XnSensorGenerator::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_pSensor->GetProperty(XN_MODULE_NAME_DEVICE, XN_MODULE_PROPERTY_VERSION, XN_PACK_GENERAL_BUFFER(m_Version));
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_pSensor->CreateStreamData(m_strModule, &m_pStreamData);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnBool XnSensorGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return (strcmp(strCapabilityName, XN_CAPABILITY_MIRROR) == 0 ||
		XnSensorProductionNode::IsCapabilitySupported(strCapabilityName));
}

XnStatus XnSensorGenerator::StartGenerating()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_pSensor->OpenStream(m_strModule);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnBool XnSensorGenerator::IsGenerating()
{
	XnUInt64 nValue = FALSE;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_STATE, &nValue);
	return (XnBool)nValue;
}

void XnSensorGenerator::StopGenerating()
{
	m_pSensor->CloseStream(m_strModule);
}

XnStatus XnSensorGenerator::RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_STATE,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorGenerator::UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

XnStatus XnSensorGenerator::RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;
	NewDataCallback* pNewDataCallback;
	XN_VALIDATE_NEW(pNewDataCallback, NewDataCallback, this, handler, pCookie);
	hCallback = pNewDataCallback;
	nRetVal = m_pSensor->RegisterToNewStreamData(&OnDeviceNewStreamData, pNewDataCallback, pNewDataCallback->m_hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNewDataCallback);
		return nRetVal;
	}

	return XN_STATUS_OK;
}

void XnSensorGenerator::UnregisterFromNewDataAvailable(XnCallbackHandle hCallback)
{
	NewDataCallback *pNewDataCallback = (NewDataCallback*)hCallback;
	m_pSensor->UnregisterFromNewStreamData(pNewDataCallback->m_hCallback);
	XN_DELETE(pNewDataCallback);
}

XnBool XnSensorGenerator::IsNewDataAvailable(XnUInt64& nTimestamp)
{
	XnBool bResult = FALSE;
	m_pSensor->IsNewDataAvailable(m_strModule, &bResult, &nTimestamp);
	return bResult;
}

XnStatus XnSensorGenerator::UpdateData()
{
	return m_pSensor->ReadStream(m_pStreamData);
}

XnUInt32 XnSensorGenerator::GetDataSize()
{
	return m_pStreamData->nDataSize;
}

XnUInt64 XnSensorGenerator::GetTimestamp()
{
	return m_pStreamData->nTimestamp;
}

XnUInt32 XnSensorGenerator::GetFrameID()
{
	return m_pStreamData->nFrameID;
}

XnStatus XnSensorGenerator::SetMirror(XnBool bMirror)
{
	if (bMirror != IsMirrored())
	{
		return m_pSensor->SetProperty(m_strModule, XN_MODULE_PROPERTY_MIRROR, (XnUInt64)bMirror);
	}
	else
	{
		return (XN_STATUS_OK);
	}
}

XnBool XnSensorGenerator::IsMirrored()
{
	XnUInt64 nValue;
	m_pSensor->GetProperty(m_strModule, XN_MODULE_PROPERTY_MIRROR, &nValue);
	return (XnBool)nValue;
}

XnStatus XnSensorGenerator::RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_MODULE_PROPERTY_MIRROR,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorGenerator::UnregisterFromMirrorChange(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

void XnSensorGenerator::FilterProperties(XnActualPropertiesHash* pHash)
{
	XnSensorProductionNode::FilterProperties(pHash);
	pHash->Remove(XN_MODULE_PROPERTY_MIRROR);
	pHash->Remove(XN_STREAM_PROPERTY_STATE);
}

void XN_CALLBACK_TYPE XnSensorGenerator::OnDeviceNewStreamData(const XnNewStreamDataEventArgs& args, void* pCookie)
{
	NewDataCallback *pNewDataCBParams = (NewDataCallback*)pCookie;
	if (strcmp(pNewDataCBParams->m_pGenerator->m_strInstanceName, args.strStreamName) == 0)
	{
		pNewDataCBParams->m_handler(pNewDataCBParams->m_pCookie);
	}
}

const void* XnSensorGenerator::GetData()
{
	return m_pStreamData->pData;
}
