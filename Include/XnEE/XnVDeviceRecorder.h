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
