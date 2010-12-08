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
//	XnSensorRes SensorRes;	
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

	XnStatus OpenDevice(const XnChar* strPath);

	XnStatus OpenDataEndPoints(XnSensorUsbInterface nInterface);

	XnSensorUsbInterface GetCurrentInterface() { return m_interface; }

	XnStatus CloseDevice();

	static XnStatus GetNumOfSensors(XnUInt32* pnNumSensors);

	inline XnBool IsMiscEndpointSupported() const { return m_bMiscSupported; }

	XnStatus SetCallback(XnUSBEventCallbackFunctionPtr pCallbackPtr, void* pCallbackData);

private:
	XN_SENSOR_HANDLE* m_pSensorHandle;
	XnBool m_bMiscSupported;
	XnSensorUsbInterface m_interface;
};

#endif //__XN_DEVICE_SENSOR_I_O_H__
