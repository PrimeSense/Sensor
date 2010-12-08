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





#ifndef __XN_DEVICE_BASE_H__
#define __XN_DEVICE_BASE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/IXnDevice.h>
#include <XnStringsHash.h>
#include <XnDevice.h>
#include <XnDDK/XnDeviceModule.h>
#include "XnDeviceModuleHolder.h"
#include <XnEvent.h>
#include <XnDDK/XnDeviceStream.h>
#include <XnDDK/XnActualStringProperty.h>
#include <XnDDK/XnActualIntProperty.h>
#include <XnDDK/XnActualGeneralProperty.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_DEVICE						"Device"
#define XN_DEVICE_BASE_MAX_STREAMS_COUNT	100

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XN_DDK_CPP_API XnDeviceBase : public IXnDevice
{
public:
	XnDeviceBase(const XnChar* csName, XnBool bStrictProperties);
	~XnDeviceBase();

	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& ReadWriteModeProperty() { return m_ReadWriteMode; }
	inline XnActualStringProperty& PrimaryStreamProperty() { return m_PrimaryStream; }
	inline XnActualIntProperty& DeviceMirrorProperty() { return m_DeviceMirror; }

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline XnDeviceMode GetReadWriteMode() const { return (XnDeviceMode)m_ReadWriteMode.GetValue(); }
	inline const XnChar* GetPrimaryStream() const { return m_PrimaryStream.GetValue(); }
	inline XnBool GetDeviceMirror() const { return (XnBool)m_DeviceMirror.GetValue(); }
	inline XnBool IsHighResTimestamps() const { return (XnBool)m_HighResTimestamps.GetValue(); }

	inline XnDeviceModule* DeviceModule() { return m_pDevicePropertiesHolder->GetModule(); }
	inline XnDeviceModuleHolder* DeviceModuleHolder() { return m_pDevicePropertiesHolder; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetPrimaryStream(const XnChar* strPrimaryStream);
	virtual XnStatus SetMirror(XnBool bMirror);
	virtual XnStatus SetHighresTimestamps(XnBool bHighRes);

	//---------------------------------------------------------------------------
	// Helpers
	//---------------------------------------------------------------------------
	static inline XnDeviceBase* GetFromDeviceHandle(XnDeviceHandle DeviceHandle) { return (XnDeviceBase*)DeviceHandle; }

	//---------------------------------------------------------------------------
	// IXnDevice Methods
	//---------------------------------------------------------------------------
	virtual XnStatus Init(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus Destroy();
	virtual XnStatus GetSupportedStreams(const XnChar** aStreamNames, XnUInt32* pnStreamNamesCount);
	virtual XnStatus CreateStream(const XnChar* StreamType, const XnChar* StreamName = NULL, const XnPropertySet* pInitialValues = NULL);
	virtual XnStatus DestroyStream(const XnChar* StreamName);
	virtual XnStatus OpenStream(const XnChar* StreamName);
	virtual XnStatus CloseStream(const XnChar* StreamName);
	virtual XnStatus GetStreamNames(const XnChar** pstrNames, XnUInt32* pnNamesCount);
	virtual XnStatus DoesModuleExist(const XnChar* ModuleName, XnBool* pbDoesExist);
	virtual XnStatus OpenAllStreams();
	virtual XnStatus CloseAllStreams();
	virtual XnStatus RegisterToStreamsChange(XnDeviceOnStreamsChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback);
	virtual XnStatus UnregisterFromStreamsChange(XnCallbackHandle hCallback);
	virtual XnStatus CreateStreamData(const XnChar* StreamName, XnStreamData** ppStreamData);
	static XnStatus DestroyStreamData(XnStreamData** ppStreamData);
	virtual XnStatus RegisterToNewStreamData(XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback);
	virtual XnStatus UnregisterFromNewStreamData(XnCallbackHandle hCallback);
	virtual XnStatus IsNewDataAvailable(const XnChar* StreamName, XnBool* pbNewDataAvailable, XnUInt64* pnTimestamp);
	virtual XnStatus ReadStream(XnStreamData* pStreamOutput);
	virtual XnStatus Read(XnStreamDataSet* pStreamOutputSet);
	virtual XnStatus WriteStream(XnStreamData* pStreamOutput);
	virtual XnStatus Write(XnStreamDataSet* pStreamOutputSet);
	virtual XnStatus Tell(XnUInt64* pnTimestamp);
	virtual XnStatus TellFrame(XnUInt32* pnFrameID);
	virtual XnStatus DoesPropertyExist(const XnChar* ModuleName, const XnChar* PropertyName, XnBool* pbDoesExist);
	virtual XnStatus GetPropertyType(const XnChar* ModuleName, const XnChar* PropertyName, XnPropertyType* pnType);
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue);
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue);
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue);
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& Value);
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue);
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue);
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnChar* csValue);
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& pValue);
	virtual XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName);
	virtual XnStatus BatchConfig(const XnPropertySet* pChangeSet);
	virtual XnStatus GetAllProperties(XnPropertySet* pSet, XnBool bNoStreams = FALSE, const XnChar* strModule = NULL);
	virtual XnStatus RegisterToPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback);
	virtual XnStatus UnregisterFromPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnCallbackHandle hCallback);

	XN_DECLARE_EVENT_3ARG(StreamCollectionChangedEvent, StreamCollectionChangedEventInterface, XnDeviceHandle, DeviceHandle, const XnChar*, StreamName, XnStreamsChangeEventType, EventType);
	StreamCollectionChangedEventInterface& OnStreamCollectionChangedEvent() { return m_OnStreamsChangeEvent; }

	XN_DECLARE_EVENT_2ARG(NewStreamDataEvent, NewStreamDataEventInterface, XnDeviceHandle, DeviceHandle, const XnChar*, StreamName);
	NewStreamDataEventInterface& OnNewStreamDataEvent() { return m_OnNewStreamDataEvent; }

protected:
	virtual XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus CreateStreamImpl(const XnChar* strType, const XnChar* strName, const XnActualPropertiesHash* pInitialSet);

	virtual XnStatus CreateModule(const XnChar* strName, XnDeviceModuleHolder** ppModuleHolder);
	virtual XnStatus CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder);
	virtual void DestroyModule(XnDeviceModuleHolder* pModuleHolder);

	XnStatus CreateStreams(const XnPropertySet* pSet);

	/**
	* Adds a module to the device modules.
	*/
	XnStatus AddModule(XnDeviceModuleHolder* pModuleHolder);

	/**
	* Removes a module from the device modules.
	*/
	XnStatus RemoveModule(const XnChar* ModuleName);

	/**
	* Finds a module. 
	*/
	XnStatus FindModule(const XnChar* ModuleName, XnDeviceModule** ppModule);

	/**
	* Finds a module. 
	*/
	XnStatus FindModule(const XnChar* ModuleName, XnDeviceModuleHolder** ppModuleHolder);

	/**
	* Checks if a module is a stream.
	*/
	static XnBool IsStream(XnDeviceModule* pModule);

	/**
	* Finds a stream (a module which has the IS_STREAM property set to TRUE). 
	*/
	XnStatus FindStream(const XnChar* StreamName, XnDeviceStream** ppStream);

	/**
	* Finds a stream holder (a module which has the IS_STREAM property set to TRUE). 
	*/
	XnStatus FindStream(const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder);

	/**
	* Adds a stream to the list of supported streams.
	*/
	XnStatus AddSupportedStream(const XnChar* StreamType);

	/**
	* Creates a stream.
	*
	* @param	StreamType	[in]	Type of the stream to create.
	* @param	StreamName	[in]	The name of the new stream.
	*/
	virtual XnStatus CreateStreamModule(const XnChar* StreamType, const XnChar* StreamName, XnDeviceModuleHolder** ppStreamHolder) = 0;

	virtual void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder) = 0;

	/**
	* Starts a transaction.
	*/
	virtual XnStatus StartTransaction();

	/**
	* Commits a transaction.
	*/
	virtual XnStatus CommitTransaction();

	/**
	* Rollbacks a transaction.
	*/
	virtual XnStatus RollbackTransaction();

	/**
	* Gets the required output size of a stream.
	*/
	XnStatus GetStreamRequiredDataSize(const XnChar* StreamName, XnUInt32* pnRequiredSize);

	/**
	* Gets the list of modules the device supports.
	*
	* @param	aModules	[out]	an array of modules.
	* @param	pnModules	[out]	The number of modules.
	*/
	XnStatus GetModulesList(XnDeviceModuleHolder** apModules, XnUInt32* pnCount);
	XnStatus GetModulesList(XnDeviceModuleHolderList& list);

	XnStatus GetStreamsList(XnDeviceModuleHolderList& list);

	/**
	* Raises the NewStreamData event.
	*
	* @param	StreamName	[in]	The name of the stream with new data.
	*/
	XnStatus RaiseNewStreamDataEvent(const XnChar* StreamName);

	/** Gets called when a stream has new data. */
	virtual void OnNewStreamData(XnDeviceStream* pStream, XnUInt64 nTimestamp, XnUInt32 nFrameID);

	virtual XnStatus WaitForPrimaryStream(XN_EVENT_HANDLE hNewDataEvent, XnStreamDataSet* pSet);
	virtual XnStatus WaitForStream(XN_EVENT_HANDLE hNewDataEvent, XnDeviceStream* pStream);

	XnBool HasPrimaryStreamAdvanced(XnStreamDataSet* pSet);
	XnBool HasStreamAdvanced(XnDeviceStream* pStream);

	inline XnUInt64 GetLastReadTimestamp() const { return m_nLastReadTimestamp; }
	inline XnUInt32 GetLastReadFrameID() const { return m_nLastReadFrameID; }

	inline XnUInt64 GetLastTimestamp() const { return m_nLastTimestamp; }
	inline XnUInt32 GetLastFrameID() const { return m_nLastFrameID; }

	void ResetLastTimestampAndFrame();
	XnStatus ValidateOnlyModule(const XnPropertySet* pSet, const XnChar* StreamName);
	XnStatus CreateStreamsFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName);

	XnStatus StreamAdded(XnDeviceStream* pStream);

	inline XnDeviceSharingMode GetSharingMode() const { return (XnDeviceSharingMode)m_SharingMode.GetValue(); }


private:
	typedef XnBool (XnDeviceBase::*ConditionFuncPtr)(void*);

	static XnBool XN_CALLBACK_TYPE HasPrimaryStreamAdvancedCallback(void* pSet);
	static XnBool XN_CALLBACK_TYPE HasStreamAdvancedCallback(void* pStream);

	XnStatus ReadFromStreamImpl(XnDeviceStream* pStream, XnStreamData* pStreamData);
	void FreeModuleRegisteredProperties(const XnChar* strModule);

	static XnStatus XN_CALLBACK_TYPE PropertyValueChangedCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetPrimaryStreamCallback(XnActualStringProperty* pSender, const XnChar* strValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetMirrorCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetHighresTimestampsCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	static void NewStreamDataCallback(XnDeviceStream* pSender, XnUInt64 nTimestamp, XnUInt32 nFrameID, void* pCookie);

	XnBool m_bStrictProperties;

	XnDeviceModuleHolder* m_pDevicePropertiesHolder;
	XnActualIntProperty m_ReadWriteMode;
	XnActualIntProperty m_SharingMode;
	XnActualStringProperty m_PrimaryStream;
	XnActualIntProperty m_DeviceMirror;
	XnActualGeneralProperty m_SDKVersionProp;
	XnActualIntProperty m_HighResTimestamps;
	XnActualStringProperty m_DeviceName;

	/** Used to tell the read thread new data is available. */
	XN_EVENT_HANDLE m_hNewDataEvent;

	/** Keeps the last read timestamp and frame id. */
	XnUInt64 m_nLastReadTimestamp;
	XnUInt32 m_nLastReadFrameID;

	/** Keeps the latest available timestamp and frame id. */
	XnUInt64 m_nLastTimestamp;
	XnUInt32 m_nLastFrameID;

	static XnStatus XN_CALLBACK_TYPE StreamNewDataCallback(XnDeviceStream* pStream, void* pCookie);

	XnStringsHash m_Modules;
	XnStringsHash m_SupportedStreams;

	XnList m_PropertyCallbacks;

	StreamCollectionChangedEvent m_OnStreamsChangeEvent;
	NewStreamDataEvent m_OnNewStreamDataEvent;

	XnSDKVersion m_SDKVersion;
	XnDump m_StreamsDataDump;
};

#endif //__XN_DEVICE_BASE_H__
