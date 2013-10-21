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
