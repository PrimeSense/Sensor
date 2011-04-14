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






#ifndef __XN_DEVICE_SENSOR_I_O_H__
#define __XN_DEVICE_SENSOR_I_O_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnUSB.h>
#include <XnStreamParams.h>
#include <XnDevice.h>

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

	XnStatus OpenDevice(const XnChar* strPath);

	XnStatus OpenDataEndPoints(XnSensorUsbInterface nInterface);

	XnSensorUsbInterface GetCurrentInterface() { return m_interface; }

	XnStatus CloseDevice();

	inline XnBool IsMiscEndpointSupported() const { return m_bMiscSupported; }
	inline XnBool IsLowBandwidth() const { return m_bIsLowBandwidth; }

	XnStatus SetCallback(XnUSBEventCallbackFunctionPtr pCallbackPtr, void* pCallbackData);

	const XnChar* GetDevicePath();

private:
	XN_SENSOR_HANDLE* m_pSensorHandle;
	XnBool m_bMiscSupported;
	XnSensorUsbInterface m_interface;
	XnChar m_strDeviceName[XN_DEVICE_MAX_STRING_LENGTH];
	XnBool m_bIsLowBandwidth;
};

#endif //__XN_DEVICE_SENSOR_I_O_H__
