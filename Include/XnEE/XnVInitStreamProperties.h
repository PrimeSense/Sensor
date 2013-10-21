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
#ifndef _XNV_INIT_STREAM_PROPERTIES_H
#define _XNV_INIT_STREAM_PROPERTIES_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <XnPlatform.h>
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This class is used to initialize a XnVDevice to the correct configuration.
 * Default values when used are:
 * All streams are off, and configuration is to be done.
 */
class XN_3_6_API XN_EE_CORE_API XnVInitStreamProperties
{
public:
	XnVInitStreamProperties();
	~XnVInitStreamProperties();

	/**
	* Add a stream to the configuration.
	* Possible stream names can be found in XnStreamParams.h
	*
	* @param	[in]	strStream	Name of the stream.
	*/
	XnStatus AddStream(const XnChar* strStream);
	/**
	* Remove a stream from the configuration.
	* Possible stream names can be found in XnStreamParams.h
	*
	* @param	[in]	strStream	Name of the stream
	*/
	XnStatus RemoveStream(const XnChar* strStream);
	/**
	* Check if stream was added to configuration.
	* Possible stream names can be found in XnStreamParams.h
	*
	* @param	[in]	strStream	Name of the stream
	*/
	XnBool IsStream(const XnChar* strStream) const;
	
	/**
	* Add a property to the stream configuration.
	* Possible stream and property names can be found in XnStreamParams.h
	*
	* @param	[in]	strStream	Name of the stream
	* @param	[in]	strProperty	Name of the property
	* @param	[in]	nValue		Value of the property
	*/
	XnStatus AddProperty(const XnChar* strStream, const XnChar* strProperty, XnUInt32 nValue);
	/**
	* Remove a property from the stream configuration.
	* Possible stream and property names can be found in XnStreamParams.h
	*
	* @param	[in]	strStream	Name of the stream
	* @param	[in]	strProperty	Name of the property
	*/
	XnStatus RemoveProperty(const XnChar* strStream, const XnChar* strProperty);
	/**
	* Get a property from the stream configuration.
	* Possible stream and property names can be found in XnStreamParams.h
	*
	* @param	[in]	strStream	Name of the stream
	* @param	[in]	strProperty	Name of the property
	* @param	[out]	nValue		Value of the property
	*/
	XnStatus GetProperty(const XnChar* strStream, const XnChar* strProperty, XnUInt32& nValue) const;
	/**
	* Check if a property was added to the stream configuration.
	* Possible stream and property names can be found in XnStreamParams.h
	*
	* @param	[in]	strStream	Name of the stream
	* @param	[in]	strProperty	Name of the property
	*/
	XnBool IsProperty(const XnChar* strStream, const XnChar* strProperty) const;

	/**
	* Set whether or not to configure the device at all.
	*
	* @param	[in]	bConfigure		Should configure?
	*/
	void SetConfigure(XnBool bConfigure);
	XnBool GetConfigure() const;
protected:
	void* m_pInternal;
};

#endif // _XNV_INIT_STREAM_PROPERTIES_H
