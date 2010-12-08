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






#ifndef _XNV_DEVICE_H_
#define _XNV_DEVICE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDevice.h>
#include "XnVStreamContainer.h"
#include "XnVDepthMap.h"
#include "XnVImageMap.h"
#include "XnVAudioBuffer.h"
#include "XnVStatus.h"
#include "XnVPropertySet.h"
#include "XnVEventHandlers.h"
#include <XnStringsHash.h>

#ifdef XN_USE_DEVICE_3_6
#include "XnVDeviceCallbackHandler.h"
#include "XnVInitStreamProperties.h"
#include "XnStream.h"
#include "XnIOParams.h"
#else
class XnVDeviceCallbackHandler;
class XnVInitStreamProperties;
#endif

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#ifdef XN_USE_DEVICE_3_6
	#define XNV_DEVICE_QVGA_XRES	320
	#define XNV_DEVICE_QVGA_YRES	240
	#define XNV_DEVICE_QVGA_YRES_V1	256

	#define XNV_DEVICE_VGA_XRES		640
	#define XNV_DEVICE_VGA_YRES		480
	#define XNV_DEVICE_VGA_YRES_V1	512

	#define XNV_DEVICE_SXGA_XRES	1280
	#define XNV_DEVICE_SXGA_YRES	1024
#endif 

#ifdef XN_USE_DEVICE_3_6
#define XN_IS_3_6 TRUE
#else
#define XN_IS_3_6 FALSE
#endif 

#define XNV_SENSOR_CONSTANT 1280

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnProperty; // forward declaration

// start v3.6 section - disable deprecated warnings
#pragma warning (push)
#pragma warning (disable: XN_DEPRECATED_WARNING_IDS)

/**
 * This is generally the first entry point to the Experience Engine.
 * It represents a device of any kind, and once initialized supplies Read() or Write()
 * capabilities, according to the open mode and the specific device capabilities.
 * In order to get the current frame, GetDepthMap() and GetImageMap() can be called,
 * which will return pointers to XnVDepthMap and XnVImageMap, respectively.
 * These pointers can be used through frames - once Read() is called again,
 * The internal buffer of the XnVDepthMap and XnVImageMap are overwritten.
 */

class XN_EE_CORE_API XnVDevice : public XnVStreamContainer, private XnVStreamCollectionChangedHandler
{
public:
	/**
	* Initialize Engine Core
	*
	* @param	[in]	strINIFile	File name, from which to take Core:Security and IO:WorkingDirectory
	*/
	static XnStatus EngineInit(const XnChar* strINIFile);
	static XnStatus EngineInit(const XnChar* strSecurityString, const XnChar* strWorkingDirectory);
	static XnStatus EngineShutdown();

	/**
	* A device is initially unusable, until it is opened.
	*/
	XnVDevice(XnBool bUse36 = XN_IS_3_6)
	{
		Ctor(bUse36);
	}

	virtual ~XnVDevice();

	/**
	 * Enumerates available devices.
	 *
	 * @param	aConnectionStrings		[in/out]	An array to be filled with connection strings.
	 * @param	pnCount					[in/out]	In: the size of the array. Out: the number of elements filled in the array.
	 *
	 * @returns	XN_STATUS_OUTPUT_BUFFER_OVERFLOW	The size of the array is too small. In this case, pnCount will hold needed size.
	 */
	XnStatus Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount);

	/**
	* Creates a device using definitions from an INI file.
	*
	* @param	[in]	strIniFileName	INI file to use for initialization.
	* @param	[in]	strSectionName	section name in INI file that describes the device.
	* @param	[in]	pInitialValues	[Optional] When provided, contains values to be overridden in created device.
	*/
	XnStatus CreateFromINI(const XnChar* strIniFileName, const XnChar* strSectionName, XnVPropertySet* pInitialValues = NULL);

	/**
	 * Creates a device using a connection string.
	 *
	 * @param	[in]	strConnectionString	Connection string.
	 * @param	[in]	Mode				The mode in which device should be opened.
	 * @param	[in]	pInitialValues		[Optional] When provided, contains values to be overridden in created device.
	 * @param	[in]	Sharing				[Optional] Can change the sharing mode of the device.
	 */
	XnStatus CreateByConnectionString(XnConnectionString strConnectionString, XnDeviceMode Mode, XnVPropertySet* pInitialValues = NULL, XnDeviceSharingMode Sharing = XN_DEVICE_EXCLUSIVE);

	/**
	* Creates a sensor device.
	*
	* @param	[in]	strSensorID			ID of the sensor to connect to.
	* @param	[in]	pInitialValues		[Optional] When provided, contains values to be overridden in created device.
	* @param	[in]	Sharing				[Optional] Can change the sharing mode of the sensor.
	*/
	XnStatus CreateSensor(const XnChar* strSensorID, XnVPropertySet* pInitialValues = NULL, XnDeviceSharingMode Sharing = XN_DEVICE_EXCLUSIVE);

	/**
	* Creates a file device.
	*
	* @param	[in]	strFileName			Path of the file to open.
	* @param	[in]	Mode				The mode in which device should be opened.
	* @param	[in]	pInitialValues		[Optional] When provided, contains values to be overridden in created device.
	*/
	XnStatus CreateFileDevice(const XnChar* strFileName, XnDeviceMode Mode, XnVPropertySet* pInitialValues = NULL);

	/**
	* Configures a device from an INI file.
	*
	* @param	[in]	strIniFileName		INI file to use for initialization.
	* @param	[in]	strSectionName		section name in INI file that describes the device.
	*/
	XnStatus ConfigureFromINI(const XnChar* strIniFileName, const XnChar* strSectionName);

	/**
	* Creates and configures a device using an INI file, and opens it for reading.
	* 
	* @param	[in]	strIniFileName		INI file to use for initialization.
	* @param	[in]	strSectionName		section name in INI file that describes the device.
	* @param	[in]	pInitialValues		[Optional] When provided, contains values to be overridden in created device.
	*/
	XnStatus OpenDeviceFromINI(const XnChar* strIniFileName, const XnChar* strSectionName, XnVPropertySet* pInitialValues = NULL);

	/**
	* Creates and configures a sensor using a configuration set.
	*
	* @param	[in]	strSensorID			ID of the sensor to connect to.
	* @param	[in]	Initializer			A configuration set to use.
	* @param	[in]	Sharing				[Optional] Can change the sharing mode of the sensor.
	*/
	XnStatus OpenSensor(const XnChar* strSensorID, XnVPropertySet& InitialValues, XnDeviceSharingMode Sharing = XN_DEVICE_EXCLUSIVE);

	/**
	* Creates a read file device, and opens all streams for reading.
	*
	* @param	[in]	strFileName			Path of the file to open.
	*/
	XnStatus OpenFileForReading(const XnChar* strFileName);

	/**
	* Creates a write file device, and configures it using a configuration set.
	*
	* @param	[in]	strFileName			Path of the file to open.
	* @param	[in]	Initializer			A configuration set to use.
	*/
	XnStatus OpenFileForWriting(const XnChar* strFileName, XnVPropertySet& InitialValues);

	/**
	* Close the device.
	*/
	XnStatus Close();

	/**
	* Gets a list of stream types this device supports.
	*/
	XnStatus GetSupportedStreams(const XnChar** aStreamTypes, XnUInt32* pnStreamTypesCount) const;

	/**
	* Creates a new stream in the device.
	*
	* @param	[in]	strType				The type of the stream to create
	* @param	[in]	strName				The name to give the new stream. If NULL then type will also be used as the name.
	* @param	[in]	pInitialValues		[Optional]. When provided, values in the set will replace default values in the newly created stream.
	*/
	XnStatus AddStream(const XnChar* strType, const XnChar* strName = NULL, const XnVPropertySet* pInitialValues = NULL);

	/**
	* Removes a stream from the device.
	*
	* @param	[in]	strName				The name of the stream to remove.
	*/
	XnStatus RemoveStream(const XnChar* strName);

	/**
	* Opens a stream for reading/writing.
	*
	* @param	[in]	strName				The name of the stream to open.
	*/
	XnStatus OpenStream(const XnChar* strName);

	/**
	* Closes an open stream.
	*
	* @param	[in]	strName				The name of the stream to close.
	*/
	XnStatus CloseStream(const XnChar* strName);

	/**
	* Opens all streams of the device.
	*/
	XnStatus OpenAllStreams();

	/**
	* Closes all streams of the device.
	*/
	XnStatus CloseAllStreams();

	/**
	* Gets the device current timestamp.
	*/
	XnStatus TellTime(XnUInt64* pnTimestamp);

	/**
	* Seeks the device to the specified timestamp.
	*/
	XnStatus SeekTime(XnUInt64 nTimestamp);

	/**
	* Get the ID of the current frame.
	*
	* @param	[out]	pnFrameID	The ID of the current frame.
	*/
	XnStatus TellFrame(XnUInt32* pnFrameID);
	
	/**
	* Move to a specific frame
	*
	* @param	[in]	nFrameID	The ID of the target frame.
	*/
	XnStatus SeekFrame(XnUInt32 nFrameID);

	/**
	* Check if a module (or stream) is available on the device.
	*
	* @param	[in]	strModule	Name of the Module or Stream
	*/
	XnStatus DoesModuleExist(const XnChar* strModule, XnBool* pbDoesExist) const;
	/**
	* Check if a specific property exists on the module.
	* Possible Property names are available in XnStreamParams.h
	*
	* @param	[in]	strModule	Name of the Module or Stream
	* @param	[in]	strProperty	Name of the property
	*/
	XnStatus DoesPropertyExist(const XnChar* strModule, const XnChar* strProperty, XnBool* pbDoesExist) const;

	XnStatus GetAllProperties(XnVPropertySet& PropertySet, const XnChar* strModule) const;
	virtual XnStatus GetAllProperties(XnVPropertySet& PropertySet, XnBool bNoStreams = FALSE, const XnChar* strModule = NULL) const;

	XnStatus GetStreamNames(const XnChar** pstrStreamNames, XnUInt32* pnArraySize);

	XnStatus FindStreamOfType(const XnChar* strType, XnVStream& stream);

	XnStatus RegisterForPropertyChangedEvent(const XnChar* strModule, const XnChar* strProperty, XnVModulePropertyChangedHandler* pHandler, XnCallbackHandle* phCallback);
	XnStatus RegisterForNewDataFromStreamEvent(XnVNewDataFromStreamHandler* pHandler, XnCallbackHandle* phCallback);
	XnStatus RegisterForStreamCollectionChangedEvent(XnVStreamCollectionChangedHandler* pHandler, XnCallbackHandle* phCallback);

	XnStatus UnregisterFromPropertyChangedEvent(const XnChar* strModule, const XnChar* strProperty, XnCallbackHandle hCallback);
	XnStatus UnregisterFromNewDataFromStreamEvent(XnCallbackHandle hCallback);
	XnStatus UnregisterFromStreamCollectionChangedEvent(XnCallbackHandle hCallback);

	XnStatus IsStreamSupported(const XnChar* strStream, XnBool* pbSupported) const;

	/**
	* Gets the stream data object related to the requested stream.
	*
	* @param	[in]	strStream		Name of the stream.
	* @param	[out]	pStreamData		The stream data object of this stream.
	*/
	XnStatus GetStreamData(const XnChar* strStream, XnVStreamData*& pStreamData) const;

	/**
	* Read the next frame from the device into the data buffers.
	*/
	XnStatus Read();
	/**
	* Read the next frame from the device into the data buffers. Waits till specific stream is received.
	*/
	XnStatus Read(const XnChar* strStreamName);
	/**
	* Read the next frame from the device into the data buffers. Waits till specific stream is received.
	*/
	XnStatus Read(const XnVStream& Stream);

	/**
	* Copies data into a stream. 
	*/
	XnStatus CopyFrom(XnVDevice& sourceDevice);

	/**
	* Write only a specific stream to the device. The device must be opened for writing.
	*
	* @param	[in]	Stream		The stream from which to take the buffer to write to the device.
	*/
	XnStatus Write(XnVStream& Stream);
	XnStatus Write(const XnChar* strStreamName);

	/**
	* Writes data to the underlying device. The device must be opened for writing.
	*/
	XnStatus Write();

	inline XnVDepthMap* GetDepthMap()
	{
		return m_pdmDepthMap;
	}

	inline XnVImageMap* GetImageMap()
	{
		return m_pimImageMap;
	}

	inline XnVAudioBuffer* GetAudioBuffer()
	{
		return m_pabAudioBuffer;
	}

	/**
	* Gets the IR map. If the device does not have a IR stream, NULL is returned.
	*/
	inline XnVImageMap* GetIRMap()
	{
		return m_pimIRMap;
	}

	inline const XnVDepthMap* GetDepthMap() const
	{
		return m_pdmDepthMap;
	}

	inline const XnVImageMap* GetImageMap() const
	{
		return m_pimImageMap;
	}

	inline const XnVAudioBuffer* GetAudioBuffer() const
	{
		return m_pabAudioBuffer;
	}

	inline const XnVImageMap* GetIRMap() const
	{
		return m_pimIRMap;
	}
	

	/**
	* Get a specific type of stream, if available.
	* Possible Stream names are available in XnStreamParams.h
	*
	* @param	[in]	strName	Name of the Stream
	* @param	[out]	Stream	The Stream
	*/
	XnStatus GetStream(const XnChar* strName, XnVStream& Stream);
	/**
	* Get a specific type of module, if available.
	* Possible Module names are available in XnStreamParams.h
	*
	* @param	[in]	strName	Name of the Module
	* @param	[out]	Module	The Module
	*/
	XnStatus GetModule(const XnChar* strName, XnVModule& Module);

	/**
	* Get value of a property from some module.
	*
	* @param	[in]	strModule	Name of the Module or Stream
	* @param	[in]	strProperty	Name of the Property
	* @param	[out]	nValue		The value of the Property
	*/
	XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64& nValue) const;
	XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnDouble& fValue) const;
	XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnChar* pcsValue) const;
	XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, const XnGeneralBuffer& gbValue) const;

	XnStatus GetPropertyType(const XnChar* strModule, const XnChar* strProperty, XnPropertyType& nType);

	/**
	* Set value of a property in some module.
	*
	* @param	[in]	strModule	Name of the Module or Stream
	* @param	[in]	strProperty	Name of the Property
	* @param	[in]	nValue		New value of the Property
	*/
	XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt64 nValue);
	XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnDouble fValue);
	XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, const XnChar* csValue);
	XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, const XnGeneralBuffer& gbValue);

	XnStatus BatchConfig(XnVPropertySet& ChangeSet);

protected:
	void Ctor(XnBool b36);

private:
	XnStatus Init();
	XnStatus CloseImpl();

	XnStatus CreateFromINIImpl(const XnChar* strIniFileName, const XnChar* strSectionName, XnVPropertySet* pInitialValues);
	XnStatus CreateByConnectionStringImpl(XnConnectionString strConnectionString, XnDeviceMode Mode, XnVPropertySet* pInitialValues, XnDeviceSharingMode Sharing);
	XnStatus CreateSensorImpl(const XnChar* strSensorID, XnVPropertySet* pInitialValues, XnDeviceSharingMode Sharing);
	XnStatus CreateFileImpl(const XnChar* strFileName, XnDeviceMode Mode, XnVPropertySet* pInitialValues);

	XnStatus UpdateAllStreams();

	XnStatus GetProperty40(const XnChar* strModule, const XnChar* strProperty, XnDouble& fValue) const;

	virtual XnStatus AddStreamToDevice(const XnChar* strName);
	virtual XnStatus RemoveStreamFromDevice(const XnChar* strName);

	virtual void OnStreamAdded(const XnChar* strName);
	virtual void OnStreamRemoved(const XnChar* strName);

	static XnBool IsXnVDeviceStreamProp(const XnChar* strProperty);

	static void XN_CALLBACK_TYPE OnStreamsChangedCallback(XnDeviceHandle pDeviceHandle, const XnChar* StreamName, XnStreamsChangeEventType EventType, void* pCookie);
	static void XN_CALLBACK_TYPE OnPropertyChangedCallback(XnDeviceHandle pDeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, void* pCookie);
	static void XN_CALLBACK_TYPE OnNewStreamDataCallback(XnDeviceHandle pDeviceHandle, const XnChar* StreamName, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE OnXnVDevicePropertyChanged(const XnProperty* pSender, void* pCookie);

	XnDeviceHandle m_DeviceHandle;
	XnVDepthMap* m_pdmDepthMap;
	XnVImageMap* m_pimImageMap;
	XnVImageMap* m_pimIRMap;
	XnVAudioBuffer* m_pabAudioBuffer;

	XnBool m_bValid;
	XnBool m_bWritable;
	XnBool m_bIs36;
	XnStreamDataSet* m_pStreamOutputSet;
	XnStringsHash m_StreamsHash;

	// for 3.6 BC
	mutable XnStatus m_LastStatus;
	XnVDeviceCallbackHandler* m_pCallbackHandler;
	XnChar m_strPrimary[XN_DEVICE_MAX_STRING_LENGTH];
	mutable XnGeneralBuffer m_D2STable;
	mutable XnGeneralBuffer m_S2DTable;

protected:
	XnStatus OpenFileRead(const XnChar* strFileName, XnVStreamContainer* pInitializer = NULL, XnVInitStreamProperties* pStreams = NULL);
	XnStatus OpenFileWrite(const XnChar* strFileName, XnVStreamContainer* pInitializer = NULL, XnVInitStreamProperties* pStreams = NULL);
	XnStatus GeneralOpenSensor(const XnChar* strType, const XnChar* strSensorID,  XnVStreamContainer* pInitializer = NULL, XnVInitStreamProperties* pStreams = NULL);

public:
	typedef XN_3_6_API enum
	{
		XNV_RES_DEFAULT,
		XNV_RES_QVGA = XN_RESOLUTION_QVGA,
		XNV_RES_VGA = XN_RESOLUTION_VGA,
		XNV_RES_SXVGA = XN_RESOLUTION_SXGA,
	} XnVResolution;

	/**
	* Initialize a device from an INI file. Calls EngineInit itself.
	*
	* @param	[in]	strIniFileName	INI file to use for initialization.
	* @param	[in]	strSectionName	section name in INI file that describes the device.
	* @param	[in]	pInitializer	StreamContainer (XnVDevice) to use for the stream. NULL implies use of default.
	*/
	XN_3_6_API XnStatus OpenFromINI(const XnChar* strIniFileName, const XnChar* strSectionName, XnVStreamContainer* pInitializer = NULL);

	/**
	* Initialize a device from any sensor found connected to the host.
	*
	* @param	[in]	strSensorID			connection string for the sensor
	* @param	[in]	pInitializer		StreamContainer (XnVDevice) to use for the stream. NULL implies use of default.
	* @param	[in]	pStreams			parameters for sensor initialization. NULL implies use of default.
	*/
	XN_3_6_API XnStatus OpenSensor(const XnChar* strSensorID,  XnVStreamContainer* pInitializer = NULL, XnVInitStreamProperties* pStreams = NULL);
	/**
	* Initialize a device from a file.
	*
	* @param	[in]	strFileName			.xns File to open as device
	* @param	[in]	Mode				mode in which to open the device: XN_DEVICE_MODE_READ or XN_DEVICE_MODE_WRITE
	* @param	[in]	pInitializer		StreamContainer (XnVDevice) to use for the stream. NULL implies use of default.
	* @param	[in]	pStreams			parameters to change in the initializer (or the default). NULL implies use of default.
	*/
	XN_3_6_API XnStatus OpenFile(const XnChar* strFileName, XnDeviceMode Mode, XnVStreamContainer* pInitializer = NULL, XnVInitStreamProperties* pStreams = NULL);

	/**
	* Check if a module (or stream) is available on the device.
	*
	* @param	[in]	strModule	Name of the Module or Stream
	*/
	XN_3_6_API XnBool IsModuleExist(const XnChar* strModule);
	/**
	* Check if a specific property exists on the module.
	* Possible Property names are available in XnStreamParams.h
	*
	* @param	[in]	strModule	Name of the Module or Stream
	* @param	[in]	strProperty	Name of the property
	*/
	XN_3_6_API XnBool IsPropertyExist(const XnChar* strModule, const XnChar* strProperty);

	/**
	* This function sets the user callback handler for the device.
	* 
	* @param	pCallbackHandler	[in]	A pointer to a user-defined class which inherits from XnVDeviceCallbackHandler.
	*/
	XN_3_6_API XnStatus SetCallbackHandler(XnVDeviceCallbackHandler* pCallbackHandler);

	/**
	* Write data to the device. The device must be opened for writing.
	*
	* @param	[in]	strModule	The stream under which to add the buffer.
	* @param	[in]	pBuffer		The buffer to add.
	* @param	[in]	nSize		The size of the buffer.
	* @param	[in]	nTimeStamp	The Timestamp of the written depth map.
	*/
	XN_3_6_API XnStatus Write(const XnChar* strModule, const XnChar* pBuffer, XnUInt32 nSize, XnUInt64 nTimeStamp = 0);

	/**
	* Write data to the device. The device must be opened for writing.
	*
	* @param	[in]	dmMap		The Depth map to add as the Depth Stream.
	* @param	[in]	nTimeStamp	The Timestamp of the written depth map.
	*/
	XN_3_6_API XnStatus Write(XnVDepthMap& dmMap, XnUInt64 nTimeStamp = 0);
	/**
	* Write data to the device. The device must be opened for writing.
	*
	* @param	[in]	imMap		The Image map to add as the Image Stream.
	* @param	[in]	nTimeStamp	The Timestamp of the written image map.
	*/
	XN_3_6_API XnStatus Write(XnVImageMap& imMap, XnUInt64 nTimeStamp = 0);
	/**
	* Write data to the device. The device must be opened for writing.
	*
	* @param	[in]	abBuffer	The audio buffer to add as the Audio Stream.
	* @param	[in]	nTimeStamp	The Timestamp of the written audio buffer.
	*/
	XN_3_6_API XnStatus Write(XnVAudioBuffer& abBuffer, XnUInt64 nTimeStamp = 0);

	/**
	* Will be removed in a future version.
	* For now, calling Write() just copies the buffers, but doesn't actually write to the device.
	* This function actually writes to the device.
	* It writes one buffer of each stream.
	*/
	XN_3_6_API XnStatus CommitWrite();
	/**
	* Get the last low level SDK's status.
	*/
	XN_3_6_API inline XnStatus GetLastStatus()
	{
		return m_LastStatus;
	}

	XN_3_6_API XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetProperty(const XnChar* strModule, const XnChar* strProperty, void*& pValue) const;

	XN_3_6_API XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt16 nValue);
	XN_3_6_API XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, XnUInt32 nValue);
	XN_3_6_API XnStatus SetProperty(const XnChar* strModule, const XnChar* strProperty, void* pValue);

	// For possible parameters, take a look at XnIOParams.h
	XN_3_6_API XnStatus SetIOParam(const XnChar* strParam, XnInt32 nValue);
	XN_3_6_API XnStatus SetIOParam(const XnUInt32 nParam, XnInt32 nValue);
	XN_3_6_API XnStatus SetIOParam(const XnUInt32 nParam, const void* pValue);

	XN_3_6_API XnStatus GetIOParam(const XnChar* strParam, XnInt32& nValue) const;
	XN_3_6_API XnStatus GetIOParam(const XnUInt32 nParam, XnInt32& nValue) const;
	XN_3_6_API XnStatus GetIOParam(const XnUInt32 nParam, void* pValue) const;

	XN_3_6_API XnStatus HandleDeviceCallback(XnUInt32 CallbackEvent, void* pCallbackData);

	XN_3_6_API XnStatus EnableFlag(XnInt32 nFlag);
	XN_3_6_API XnStatus DisableFlag(XnInt32 nFlag);
	XN_3_6_API XnStatus FlipFlag(XnInt32 nFlag);
	XN_3_6_API XnStatus GetFlag(XnInt32 nFlag, XnUInt32& nValue);

	XN_3_6_API XnStatus StartCaptureMode(XnBool bNewFramesOnly);
	XN_3_6_API XnStatus StopCaptureMode();

	XN_3_6_API XnStatus AddStream(const XnChar* strName);
	XN_3_6_API XnStatus AddStream(const XnChar* strName, const XnVStream& Stream);

private:
	XN_3_6_API XnStatus Write36(XnVStream& Stream);

	XN_3_6_API XnStatus GetDepthProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetDepthProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetDepthProperty(const XnChar* strProperty, XnUInt64& nValue) const;
	XN_3_6_API XnStatus GetDepthProperty(const XnChar* strProperty, void*& pValue) const;

	XN_3_6_API XnStatus GetImageProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetImageProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetImageProperty(const XnChar* strProperty, XnUInt64& nValue) const;

	XN_3_6_API XnStatus GetIRProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetIRProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetIRProperty(const XnChar* strProperty, XnUInt64& nValue) const;

	XN_3_6_API XnStatus GetAudioProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetAudioProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetAudioProperty(const XnChar* strProperty, XnUInt64& nValue) const;

	XN_3_6_API XnStatus GetShiftsProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetShiftsProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetShiftsProperty(const XnChar* strProperty, void*& pValue) const;

	XN_3_6_API XnStatus GetFixedParamsProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetFixedParamsProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetFixedParamsProperty(const XnChar* strProperty, XnDouble& fValue) const;

	XN_3_6_API XnStatus GetDeviceProperty(const XnChar* strProperty, XnUInt32& nValue) const;
	XN_3_6_API XnStatus GetDeviceProperty(const XnChar* strProperty, void*& pValue) const;

	XN_3_6_API XnStatus GetRegistrationProperty(const XnChar* strProperty, XnUInt16& nValue) const;
	XN_3_6_API XnStatus GetRegistrationProperty(const XnChar* strProperty, XnUInt32& nValue) const;

	XN_3_6_API XnStatus SetDepthProperty(const XnChar *strProperty, XnUInt16 nValue);
	XN_3_6_API XnStatus SetDepthProperty(const XnChar* strProperty, XnUInt32 nValue);
	XN_3_6_API XnStatus SetDepthProperty(const XnChar* strProperty, XnUInt64 nValue);
	XN_3_6_API XnStatus SetDepthProperty(const XnChar* strProperty, void* pValue);

	XN_3_6_API XnStatus SetImageProperty(const XnChar* strProperty, XnUInt32 nValue);
	XN_3_6_API XnStatus SetImageProperty(const XnChar* strProperty, XnUInt64 nValue);

	XN_3_6_API XnStatus SetIRProperty(const XnChar* strProperty, XnUInt32 nValue);
	XN_3_6_API XnStatus SetIRProperty(const XnChar* strProperty, void* pValue);

	XN_3_6_API XnStatus SetAudioProperty(const XnChar* strProperty, XnUInt16 nValue);
	XN_3_6_API XnStatus SetAudioProperty(const XnChar* strProperty, XnUInt32 nValue);
	XN_3_6_API XnStatus SetAudioProperty(const XnChar* strProperty, XnUInt64 nValue);

	XN_3_6_API XnStatus SetRegistrationProperty(const XnChar* strProperty, XnUInt32 nValue);

	XN_3_6_API XnStatus SetDeviceProperty(const XnChar* strProperty, XnUInt32 nValue);
	XN_3_6_API XnStatus SetDeviceProperty(const XnChar* strProperty, void* pValue);

	XN_3_6_API XnStatus ConfigureFrom36Device(XnVDevice& otherDevice);
	XN_3_6_API XnStatus ConfigureFileFrom36Device(XnVDevice& otherDevice);
	XN_3_6_API XnStatus ConfigureDefault();
	XN_3_6_API XnStatus ConfigureDevice36FromINI(const XnChar* strIniFileName, const XnChar* strSectionName, XnVStreamContainer* pInitializer);
	XN_3_6_API XnStatus ConfigureFromOldINIFileAndOpen(const XnChar* cpINIFileName, const XnChar* cpINISectionName);
	XN_3_6_API XnStatus ConfigureAGCDepthBinFromOldINIFile(const XnChar* cpINIFileName, const XnChar* cpINISectionName, XnUInt32 nBin, XnBool bMax);

	XN_3_6_API XnStatus GetModuleIntProperty36(const XnChar* Module, const XnChar* Property, XnInt32* pnValue, XnBool bAllowModuleDontExist = FALSE, XnInt32 nValueIfOff = 0) const;
	XN_3_6_API XnStatus CheckAudioStreamProperties36() const;
	XN_3_6_API XnStatus InitS2DTables36() const;
	XN_3_6_API XnStatus GetProperty36(const XnChar* strModule, const XnChar* strProperty, XnDouble& fValue) const;
	static void XN_CALLBACK_TYPE OnS2DTableChanged36(XnDeviceHandle pDeviceHandle, const XnChar* ModuleName, const XnChar* PropertyName, void* pCookie);
	static void XN_CALLBACK_TYPE OnNewStreamDataCallback36(XnDeviceHandle pDeviceHandle, const XnChar* StreamName, void* pCookie);
};

#pragma warning (pop)

#endif //_XNV_DEVICE_H_

