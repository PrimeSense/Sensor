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
