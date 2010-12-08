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
