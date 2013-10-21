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
#ifndef __XN_DEVICE_SENSOR_I_O_H__
#define __XN_DEVICE_SENSOR_I_O_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnUSB.h>
#include <XnArray.h>
#include <XnStreamParams.h>
#include <XnDevice.h>
#include "XnFirmwareInfo.h"

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef struct XnUsbConnection
{
	XN_USB_EP_HANDLE UsbEp;

	XnBool bIsOpen;
	XnUInt8* pUSBBuffer;
	XnUInt32 nUSBBufferReadOffset;
	XnUInt32 nUSBBufferWriteOffset;

	XnUInt32 bIsISO;
	XnUInt32 nMaxPacketSize;
} XnUsbConnection;

typedef struct XnUsbControlConnection
{
	/* When true, control connection is implemented using bulk end points. */
	XnBool bIsBulk;
	XN_USB_EP_HANDLE ControlOutConnectionEp;
	XN_USB_EP_HANDLE ControlInConnectionEp;
} XnUsbControlConnection;

typedef struct XN_SENSOR_HANDLE
{
	XN_USB_DEV_HANDLE USBDevice;

	XnUsbControlConnection ControlConnection;
	XnUsbConnection DepthConnection;
	XnUsbConnection ImageConnection;
	XnUsbConnection MiscConnection;
	XnUInt8 nBoardVer;
} XN_SENSOR_HANDLE;

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
class XnSensorIO
{
public:
	XnSensorIO(XN_SENSOR_HANDLE* pSensorHandle);
	~XnSensorIO();

	static XnStatus EnumerateSensors(XnConnectionString* aConnectionStrings, XnUInt32* pnCount);
	static XnStatus IsSensorLowBandwidth(const XnConnectionString connectionString, XnBool* pbIsLowband);

	XnStatus OpenDevice(const XnChar* strPath, XnBool bLeanInit);

	XnStatus OpenDataEndPoints(XnSensorUsbInterface nInterface, const XnFirmwareInfo& fwInfo);

	XnSensorUsbInterface GetCurrentInterface() { return m_interface; }

	XnStatus CloseDevice();

	inline XnBool IsMiscEndpointSupported() const { return m_bMiscSupported; }
	inline XnBool IsLowBandwidth() const { return m_bIsLowBandwidth; }

	XnStatus SetCallback(XnUSBEventCallbackFunctionPtr pCallbackPtr, void* pCallbackData);

	const XnChar* GetDevicePath();

private:
	typedef struct XnUsbId
	{
		XnUInt16 vendorID;
		XnUInt16 productID;
	} XnUsbId;

	static void XN_CALLBACK_TYPE OnConnectivityEvent(XnUSBEventArgs* pArgs, void* pCookie);

	XN_SENSOR_HANDLE* m_pSensorHandle;
	XnBool m_bMiscSupported;
	XnSensorUsbInterface m_interface;
	XnChar m_strDeviceName[XN_DEVICE_MAX_STRING_LENGTH];
	XnBool m_bIsLowBandwidth;
	XnArray<XnRegistrationHandle> m_aRegistrationHandles;
	XnUSBEventCallbackFunctionPtr m_pCallbackPtr; 
	void* m_pCallbackData;
	static XnUsbId ms_supportedProducts[];
	static XnUInt32 ms_supportedProductsCount;
};

#endif //__XN_DEVICE_SENSOR_I_O_H__
