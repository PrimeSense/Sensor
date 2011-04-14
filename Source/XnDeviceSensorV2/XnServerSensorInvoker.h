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
#ifndef __XN_SERVER_SENSOR_INVOKER_H__
#define __XN_SERVER_SENSOR_INVOKER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensor.h"
#include <XnGeneralBuffer.h>
#include <XnEvent.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XnServerSensorInvoker
{
private:
	XN_DECLARE_EVENT_3ARG(NewStreamDataEvent, INewStreamDataEvent, const XnChar*, strStreamName, XnUInt64, nTimestamp, XnUInt32, nFrameID);

public:
	XnServerSensorInvoker();
	~XnServerSensorInvoker();

	XnStatus Init(const XnChar* strDevicePath, const XnChar* strGlobalConfigFile, XnUInt32 nAdditionalProps, XnProperty** aAdditionalProps);
	void Free();

	const XnChar* GetDevicePath();

	XnStatus GetIntProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64* pnValue);
	XnStatus GetRealProperty(const XnChar* strModule, const XnChar* strProperty, XnDouble* pdValue);
	XnStatus GetStringProperty(const XnChar* strModule, const XnChar* strProperty, XnChar* strValue);
	XnStatus GetGeneralProperty(const XnChar* strModule, const XnChar* strProperty, XnGeneralBuffer& gbValue);

	XnStatus SetIntProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64 nValue);
	XnStatus SetRealProperty(const XnChar* strModule, const XnChar* strProperty, XnDouble dValue);
	XnStatus SetStringProperty(const XnChar* strModule, const XnChar* strProperty, const XnChar* strValue);
	XnStatus SetGeneralProperty(const XnChar* strModule, const XnChar* strProperty, const XnGeneralBuffer& gbValue);

	XnStatus GetAllProperties(XnPropertySet* pSet, XnBool bNoStreams = FALSE, const XnChar* strModule = NULL);

	XnStatus LoadConfigFromFile(const XnChar* strFileName, const XnChar* strSectionName);
	XnStatus BatchConfig(const XnPropertySet* pChangeSet);
	XnStatus ConfigureModuleFromGlobalFile(const XnChar* strModule);

	XnStatus CreateStreamData(const XnChar* strStreamName, XnStreamData** ppStreamData);

	XnStatus GetStream(const XnChar* strType, const XnPropertySet* pInitialValues);
	XnStatus ReleaseStream(const XnChar* strType);

	typedef INewStreamDataEvent::HandlerPtr NewStreamDataHandler;
	XnStatus OpenStream(const XnChar* strName, NewStreamDataHandler pNewDataHandler, void* pCookie, XnCallbackHandle* phCallback);
	XnStatus CloseStream(const XnChar* strName, XnCallbackHandle hCallback);

	XnStatus AddRefFrameBuffer(const XnChar* strStreamName, XnBuffer* pBuffer);
	XnStatus ReleaseFrameBuffer(const XnChar* strStreamName, XnBuffer* pBuffer);

	XnStatus ReadStream(XnStreamData* pStreamData, XnUInt32* pnOffset);

	XN_DECLARE_EVENT_1ARG(PropChangeEvent, IPropChangeEvent, const XnProperty*, pProp);
	IPropChangeEvent& PropChangedEvent() { return m_propChangedEvent; }

private:
	// Types
	typedef struct SensorInvokerStream 
	{
		XnChar strType[XN_DEVICE_MAX_STRING_LENGTH];
		XnUInt32 nRefCount;
		XnUInt32 nOpenRefCount;
		XnStreamData* pStreamData;
		XnBool bNewData;
		NewStreamDataEvent* pNewDataEvent;
	} SensorInvokerStream;

	XN_DECLARE_STRINGS_HASH(SensorInvokerStream, _XnServerStreamsHash);

	class XnServerStreamsHash;

	class XnLockedServerStreamsHash
	{
	public:
		inline XnLockedServerStreamsHash(const XnLockedServerStreamsHash& other) :
			m_hash(other.m_hash),
			m_locker(other.m_locker)
		{}

		inline XnLockedServerStreamsHash& operator=(const XnLockedServerStreamsHash& other)
		{
			this->m_hash = other.m_hash;
			this->m_locker = other.m_locker;
			return *this;
		}

		inline _XnServerStreamsHash::Iterator begin()
		{
			return m_hash.begin();
		}

		inline _XnServerStreamsHash::Iterator end()
		{
			return m_hash.end();
		}

		typedef _XnServerStreamsHash::Iterator Iterator;

	private:
		inline XnLockedServerStreamsHash(_XnServerStreamsHash& hash, XN_CRITICAL_SECTION_HANDLE hLock) :
			m_hash(hash),
			m_locker(hLock)
		{}

		friend class XnServerSensorInvoker::XnServerStreamsHash;

		_XnServerStreamsHash& m_hash;
		XnAutoCSLocker m_locker;
	};

	class XnServerStreamsHash
	{
	public:
		XnServerStreamsHash()
		{
			XnStatus nRetVal = xnOSCreateCriticalSection(&m_hLock);
			XN_ASSERT(nRetVal == XN_STATUS_OK);
		}

		~XnServerStreamsHash()
		{
			XnStatus nRetVal = xnOSCloseCriticalSection(&m_hLock);
			XN_ASSERT(nRetVal == XN_STATUS_OK);
		}

		inline XnStatus Set(const XnChar* strName, const SensorInvokerStream& stream)
		{
			XnAutoCSLocker locker(m_hLock);
			return m_hash.Set(strName, stream);
		}

		inline XnStatus Get(const XnChar* strName, SensorInvokerStream*& pStream)
		{
			XnAutoCSLocker locker(m_hLock);
			return m_hash.Get(strName, pStream);
		}

		inline XnStatus Remove(const XnChar* strName)
		{
			XnAutoCSLocker locker(m_hLock);
			return m_hash.Remove(strName);
		}

		inline XnLockedServerStreamsHash GetLockedHashForIterating()
		{
			return XnLockedServerStreamsHash(m_hash, m_hLock);
		}

	private:
		XN_CRITICAL_SECTION_HANDLE m_hLock;
		_XnServerStreamsHash m_hash;
	};

	// Functions
	XnStatus RegisterToProps(XnPropertySet* pProps);

	XnStatus OnPropertyChanged(const XnProperty* pProp);
	XnStatus OnStreamAdded(const XnChar* StreamName);
	XnStatus OnStreamRemoved(const XnChar* StreamName);
	XnStatus OnStreamCollectionChanged(const XnChar* StreamName, XnStreamsChangeEventType EventType);
	XnStatus OnNewStreamData(const XnChar* StreamName);
	XnStatus ReadStreams();

	static XnStatus XN_CALLBACK_TYPE PropertyChangedCallback(const XnProperty* pProp, void* pCookie);
	static void XN_CALLBACK_TYPE StreamCollectionChangedCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, XnStreamsChangeEventType EventType, void* pCookie);
	static void XN_CALLBACK_TYPE NewStreamDataCallback(XnDeviceHandle DeviceHandle, const XnChar* StreamName, void* pCookie);
	static XN_THREAD_PROC ReaderThread(XN_THREAD_PARAM pThreadParam);

	// Members
	XnSensor m_sensor;
	XN_CRITICAL_SECTION_HANDLE m_hSensorLock;
	XN_THREAD_HANDLE m_hReaderThread;
	XN_EVENT_HANDLE m_hNewDataEvent;
	volatile XnBool m_bShouldRun;
	XnStatus m_errorState;

	PropChangeEvent m_propChangedEvent;
	XnServerStreamsHash m_streams;
};

#endif // __XN_SERVER_SENSOR_INVOKER_H__