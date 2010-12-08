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





#ifndef __XN_AUDIO_PROCESSOR_H__
#define __XN_AUDIO_PROCESSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnWholePacketProcessor.h"
#include "XnSensorAudioStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

class XnAudioProcessor : public XnWholePacketProcessor
{
public:
	XnAudioProcessor(XnSensorAudioStream* pStream, XnSensorStreamHelper* pHelper, XnUInt32 nInputPacketSize);
	~XnAudioProcessor();

	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void ProcessWholePacket(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData);

	inline XnSensorAudioStream* GetStream()
	{
		return m_pStream;
	}

	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
private:
	void CalcDeleteChannel();
	static XnStatus XN_CALLBACK_TYPE DeleteChannelChangedCallback(const XnProperty* pSender, void* pCookie);

	/** Used to dump Audio In data. */
	XnDump m_AudioInDump;
	XnBool m_bDeleteChannel;
	XnSensorAudioStream* m_pStream;
	XnSensorStreamHelper* m_pHelper;

	XnCallbackHandle m_hNumChannelsCallback;
};

#endif //__XN_AUDIO_PROCESSOR_H__
