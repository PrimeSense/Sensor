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
#ifndef __XN_V_STREAM_OUTPUT_H__
#define __XN_V_STREAM_OUTPUT_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVEventHandlers.h"
#include "XnVStream.h"
#include <XnListT.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_EE_CORE_API XnVStreamData : private XnVModulePropertyChangedHandler
{
public:
	XnVStreamData(XnVStream* pStream = NULL, XnBool bIsLive = FALSE, XnBool bWaitForDataUpdate = FALSE);
	virtual ~XnVStreamData();

	XnStatus Initialize(const XnVStreamData& soOther);

	inline XnBool IsValid() const { return m_bValid; }

	inline XnVStream* GetStream() { return m_pStream; }
	inline const XnVStream* GetStream() const { return m_pStream; }

	XnVStreamContainer* GetCreator();
	const XnVStreamContainer* GetCreator() const;

	inline XnBool IsLive() const { return m_bIsLive; }

	inline XnBool IsDataNew() const { return m_bIsNewData; }
	inline XnUInt64 GetTimestamp() const { return m_nTimestamp; }
	inline XnUInt32 GetFrameID() const { return m_nFrameID; }
	inline XnUInt32 GetDataSize() const { return m_nDataSize; }

	XnStatus SetTimestamp(XnUInt64 nTimestamp);
	XnStatus SetFrameID(XnUInt32 nFrameID);
	XnStatus SetDataSize(XnUInt32 nDataSize);
	XnStatus SetDataNew(XnBool bIsNew);

	inline void* Data() const { return m_pData; }

	XnStatus CopyFrom(const void* pData, XnUInt32 nDataSize, XnUInt64 nTimestamp = 0);
	XnStatus CopyFrom(const XnVStreamData& sd, XnUInt64 nTimestamp = 0);

protected:
	friend class XnVDeviceStream;
	XnStatus UnderlyingStreamUpdated();
	virtual XnStatus ReadPropertiesFromStream();
	XnStatus AddWatchedProperty(const XnChar* strProp);

	inline void SetValidity(XnBool bValid) { m_bValid = bValid; }

private:
	void OnModulePropertyChanged(const XnChar* strModule, const XnChar* strProperty);

	typedef struct XnWatchedProperty
	{
		const XnChar* strName;
		XnCallbackHandle hCallback;
	} XnWatchedProperty;

	typedef XnListT<XnWatchedProperty> XnWatchedPropertiesList;

	XnBool m_bIsLive;
	XnBool m_bWaitForDataUpdate;
	XnWatchedPropertiesList m_WatchedProperties;
	XnBool m_bChangeOccurred;

	XN_3_6_PUBLIC_MEMBER XnVStream* m_pStream;
	XN_3_6_PUBLIC_MEMBER XnBool m_bValid;

	XnBool m_bIsNewData;
	XnUInt64 m_nTimestamp;
	XnUInt32 m_nFrameID;
	void* m_pData;
	XnUInt32 m_nDataSize;
};

#endif //__XN_V_STREAM_OUTPUT_H__
