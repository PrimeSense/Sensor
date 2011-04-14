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
	nRetVal = m_pSensor->RegisterToNewStreamData(&OnDeviceNewStreamData, pNewDataCallback, &(pNewDataCallback->m_hCallback));
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

void XN_CALLBACK_TYPE XnSensorGenerator::OnDeviceNewStreamData(XnDeviceHandle pDeviceHandle, const XnChar* StreamName, void* pCookie)
{
	NewDataCallback *pNewDataCBParams = (NewDataCallback*)pCookie;
	if (strcmp(pNewDataCBParams->m_pGenerator->m_strInstanceName, StreamName) == 0)
	{
		pNewDataCBParams->m_handler(pNewDataCBParams->m_pCookie);
	}
}

const void* XnSensorGenerator::GetData()
{
	return m_pStreamData->pData;
}
