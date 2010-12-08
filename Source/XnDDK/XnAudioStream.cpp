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
#include "XnAudioStream.h"
#include <XnOS.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_AUDIO_STREAM_BUFFER_SIZE_IN_SECONDS	1.5

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnAudioStream::XnAudioStream(const XnChar* csName, XnUInt32 nMaxNumberOfChannels) :
	XnStreamingStream(XN_STREAM_TYPE_AUDIO, csName),
	m_SampleRate(XN_STREAM_PROPERTY_SAMPLE_RATE, XN_SAMPLE_RATE_48K),
	m_NumberOfChannels(XN_STREAM_PROPERTY_NUMBER_OF_CHANNELS, 2),
	m_nMaxNumberOfChannels(nMaxNumberOfChannels)
{
}

XnStatus XnAudioStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnStreamingStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	m_SampleRate.UpdateSetCallback(SetSampleRateCallback, this);
	m_NumberOfChannels.UpdateSetCallback(SetNumberOfChannelsCallback, this);

	XN_VALIDATE_ADD_PROPERTIES(this, &m_SampleRate, &m_NumberOfChannels);

	// required size 
	nRetVal = RegisterRequiredSizeProperty(&m_SampleRate);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnAudioStream::SetSampleRate(XnSampleRate nSampleRate)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_SampleRate.UnsafeUpdateValue(nSampleRate);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnAudioStream::SetNumberOfChannels(XnUInt32 nNumberOfChannels)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = m_NumberOfChannels.UnsafeUpdateValue(nNumberOfChannels);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnAudioStream::CalcRequiredSize(XnUInt32* pnRequiredSize) const
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt32 nSampleSize = 2 * m_nMaxNumberOfChannels; // 16-bit per channel (2 bytes)
	XnUInt32 nSamples = (XnUInt32)(GetSampleRate() * XN_AUDIO_STREAM_BUFFER_SIZE_IN_SECONDS);

	*pnRequiredSize = nSamples * nSampleSize;
	
	return (XN_STATUS_OK);
}

XnStatus XnAudioStream::SetSampleRateCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnAudioStream* pStream = (XnAudioStream*)pCookie;
	return pStream->SetSampleRate((XnSampleRate)nValue);
}

XnStatus XnAudioStream::SetNumberOfChannelsCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnAudioStream* pStream = (XnAudioStream*)pCookie;
	return pStream->SetNumberOfChannels((XnUInt32)nValue);
}
