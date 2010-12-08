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






#ifndef _XNV_DEVICE_CALLBACK_HANDLER_H_
#define _XNV_DEVICE_CALLBACK_HANDLER_H_

class XnVDevice;	// Forward declaration!
class XnVDepthMap;	// Forward declaration!

struct XnCallbackSetRegData;

/**
 * This class is the base class for classes processing callbacks from the device.
 * One should inherit from this class, and implement the methods corresponding to the events he wish
 * to handle.
 * This base class contains empty implementations for the methods.
 */
class XN_3_6_API XN_EE_CORE_API XnVDeviceCallbackHandler
{
public:
	virtual ~XnVDeviceCallbackHandler() {}
	/**
	* Called when a new depth frame is available.
	*
	* param	[in]	pSender		The device which raised the event.
	* param	[in]	dm			The new depth map.
	*/
	virtual XnBool OnNewDepthFrameAvailable(XnVDevice* /*pSender*/, XnVDepthMap& /*dm*/) { return TRUE; }

	/**
	* Called when a new depth frame is available.
	*
	* param	[in]	pSender		The device which raised the event.
	* param	[in]	im			The new image map.
	*/
	virtual XnBool OnNewImageFrameAvailable(XnVDevice* /*pSender*/, XnVImageMap& /*im*/) { return TRUE; }

	/**
	* Called when a new depth frame is available.
	*
	* param	[in]	pSender		The device which raised the event.
	* param	[in]	pBuffer		A pointer to the audio data buffer.
	* param	[in]	nBytes		The number of bytes in the buffer.
	*/
	virtual XnBool OnNewAudioBufferAvailable(XnVDevice* /*pSender*/, XnVAudioBuffer& /*ab*/) { return TRUE; }

	/**
	* Called when a new client has connected to the device
	*
	* param	[in]	pSender		The device which raised the event.
	*/
	virtual XnBool OnNewClient(XnVDevice* /*pSender*/) { return TRUE; }

	/**
	* Called when a client has changed a property of the device
	*
	* param	[in]	pSender		The device which raised the event.
	* param	[in]	pData		Information about changed property.
	*/
	virtual XnBool OnClientSetReg(XnVDevice* /*pSender*/, XnCallbackSetRegData* /*pData*/) { return TRUE; }
};

#endif //_XNV_DEVICE_CALLBACK_HANDLER_H_
