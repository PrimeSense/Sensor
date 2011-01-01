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





#ifndef __XN_DEVICE_STREAM_H__
#define __XN_DEVICE_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnDeviceModule.h>
#include <XnDDK/XnActualIntProperty.h>
#include <XnDDK/XnActualStringProperty.h>
#include <XnEvent.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** Represents a stream in a device. */
class XN_DDK_CPP_API XnDeviceStream : public XnDeviceModule
{
public:
	XnDeviceStream(const XnChar* csType, const XnChar* csName);
	~XnDeviceStream() { Free(); }

	XnStatus Init();
	XnStatus Free();

	virtual XnStatus CreateStreamData(XnStreamData** ppStreamData);

	/** Reads latest stream info into stream output. */
	virtual XnStatus Read(XnStreamData* pStreamData);

	/** Writes stream data into stream. */
	virtual XnStatus Write(XnStreamData* pStreamData);

	/** Checks if new data is available from stream. */
	inline XnBool IsNewDataAvailable() const { return m_bNewDataAvailable; }

	typedef void (*NewDataCallbackPtr)(XnDeviceStream* pSender, XnUInt64 nTimestamp, XnUInt32 nFrameID, void* pCookie);

	/** Sets a function callback to be called when new data is available. */
	void SetNewDataCallback(NewDataCallbackPtr pFunc, void* pCookie);

	/** Notifies new data is available in this stream. */
	virtual void NewDataAvailable(XnUInt64 nTimestamp, XnUInt32 nFrameID);

	void ResetLastTimestampAndFrameID();

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline const XnChar* GetType() const { return m_Type.GetValue(); }
	inline XnBool IsOpen() const { return (XnBool)m_IsOpen.GetValue(); }
	inline XnOutputFormats GetOutputFormat() const { return (XnOutputFormats)m_OutputFormat.GetValue(); }
	inline XnBool IsMirrored() const { return (XnBool)m_IsMirrored.GetValue(); }
	inline XnUInt32 GetRequiredDataSize() const { return (XnUInt32)m_RequiredSize.GetValue(); }
	inline XnUInt64 GetLastTimestamp() const { return m_nTimestamp; }
	inline XnUInt32 GetLastFrameID() const { return m_nFrameID; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus Open();
	virtual XnStatus Close();
	virtual XnStatus SetOutputFormat(XnOutputFormats nOutputFormat);
	virtual XnStatus SetMirror(XnBool bIsMirrored);

protected:
	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& IsOpenProperty() { return m_IsOpen; }
	inline XnActualIntProperty& OutputFormatProperty() { return m_OutputFormat; }
	inline XnActualIntProperty& RequiredSizeProperty() { return m_RequiredSize; }
	inline XnActualIntProperty& IsMirroredProperty() { return m_IsMirrored; }

	inline XN_CRITICAL_SECTION_HANDLE* GetLock() { return &m_hCriticalSection; }

protected:
	//---------------------------------------------------------------------------
	// Virtual Functions
	//---------------------------------------------------------------------------

	/** Actually reads data into the stream output object. */
	virtual XnStatus ReadImpl(XnStreamData* pStreamOutput) = 0;
	/** Actually writes data from the stream output object. */
	virtual XnStatus WriteImpl(XnStreamData* pStreamData) = 0;
	/** Performs mirror on the given stream output. */
	virtual XnStatus Mirror(XnStreamData* pStreamOutput) const = 0;
	/** Calculates the required size. */
	virtual XnStatus CalcRequiredSize(XnUInt32* pnRequiredSize) const = 0;
	/** Allows inheriting classes to do something when user called Read(), and there is no new data. */
	virtual XnStatus HandleNoNewData(XnStreamData* pStreamOutput);

	//---------------------------------------------------------------------------
	// Utility Functions
	//---------------------------------------------------------------------------
	XnStatus RegisterRequiredSizeProperty(XnProperty* pProperty);

private:
	XnStatus UpdateRequiredSize();

	static XnStatus XN_CALLBACK_TYPE UpdateRequiredSizeCallback(const XnProperty* pSenser, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetIsOpenCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetOutputFormatCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetIsMirrorCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);


	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnActualIntProperty m_IsStream;
	XnActualStringProperty m_Type;
	XnActualIntProperty m_IsOpen;
	XnActualIntProperty m_RequiredSize;
	XnActualIntProperty m_OutputFormat;
	XnActualIntProperty m_IsMirrored;

	/** TRUE if new data is available. */
	XnBool m_bNewDataAvailable;
	/** Current timestamp of this stream. */
	XnUInt64 m_nTimestamp;
	/** Current frame id of this stream. */
	XnUInt32 m_nFrameID;

	NewDataCallbackPtr m_pNewDataCallback;
	void* m_pNewDataCallbackCookie;

	XN_CRITICAL_SECTION_HANDLE m_hCriticalSection;
};

#endif //__XN_DEVICE_STREAM_H__