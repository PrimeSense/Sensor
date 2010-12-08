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





#ifndef __XN_AUDIO_STREAM_H__
#define __XN_AUDIO_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnStreamingStream.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/**
* Represents a default base implementation of an audio stream.
*/
class XN_DDK_CPP_API XnAudioStream : public XnStreamingStream
{
public:
	XnAudioStream(const XnChar* csName, XnUInt32 nMaxNumberOfChannels);
	~XnAudioStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline XnSampleRate GetSampleRate() const { return (XnSampleRate)m_SampleRate.GetValue(); }
	inline XnUInt32 GetNumberOfChannels() const { return (XnUInt32)m_NumberOfChannels.GetValue(); }

protected:
	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& SampleRateProperty() { return m_SampleRate; }
	inline XnActualIntProperty& NumberOfChannelsProperty() { return m_NumberOfChannels; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetSampleRate(XnSampleRate nSampleRate);
	virtual XnStatus SetNumberOfChannels(XnUInt32 nNumberOfChannels);

	XnStatus CalcRequiredSize(XnUInt32* pnRequiredSize) const;

private:

	static XnStatus XN_CALLBACK_TYPE SetSampleRateCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetNumberOfChannelsCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnActualIntProperty m_SampleRate;
	XnActualIntProperty m_NumberOfChannels;

	XnUInt32 m_nMaxNumberOfChannels;
};

#endif //__XN_AUDIO_STREAM_H__