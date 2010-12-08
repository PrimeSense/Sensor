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





#ifndef __IXN_DEVICE_BASE_H__
#define __IXN_DEVICE_BASE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDevice.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_DDK_CPP_API IXnDevice
{
public:
	IXnDevice() {}
	virtual ~IXnDevice() {}

	inline XnDeviceHandle GetDeviceHandle() const { return (XnDeviceHandle)this; }
	static inline IXnDevice* GetFromDeviceHandle(XnDeviceHandle DeviceHandle) { return (IXnDevice*)(DeviceHandle); }

	virtual XnStatus Init(const XnDeviceConfig* pDeviceConfig) = 0;
	virtual XnStatus Destroy() = 0;

	/**
	* Returns the types of the streams supported by this device.
	* 
	* @param	aStreamName			[in/out]	An array of stream names. Will be filled by the function.
	* @param	pnStreamNamesCount	[in/out]	The size of the array. Upon successful return, will contain the number of elements written to the array.
	*/
	virtual XnStatus GetSupportedStreams(const XnChar** aStreamNames, XnUInt32* pnStreamNamesCount) = 0;

	/**
	* Creates a new stream in the device.
	* 
	* @param	StreamType		[in]	The type of the stream to create (one of the types returned by XnDeviceEnumerateStreams).
	* @param	StreamName		[in]	[Optional] A name for the new stream.
	* @param	pInitialValues	[in]	[Optional] A set of initial values for properties.
	*/
	virtual XnStatus CreateStream(const XnChar* StreamType, const XnChar* StreamName = NULL, const XnPropertySet* pInitialValues = NULL) = 0;

	/**
	* Destroys a previously created stream.
	* 
	* @param	StreamName		[in]	The name of the stream to destroy.
	*/
	virtual XnStatus DestroyStream(const XnChar* StreamName) = 0;

	/**
	* Opens a stream for I/O operations.
	* 
	* @param	StreamName		[in]	The name of the stream to open.
	*/
	virtual XnStatus OpenStream(const XnChar* StreamName) = 0;

	/**
	* Closes an open stream.
	* 
	* @param	StreamName		[in]	The name of the stream to close.
	*/
	virtual XnStatus CloseStream(const XnChar* StreamName) = 0;

	/**
	* Get a list of all the streams that exist in the device.
	* 
	* @param	pstrStreamNames		[in/out]	An array of stream names. Will be filled by the function.
	* @param	pnNamesCount		[in/out]	The size of the array. Upon successful return, will contain the number of elements written to the array.
	*/
	virtual XnStatus GetStreamNames(const XnChar** pstrNames, XnUInt32* pnNamesCount) = 0;

	/**
	* Checks if a specific module exists in this device.
	* 
	* @param	ModuleName		[in]	The name of the module to look for.
	* @param	pbDoesExist		[out]	TRUE if the module exists, FALSE otherwise.
	*/
	virtual XnStatus DoesModuleExist(const XnChar* ModuleName, XnBool* pbDoesExist) = 0;

	/**
	* Opens all closed streams.
	*/
	virtual XnStatus OpenAllStreams() = 0;

	/**
	* Closes all open streams.
	*/
	virtual XnStatus CloseAllStreams() = 0;

	/**
	* Registers to the event of streams change (stream created / destroyed)
	* 
	* @param	Handler			[in]	A pointer to the function that will handle the event.
	* @param	pCookie			[in]	User cookie that will be passed as an argument to the event handler.
	* @param	phCallback		[out]	A handle for unregister.
	*/
	virtual XnStatus RegisterToStreamsChange(XnDeviceOnStreamsChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback) = 0;

	/**
	* Unregisters from the event of streams change (stream created / destroyed)
	* 
	* @param	hCallback		[in]	The handle returned from RegisterToStreamsChange.
	*/
	virtual XnStatus UnregisterFromStreamsChange(XnCallbackHandle hCallback) = 0;

	/**
	* Creates a stream data object for the requested stream.
	* 
	* @param	StreamName		[in]	The requested stream.
	* @param	ppStreamData	[out]	The created stream data object.
	*/
	virtual XnStatus CreateStreamData(const XnChar* StreamName, XnStreamData** ppStreamData) = 0;

	/**
	* Registers to the event of new data from a stream.
	* 
	* @param	Handler			[in]	A pointer to the function that will handle the event.
	* @param	pCookie			[in]	User cookie that will be passed as an argument to the event handler.
	* @param	phCallback		[out]	A handle for unregister.
	*/
	virtual XnStatus RegisterToNewStreamData(XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback) = 0;

	/**
	* Unregisters from the event of new data from a stream.
	* 
	* @param	hCallback		[in]	The handle returned from RegisterToNewStreamData.
	*/
	virtual XnStatus UnregisterFromNewStreamData(XnCallbackHandle hCallback) = 0;

	/**
	* Checks if new data is available from stream.
	* 
	* @param	StreamName			[in]	The name of the stream to check.
	* @param	pbNewDataAvailable	[out]	TRUE if new data is available, FALSE otherwise.
	*/
	virtual XnStatus IsNewDataAvailable(const XnChar* StreamName, XnBool* pbNewDataAvailable, XnUInt64* pnTimestamp) = 0;

	/**
	* Waits for new data to be available from requested stream, and then return it.
	* 
	* @param	pStreamOutput	[in/out]	A stream output object. The function will use the stream output object to determine which stream to read.
	*/
	virtual XnStatus ReadStream(XnStreamData* pStreamOutput) = 0;

	/**
	* Waits for new data from the primary stream to be available, and then reads all requested streams.
	* 
	* @param	pStreamOutputSet	[in/out]	A set of stream output objects.
	*/
	virtual XnStatus Read(XnStreamDataSet* pStreamOutputSet) = 0;

	/**
	* Writes a single stream data to the device.
	* 
	* @param	pStreamOutput	[in]	A stream output object.
	*/
	virtual XnStatus WriteStream(XnStreamData* pStreamOutput) = 0;

	/**
	* Writes multiple streams to the device.
	* 
	* @param	pStreamOutputSet	[in]	A set of stream output objects.
	*/
	virtual XnStatus Write(XnStreamDataSet* pStreamOutputSet) = 0;

	/**
	* Gets current position of the device.
	* 
	* @param	pnTimestamp		[out]	Current device timestamp.
	*/
	virtual XnStatus Tell(XnUInt64* pnTimestamp) = 0;

	/**
	* Seeks the device to the requested position.
	* 
	* @param	nTimestamp		[in]	Requested device timestamp.
	*/
	virtual XnStatus Seek(XnUInt64 nTimestamp) = 0;

	/**
	* Gets current frame position of the device.
	* 
	* @param	DeviceHandle	[in]	The requested device handle.
	* @param	pnFrameID		[out]	Current device frame.
	*/
	virtual XnStatus TellFrame(XnUInt32* pnFrameID) = 0;

	/**
	* Seeks the device to the requested frame position.
	* 
	* @param	DeviceHandle	[in]	The requested device handle.
	* @param	nFrameID		[in]	Requested device frame.
	*/
	virtual XnStatus SeekFrame(XnUInt32 nFrameID) = 0;

	/**
	* Checks if a specific property exists in a module.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	pbDoesExist		[out]	TRUE if the property exists, FALSE otherwise.
	*/
	virtual XnStatus DoesPropertyExist(const XnChar* ModuleName, const XnChar* PropertyName, XnBool* pbDoesExist) = 0;

	/**
	* Returns the type of a specific property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	pnType			[out]	Type of this property.
	*/
	virtual XnStatus GetPropertyType(const XnChar* ModuleName, const XnChar* PropertyName, XnPropertyType* pnType) = 0;

	/**
	* Sets the value of an int property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	nValue			[in]	New requested value.
	*/
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue) = 0;

	/**
	* Sets the value of a real property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	dValue			[in]	New requested value.
	*/
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue) = 0;

	/**
	* Sets the value of a string property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	csValue			[in]	New requested value.
	*/
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue) = 0;

	/**
	* Sets the value of a general property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	gbValue			[in]	New requested value.
	*/
	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& gbValue) = 0;

	/**
	* Gets the value of an int property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	pnValue			[out]	Current value.
	*/
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue) = 0;

	/**
	* Gets the value of a real property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	pdValue			[out]	Current value.
	*/
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue) = 0;

	/**
	* Gets the value of a string property.
	* 
	* @param	ModuleName		[in]		Name of the module.
	* @param	PropertyName	[in]		Name of the property to change.
	* @param	csValue			[in/out]	Current value. The passed buffer should be of size XN_DEVICE_MAX_STRING_LENGTH.
	*/
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnChar* csValue) = 0;

	/**
	* Gets the value of a general property.
	* 
	* @param	ModuleName		[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to change.
	* @param	gbValue			[out]	A buffer to fill.
	*/
	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& gbValue) = 0;

	/**
	* Loads configuration from INI file.
	* 
	* @param	csINIFilePath	[in]	A path to the INI file.
	* @param	csSectionName	[in]	The name of the section containing configuration.
	*/
	virtual XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName) = 0;

	/**
	* Batch-Configures device. All the properties in the set will be set as a single transaction.
	*
	* @param	pChangeSet		[in]	A set of properties to be changed.
	*/
	virtual XnStatus BatchConfig(const XnPropertySet* pChangeSet) = 0;

	/**
	* Gets all the properties of a device.
	* 
	* @param	pSet			[in]	The property set to be filled.
	*/
	virtual XnStatus GetAllProperties(XnPropertySet* pSet, XnBool bNoStreams = FALSE, const XnChar* strModule = NULL) = 0;

	/**
	* Registers an event handler to the Property Changed event of a specific property.
	* 
	* @param	Module			[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to register to.
	* @param	Handler			[in]	A pointer to the function that will handle the event.
	* @param	pCookie			[in]	User cookie that will be passed as an argument to the event handler.
	* @param	phCallback		[out]	A handle for unregister.
	*/
	virtual XnStatus RegisterToPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback) = 0;

	/**
	* Unregisters an event handler from the Property Changed event.
	* 
	* @param	Module			[in]	Name of the module.
	* @param	PropertyName	[in]	Name of the property to register to.
	* @param	hCallback		[in]	The handle returned from RegisterToNewStreamData.
	*/
	virtual XnStatus UnregisterFromPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnCallbackHandle hCallback) = 0;
};

#endif //__IXN_DEVICE_BASE_H__