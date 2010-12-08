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






#ifndef _XNV_AUDIO_BUFFER_H_
#define _XNV_AUDIO_BUFFER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVStream.h"
#include "XnVStreamData.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* This object holds an audio buffer.
* A XnVAudioBuffer instance should be initialized either by XnVStream or
* by another XnVImageMap instance using the Clone method.
*/

class XN_EE_CORE_API XnVAudioBuffer : public XnVStreamData
{
public:
	XnVAudioBuffer(XnVStream* pStream, XnBool bIsLive = FALSE, XnBool bWaitForDataUpdate = FALSE);
	XnVAudioBuffer();
	virtual ~XnVAudioBuffer();

	XnStatus Initialize(const XnVAudioBuffer& abOther);

	XnStatus Clone(XnVAudioBuffer& abOther) const;

	XN_AUDIO_TYPE* Data();
	const XN_AUDIO_TYPE* Data() const;
	XN_AUDIO_TYPE &operator[](XnInt32 nIndex);
	const XN_AUDIO_TYPE &operator[](XnInt32 nIndex) const;

	// properties
	XnUInt32 GetBufferAllocatedSize() { return m_nBufferAllocatedSize; }
	inline XnUInt16 GetNumberOfChannels() const { return m_nNumberOfChannels; }
	inline XnSampleRate GetSampleRate() const { return m_nSampleRate; }

	XN_3_6_API XnUInt32 GetBufferSize() const { return GetDataSize(); }
	XN_3_6_API void SetBufferSize(XnUInt32 nBufferSize) { m_nBufferSize = nBufferSize; }

	void Clear();

	XnStatus CopyFrom(const XnVAudioBuffer& ab, XnUInt64 nTimestamp = 0);

protected:
	XnStatus ReadPropertiesFromStream();

private:
	XN_AUDIO_TYPE* m_pData;
	XnUInt16 m_nNumberOfChannels;
	XnUInt32 m_nBufferAllocatedSize;
	XnUInt32 m_nBufferSize;
	XnSampleRate m_nSampleRate;
};

#endif //_XNV_AUDIO_BUFFER_H_

