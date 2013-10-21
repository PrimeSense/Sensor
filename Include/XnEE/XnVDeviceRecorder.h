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
#ifndef __XN_V_DEVICE_RECORDER_H__
#define __XN_V_DEVICE_RECORDER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDevice.h"
#include "XnVEventHandlers.h"
#include "XnVPropertySet.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnVPropertiesHash;

class XN_EE_CORE_API XnVDeviceRecorder : protected XnVModulePropertyChangedHandler, protected XnVStreamCollectionChangedHandler
{
public:
	XnVDeviceRecorder(XnVDevice* pSourceDevice, const XnChar* strFilePath);
	~XnVDeviceRecorder();

	XnStatus StartRecording(XnBool bAutoAddStreams = FALSE);
	XnStatus StopRecording();

	XnStatus AddStream(const XnChar* strName);
	XnStatus RemoveStream(const XnChar* strName);

	XnStatus WriteData();

	inline XnVDevice* GetWriteDevice() const { return m_pWriter; }

protected:
	void OnModulePropertyChanged(const XnChar* strModule, const XnChar* strProperty);
	void OnStreamAdded(const XnChar* strStream);
	void OnStreamRemoved(const XnChar* strStream);

private:
	XnStatus RegisterForAllProps(XnVPropertySet& State);
	XnStatus UnregisterFromAllProps(XnVPropertySet& State);

	XnVDevice* m_pSource;
	XnChar m_strFilePath[XN_DEVICE_MAX_STRING_LENGTH];
	XnVDevice* m_pWriter;
	XnVPropertiesHash* m_pPropsHash;
	XnBool m_bAutoAddStreams;
	XnList m_Streams;
	XnCallbackHandle m_hStreamsCallback;
};

#endif //__XN_V_DEVICE_RECORDER_H__
