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

