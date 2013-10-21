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
#include "XnSensorIRGenerator.h"
#include <XnOpenNI.h>

//---------------------------------------------------------------------------
// XnSensorIRGenerator class
//---------------------------------------------------------------------------
XnSensorIRGenerator::XnSensorIRGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) :
	XnSensorMapGenerator(context, sensor, pSensor, strStreamName),
	m_hMapModeCallback(NULL),
	m_hCroppingCallback(NULL),
	m_nBufferSize(0)
{}

XnStatus XnSensorIRGenerator::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnSensorMapGenerator::Init();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SetIntProperty(XN_STREAM_PROPERTY_OUTPUT_FORMAT, XN_OUTPUT_FORMAT_GRAYSCALE16);
	XN_IS_STATUS_OK(nRetVal);

	//Register to map output mode and cropping events
	nRetVal = RegisterToMapOutputModeChange(OnResChangedCallback, this, m_hMapModeCallback);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = RegisterToCroppingChange(OnResChangedCallback, this, m_hCroppingCallback);
	XN_IS_STATUS_OK(nRetVal);

	OnResChanged();
	
	return (XN_STATUS_OK);
}

XnBool XnSensorIRGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return XnSensorMapGenerator::IsCapabilitySupported(strCapabilityName);
}

XnIRPixel* XnSensorIRGenerator::GetIRMap()
{
	return (XnIRPixel*)m_pStreamData->pData;
}

XnUInt32 XnSensorIRGenerator::GetDataSize()
{
	return m_nBufferSize;
}

void XnSensorIRGenerator::OnResChanged()
{
	// we calculate the size because the IR stream actually gives out a bigger buffer, but
	// we want the buffer we return to be with the right size.
	XnMapOutputMode outputMode;
	GetMapOutputMode(outputMode);

	XnUInt32 nPixels = outputMode.nXRes * outputMode.nYRes;

	XnCropping cropping;
	GetCropping(cropping);

	if (cropping.bEnabled)
	{
		nPixels = cropping.nXSize * cropping.nYSize;
	}

	m_nBufferSize = nPixels * sizeof(XnIRPixel);
}

void XN_CALLBACK_TYPE XnSensorIRGenerator::OnResChangedCallback(void* pCookie)
{
	XnSensorIRGenerator* pThis = (XnSensorIRGenerator*)pCookie;
	pThis->OnResChanged();
}

//---------------------------------------------------------------------------
// XnExportedSensorIRGenerator class
//---------------------------------------------------------------------------
XnExportedSensorIRGenerator::XnExportedSensorIRGenerator() :
	XnExportedSensorGenerator(XN_NODE_TYPE_IR, XN_STREAM_TYPE_IR, FALSE)
{}

XnSensorGenerator* XnExportedSensorIRGenerator::CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName)
{
	return XN_NEW(XnSensorIRGenerator, context, sensor, pSensor, strStreamName);
}
