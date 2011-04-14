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
#include "XnSensorAudioGenerator.h"

//---------------------------------------------------------------------------
// XnSensorAudioGenerator class
//---------------------------------------------------------------------------
XnSensorAudioGenerator::XnSensorAudioGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName) :
	XnSensorGenerator(context, sensor, pSensor, strStreamName)
{
}

XnStatus XnSensorAudioGenerator::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnSensorGenerator::Init();
	XN_IS_STATUS_OK(nRetVal);

	// create supported modes list
	XnUInt8 aSupportedChannels[] = { 1, 2 };
	XnUInt32 nSupportedChannels = sizeof(aSupportedChannels)/sizeof(XnUInt8);

	XnUInt16 aSupportedBitsPerSample[] = { 16 };
	XnUInt32 nSupportedBitsPerSample = sizeof(aSupportedBitsPerSample)/sizeof(XnUInt16);

	XnUInt32 aSupportedSampleRates[] = 
	{
		XN_SAMPLE_RATE_8K,
		XN_SAMPLE_RATE_11K,
		XN_SAMPLE_RATE_12K,
		XN_SAMPLE_RATE_16K,
		XN_SAMPLE_RATE_22K,
		XN_SAMPLE_RATE_24K,
		XN_SAMPLE_RATE_32K,
		XN_SAMPLE_RATE_44K,
		XN_SAMPLE_RATE_48K,
	};
	XnUInt32 nSupportedSampleRates = sizeof(aSupportedSampleRates)/sizeof(XnUInt32);

	for (XnUInt iChannel = 0; iChannel < nSupportedChannels; ++iChannel)
	{
		for (XnUInt iBitsPerSample = 0; iBitsPerSample < nSupportedBitsPerSample; ++iBitsPerSample)
		{
			for (XnUInt iSampleRate = 0; iSampleRate < nSupportedSampleRates; ++iSampleRate)
			{
				XnWaveOutputMode Mode;
				Mode.nChannels = aSupportedChannels[iChannel];
				Mode.nBitsPerSample = aSupportedBitsPerSample[iBitsPerSample];
				Mode.nSampleRate = aSupportedSampleRates[iSampleRate];
				nRetVal = m_SupportedModes.AddLast(Mode);
				XN_IS_STATUS_OK(nRetVal);
			}
		}
	}

	return (XN_STATUS_OK);
}

XnBool XnSensorAudioGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	return FALSE;
}

XnUChar* XnSensorAudioGenerator::GetAudioBuffer()
{
	return (XnUChar*)m_pStreamData->pData;
}

XnUInt32 XnSensorAudioGenerator::GetSupportedWaveOutputModesCount()
{
	return m_SupportedModes.Size();
}

XnStatus XnSensorAudioGenerator::GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(aSupportedModes);

	if (nCount < m_SupportedModes.Size())
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	XnUInt32 i = 0;
	for (XnWaveOutputModeList::Iterator it = m_SupportedModes.begin(); it != m_SupportedModes.end(); ++it, ++i)
	{
		aSupportedModes[i] = *it;
	}

	nCount = m_SupportedModes.Size();
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioGenerator::SetWaveOutputMode(const XnWaveOutputMode& OutputMode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (OutputMode.nBitsPerSample != 16)
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	XN_PROPERTY_SET_CREATE_ON_STACK(props);
	XnPropertySetAddModule(&props, m_strModule);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_SAMPLE_RATE, OutputMode.nSampleRate);
	XnPropertySetAddIntProperty(&props, m_strModule, XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS, OutputMode.nChannels);

	nRetVal = m_pSensor->BatchConfig(&props);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioGenerator::GetWaveOutputMode(XnWaveOutputMode& OutputMode)
{
	XnUInt64 nValue;
	OutputMode.nBitsPerSample = 16;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_SAMPLE_RATE, &nValue);
	OutputMode.nSampleRate = (XnUInt32)nValue;
	m_pSensor->GetProperty(m_strModule, XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS, &nValue);
	OutputMode.nChannels = (XnUInt8)nValue;

	return (XN_STATUS_OK);
}

XnStatus XnSensorAudioGenerator::RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	const XnChar* aProps[] = 
	{
		XN_STREAM_PROPERTY_SAMPLE_RATE,
		XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS,
		NULL
	};

	return RegisterToProps(handler, pCookie, hCallback, aProps);
}

void XnSensorAudioGenerator::UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback)
{
	UnregisterFromProps(hCallback);
}

void XnSensorAudioGenerator::FilterProperties(XnActualPropertiesHash* pHash)
{
	XnSensorGenerator::FilterProperties(pHash);
	pHash->Remove(XN_STREAM_PROPERTY_SAMPLE_RATE);
	pHash->Remove(XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS);
}

//---------------------------------------------------------------------------
// XnExportedSensorAudioGenerator class
//---------------------------------------------------------------------------
XnExportedSensorAudioGenerator::XnExportedSensorAudioGenerator() :
	XnExportedSensorGenerator(XN_NODE_TYPE_AUDIO, XN_STREAM_TYPE_AUDIO, FALSE)
{}

XnSensorGenerator* XnExportedSensorAudioGenerator::CreateGenerator(xn::Context& context, xn::Device& sensor, XnDeviceBase* pSensor, const XnChar* strStreamName)
{
	return XN_NEW(XnSensorAudioGenerator, context, sensor, pSensor, strStreamName);
}
