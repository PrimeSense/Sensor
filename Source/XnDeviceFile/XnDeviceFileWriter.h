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





#ifndef _XN_DEVICE_FILE_WRITER_H_
#define _XN_DEVICE_FILE_WRITER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnStreamWriterDevice.h>
#include <XnIOFileStream.h>
#include "XnFileWriterStream.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnDeviceFileWriter : public XnStreamWriterDevice
{
public:
	XnDeviceFileWriter();
	~XnDeviceFileWriter();

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	virtual XnStatus Destroy();
	virtual XnStatus CreateStream(const XnChar* StreamType, const XnChar* StreamName = NULL, const XnPropertySet* pInitialValues = NULL);
	virtual XnStatus DestroyStream(const XnChar* StreamName);
	virtual XnStatus WriteStream(XnStreamData* pStreamOutput);
	virtual XnStatus Write(XnStreamDataSet* pStreamOutputSet);

protected:
	//---------------------------------------------------------------------------
	// Helper Methods
	//---------------------------------------------------------------------------
	inline XnIOFileStream* GetIOStream() { return (XnIOFileStream*)XnStreamWriterDevice::GetIOStream(); }
	XnStatus FindStream(const XnChar* strName, XnFileWriterStream** ppStream);

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus CreateIOStreamImpl(const XnChar* strConnectionString, XnIOStream*& pStream);
	void DestroyIOStreamImpl(XnIOStream* pStream);
	XnStatus CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder);
	void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder);

private:
	XnStatus StartTimer();

	XnOSTimer m_Timer;
	XnBool m_bTimerStarted;
};

#endif //_XN_DEVICE_FILE_WRITER_H_

