/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensorIO.h"
#include "XnDeviceSensor.h"
#include <XnStringsHashT.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_VENDOR_ID			0x1D27

//---------------------------------------------------------------------------
// Enums
//---------------------------------------------------------------------------
typedef enum
{
	XN_FW_USB_INTERFACE_ISO = 0,
	XN_FW_USB_INTERFACE_BULK = 1,
} XnFWUsbInterface;

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
XnUInt16 XnSensorIO::ms_supportedProducts[] = 
{
	0x0500,
	0x0600,
	0x0601,
};

XnUInt32 XnSensorIO::ms_supportedProductsCount = sizeof(XnSensorIO::ms_supportedProducts) / sizeof(XnSensorIO::ms_supportedProducts[0]);

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSensorIO::XnSensorIO(XN_SENSOR_HANDLE* pSensorHandle) :
	m_pSensorHandle(pSensorHandle),
	m_bMiscSupported(FALSE),
	m_bIsLowBandwidth(FALSE)
{
}

XnSensorIO::~XnSensorIO()
{
	for (XnUInt32 i = 0; i < m_aRegistrationHandles.GetSize(); ++i)
	{
		xnUSBUnregisterFromConnectivityEvents(m_aRegistrationHandles[i]);
	}
}

XnStatus XnSensorIO::OpenDevice(const XnChar* strPath)
{
	XnStatus nRetVal;
	XnUSBDeviceSpeed DevSpeed;

	nRetVal = xnUSBInit();
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_USB_ALREADY_INIT)
		return nRetVal;

	xnLogVerbose(XN_MASK_DEVICE_IO, "Connecting to USB device...");

	if (strPath == NULL || strcmp(strPath, "*:0") == 0)
	{
		// support old style API
		XnConnectionString aConnections[1];
		XnUInt32 nCount = 1;
		nRetVal = EnumerateSensors(aConnections, &nCount);
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_OUTPUT_BUFFER_OVERFLOW)
		{
			return nRetVal;
		}

		strPath = aConnections[0];
	}

	// try to open the device
	xnLogVerbose(XN_MASK_DEVICE_IO, "Trying to open sensor '%s'...", strPath);
	nRetVal = xnUSBOpenDeviceByPath(strPath, &m_pSensorHandle->USBDevice);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnUSBGetDeviceSpeed(m_pSensorHandle->USBDevice, &DevSpeed);
	XN_IS_STATUS_OK(nRetVal);

	if (DevSpeed != XN_USB_DEVICE_HIGH_SPEED)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_USB_UNKNOWN_DEVICE_SPEED, XN_MASK_DEVICE_IO, "Device is not high speed!");
	}

	// on older firmwares, control was sent over BULK endpoints. Check if this is the case
	xnLogVerbose(XN_MASK_DEVICE_IO, "Trying to open endpoint 0x4 for control out (for old firmwares)...");
	nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, 0x4, XN_USB_EP_BULK, XN_USB_DIRECTION_OUT, &m_pSensorHandle->ControlConnection.ControlOutConnectionEp);
	if (nRetVal == XN_STATUS_USB_ENDPOINT_NOT_FOUND || nRetVal == XN_STATUS_USB_WRONG_ENDPOINT_TYPE || nRetVal == XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION)
	{
		// this is not the case. use regular control endpoint (0)
		m_pSensorHandle->ControlConnection.bIsBulk = FALSE;
	}
	else
	{
		XN_IS_STATUS_OK(nRetVal);

		xnLogVerbose(XN_MASK_DEVICE_IO, "Opening endpoint 0x85 for control in...");
		nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, 0x85, XN_USB_EP_BULK, XN_USB_DIRECTION_IN, &m_pSensorHandle->ControlConnection.ControlInConnectionEp);
		XN_IS_STATUS_OK(nRetVal);

		m_pSensorHandle->ControlConnection.bIsBulk = TRUE;
	}

	nRetVal = IsSensorLowBandwidth(strPath, &m_bIsLowBandwidth);
	XN_IS_STATUS_OK(nRetVal);

	xnLogInfo(XN_MASK_DEVICE_IO, "Connected to USB device%s", m_bIsLowBandwidth ? " (LowBand)" : "");

	strcpy(m_strDeviceName, strPath);

	return XN_STATUS_OK;
}

XnStatus XnSensorIO::OpenDataEndPoints(XnSensorUsbInterface nInterface, const XnFirmwareInfo& fwInfo)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// try to set requested interface
	if (nInterface != XN_SENSOR_USB_INTERFACE_DEFAULT)
	{
		XnUInt8 nAlternativeInterface = 0;

		switch (nInterface)
		{
		case XN_SENSOR_USB_INTERFACE_ISO_ENDPOINTS:
			nAlternativeInterface = fwInfo.nISOAlternativeInterface;
			break;
		case XN_SENSOR_USB_INTERFACE_BULK_ENDPOINTS:
			nAlternativeInterface = fwInfo.nBulkAlternativeInterface;
			break;
		default:
			XN_ASSERT(FALSE);
			XN_LOG_WARNING_RETURN(XN_STATUS_USB_INTERFACE_NOT_SUPPORTED, XN_MASK_DEVICE_IO, "Unknown interface type: %d", nInterface);
		}

		xnLogVerbose(XN_MASK_DEVICE_IO, "Setting USB alternative interface to %d...", nAlternativeInterface);
		nRetVal = xnUSBSetInterface(m_pSensorHandle->USBDevice, 0, nAlternativeInterface);
		XN_IS_STATUS_OK(nRetVal);
	}

	xnLogVerbose(XN_MASK_DEVICE_IO, "Opening endpoints...");

	// up until v3.0/4.0, Image went over 0x82, depth on 0x83, audio on 0x86, and control was using bulk EPs, at 0x4 and 0x85.
	// starting v3.0/4.0, Image is at 0x81, depth at 0x82, audio/misc at 0x83, and control is using actual control EPs.
	// This means we are using the new Jungo USB Code
	XnBool bNewUSB = TRUE;

	// Depth
	m_pSensorHandle->DepthConnection.bIsISO = FALSE;

	xnLogVerbose(XN_MASK_DEVICE_IO, "Opening endpoint 0x81 for depth...");
	nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, 0x81, XN_USB_EP_BULK, XN_USB_DIRECTION_IN, &m_pSensorHandle->DepthConnection.UsbEp);
	if (nRetVal == XN_STATUS_USB_ENDPOINT_NOT_FOUND)
	{
		bNewUSB = FALSE;
		xnLogVerbose(XN_MASK_DEVICE_IO, "Endpoint 0x81 does not exist. Trying old USB: Opening 0x82 for depth...");
		nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, 0x82, XN_USB_EP_BULK, XN_USB_DIRECTION_IN, &m_pSensorHandle->DepthConnection.UsbEp);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		if (nRetVal == XN_STATUS_USB_WRONG_ENDPOINT_TYPE)
		{
			nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, 0x81, XN_USB_EP_ISOCHRONOUS, XN_USB_DIRECTION_IN, &m_pSensorHandle->DepthConnection.UsbEp);

			m_pSensorHandle->DepthConnection.bIsISO = TRUE;
		}

		bNewUSB = TRUE;

		XN_IS_STATUS_OK(nRetVal);

		if (m_pSensorHandle->DepthConnection.bIsISO  == TRUE)
		{
			xnLogVerbose(XN_MASK_DEVICE_IO, "Depth endpoint is isochronous.");
		}
		else
		{
			xnLogVerbose(XN_MASK_DEVICE_IO, "Depth endpoint is bulk.");
		}
	}
	m_pSensorHandle->DepthConnection.bIsOpen = TRUE;

	nRetVal = xnUSBGetEndPointMaxPacketSize(m_pSensorHandle->DepthConnection.UsbEp, &m_pSensorHandle->DepthConnection.nMaxPacketSize);
	XN_IS_STATUS_OK(nRetVal);

	// check this matches requested interface (unless DEFAULT was requested)
	if (nInterface == XN_SENSOR_USB_INTERFACE_BULK_ENDPOINTS && m_pSensorHandle->DepthConnection.bIsISO ||
		nInterface == XN_SENSOR_USB_INTERFACE_ISO_ENDPOINTS && !m_pSensorHandle->DepthConnection.bIsISO)
	{
		return (XN_STATUS_USB_INTERFACE_NOT_SUPPORTED);
	}

	m_interface = m_pSensorHandle->DepthConnection.bIsISO ? XN_SENSOR_USB_INTERFACE_ISO_ENDPOINTS : XN_SENSOR_USB_INTERFACE_BULK_ENDPOINTS;

	// Image
	XnUInt16 nImageEP = bNewUSB ? 0x82 : 0x83;

	m_pSensorHandle->ImageConnection.bIsISO = FALSE;

	xnLogVerbose(XN_MASK_DEVICE_IO, "Opening endpoint 0x%hx for image...", nImageEP);
	nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, nImageEP, XN_USB_EP_BULK, XN_USB_DIRECTION_IN, &m_pSensorHandle->ImageConnection.UsbEp);
	if (nRetVal == XN_STATUS_USB_WRONG_ENDPOINT_TYPE)
	{
		nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, nImageEP, XN_USB_EP_ISOCHRONOUS, XN_USB_DIRECTION_IN, &m_pSensorHandle->ImageConnection.UsbEp);

		m_pSensorHandle->ImageConnection.bIsISO = TRUE;
	}

	XN_IS_STATUS_OK(nRetVal);

	if (m_pSensorHandle->ImageConnection.bIsISO  == TRUE)
	{
		xnLogVerbose(XN_MASK_DEVICE_IO, "Image endpoint is isochronous.");
	}
	else
	{
		xnLogVerbose(XN_MASK_DEVICE_IO, "Image endpoint is bulk.");
	}

	m_pSensorHandle->ImageConnection.bIsOpen = TRUE;

	nRetVal = xnUSBGetEndPointMaxPacketSize(m_pSensorHandle->ImageConnection.UsbEp, &m_pSensorHandle->ImageConnection.nMaxPacketSize);
	XN_IS_STATUS_OK(nRetVal);

	// Misc
	XnUInt16 nMiscEP = bNewUSB ? 0x83 : 0x86;

	m_pSensorHandle->MiscConnection.bIsISO = FALSE;

	xnLogVerbose(XN_MASK_DEVICE_IO, "Opening endpoint 0x%hx for misc...", nMiscEP);
	nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, nMiscEP, XN_USB_EP_BULK, XN_USB_DIRECTION_IN, &m_pSensorHandle->MiscConnection.UsbEp);
	if (nRetVal == XN_STATUS_USB_WRONG_ENDPOINT_TYPE)
	{
		nRetVal = xnUSBOpenEndPoint(m_pSensorHandle->USBDevice, nMiscEP, XN_USB_EP_ISOCHRONOUS, XN_USB_DIRECTION_IN, &m_pSensorHandle->MiscConnection.UsbEp);

		m_pSensorHandle->MiscConnection.bIsISO = TRUE;
	}
	if (nRetVal == XN_STATUS_USB_ENDPOINT_NOT_FOUND)
	{
		// Firmware does not support misc...
		m_pSensorHandle->MiscConnection.bIsOpen = FALSE;
		m_bMiscSupported = FALSE;

		xnLogVerbose(XN_MASK_DEVICE_IO, "Misc endpoint is not supported...");
	}
	else if (nRetVal == XN_STATUS_OK)
	{
		m_pSensorHandle->MiscConnection.bIsOpen = TRUE;
		m_bMiscSupported = TRUE;

		if (m_pSensorHandle->MiscConnection.bIsISO  == TRUE)
		{ 
			xnLogVerbose(XN_MASK_DEVICE_IO, "Misc endpoint is isochronous.");
		}
		else
		{
			xnLogVerbose(XN_MASK_DEVICE_IO, "Misc endpoint is bulk.");
		}
	}
	else
	{
		return nRetVal;
	}

	if (m_pSensorHandle->MiscConnection.bIsOpen)
	{
		nRetVal = xnUSBGetEndPointMaxPacketSize(m_pSensorHandle->MiscConnection.UsbEp, &m_pSensorHandle->MiscConnection.nMaxPacketSize);
		XN_IS_STATUS_OK(nRetVal);
	}

	xnLogInfo(XN_MASK_DEVICE_IO, "Endpoints open");

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnSensorIO::CloseDevice()
{
	XnStatus nRetVal;

	xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Shutting down USB depth read thread...");
	xnUSBShutdownReadThread(m_pSensorHandle->DepthConnection.UsbEp);

	if (m_pSensorHandle->DepthConnection.UsbEp != NULL)
	{
		nRetVal = xnUSBCloseEndPoint(m_pSensorHandle->DepthConnection.UsbEp);
		XN_IS_STATUS_OK(nRetVal);
		m_pSensorHandle->DepthConnection.UsbEp = NULL;
	}

	xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Shutting down USB image read thread...");
	xnUSBShutdownReadThread(m_pSensorHandle->ImageConnection.UsbEp);

	if (m_pSensorHandle->ImageConnection.UsbEp != NULL)
	{
		nRetVal = xnUSBCloseEndPoint(m_pSensorHandle->ImageConnection.UsbEp);
		XN_IS_STATUS_OK(nRetVal);
		m_pSensorHandle->ImageConnection.UsbEp = NULL;
	}

	if (m_pSensorHandle->MiscConnection.bIsOpen)
	{
		xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Shutting down USB misc read thread...");
		xnUSBShutdownReadThread(m_pSensorHandle->MiscConnection.UsbEp);

		if (m_pSensorHandle->MiscConnection.UsbEp != NULL)
		{
			nRetVal = xnUSBCloseEndPoint(m_pSensorHandle->MiscConnection.UsbEp);
			XN_IS_STATUS_OK(nRetVal);
			m_pSensorHandle->MiscConnection.UsbEp = NULL;
		}
	}

	if (m_pSensorHandle->ControlConnection.bIsBulk)
	{
		if (m_pSensorHandle->ControlConnection.ControlInConnectionEp != NULL)
		{
			nRetVal = xnUSBCloseEndPoint(m_pSensorHandle->ControlConnection.ControlInConnectionEp);
			XN_IS_STATUS_OK(nRetVal);
			m_pSensorHandle->ControlConnection.ControlInConnectionEp = NULL;
		}

		if (m_pSensorHandle->ControlConnection.ControlOutConnectionEp != NULL)
		{
			nRetVal = xnUSBCloseEndPoint(m_pSensorHandle->ControlConnection.ControlOutConnectionEp);
			XN_IS_STATUS_OK(nRetVal);
			m_pSensorHandle->ControlConnection.ControlOutConnectionEp = NULL;
		}
	}

	if (m_pSensorHandle->USBDevice != NULL)
	{
		nRetVal = xnUSBCloseDevice(m_pSensorHandle->USBDevice);
		XN_IS_STATUS_OK(nRetVal);
		m_pSensorHandle->USBDevice = NULL;
	}

	xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Device closed successfully");

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus Enumerate(XnUInt16 nProduct, XnStringsSet& devicesSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	const XnUSBConnectionString* astrDevicePaths;
	XnUInt32 nCount;

	nRetVal = xnUSBEnumerateDevices(XN_SENSOR_VENDOR_ID, nProduct, &astrDevicePaths, &nCount);
	XN_IS_STATUS_OK(nRetVal);

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		nRetVal = devicesSet.Set(astrDevicePaths[i]);
		XN_IS_STATUS_OK(nRetVal);
	}

	xnUSBFreeDevicesList(astrDevicePaths);
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorIO::EnumerateSensors(XnConnectionString* aConnectionStrings, XnUInt32* pnCount)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnUSBInit();
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_USB_ALREADY_INIT)
		return nRetVal;

// Temporary patch: "Cache" the devices since running USB enum on the MacOSX platform takes several seconds due to problems in libusb!		
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)	
	static XnStringsSet devicesSet;
	
	if (devicesSet.Size() == 0)
	{
		// search for supported devices
		for (XnUInt32 i = 0; i < ms_supportedProductsCount; ++i)
		{
			nRetVal = Enumerate(ms_supportedProducts[i], devicesSet);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
#else
	XnStringsSet devicesSet;

	// search for supported devices
	for (XnUInt32 i = 0; i < ms_supportedProductsCount; ++i)
	{
		nRetVal = Enumerate(ms_supportedProducts[i], devicesSet);
		XN_IS_STATUS_OK(nRetVal);
	}
#endif
	
	// now copy back
	XnUInt32 nCount = 0;
	for (XnStringsSet::ConstIterator it = devicesSet.Begin(); it != devicesSet.End(); ++it, ++nCount)
	{
		if (nCount < *pnCount)
		{
			strcpy(aConnectionStrings[nCount], it->Key());
		}
	}

	if (nCount > *pnCount)
	{
		*pnCount = nCount;
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	// All is good...
	*pnCount = nCount;
	return (XN_STATUS_OK);
}

XnStatus XnSensorIO::IsSensorLowBandwidth(const XnConnectionString connectionString, XnBool* pbIsLowBandwidth)
{
	XnConnectionString cpMatchString;

	*pbIsLowBandwidth = FALSE;

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	// WAVI Detection:
	//   Normal USB string: \\?\usb#vid_1d27&pid_0600#6&XXXXXXXX&0&2
	//   WAVI USB String:   \\?\usb#vid_1d27&pid_0600#1&1d270600&2&3
	//                                                  ^^^^^^^^ - VID/PID is always repeated here with the WAVI.
	//                                                             Regular USB devices will have the port/hub chain instead.
	if ((xnOSStrCaseCmp(connectionString, "\\\\?\\usb#vid_") >= 0) && (xnOSStrLen(connectionString) > 25))
	{
		strncpy(&cpMatchString[0], &connectionString[12], 4); //VID
		strncpy(&cpMatchString[4], &connectionString[21], 4); //PID
		cpMatchString[8] = 0;

		if (strstr ((char*)connectionString,cpMatchString) != 0)
		{
			*pbIsLowBandwidth = TRUE;
		}
	}
#endif

	return (XN_STATUS_OK);
}

void XN_CALLBACK_TYPE XnSensorIO::OnConnectivityEvent(XnUSBEventArgs* pArgs, void* pCookie)
{
	XnSensorIO* pThis = (XnSensorIO*)pCookie;
	if (strcmp(pThis->m_strDeviceName, pArgs->strDevicePath) == 0)
	{
		pThis->m_pCallbackPtr(pArgs->eventType, const_cast<XnChar*>(pArgs->strDevicePath), pThis->m_pCallbackData);
	}
}

XnStatus XnSensorIO::SetCallback(XnUSBEventCallbackFunctionPtr pCallbackPtr, void* pCallbackData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (m_aRegistrationHandles.GetSize() == 0)
	{
		// register for USB events
		for (XnUInt32 i = 0; i < ms_supportedProductsCount; ++i)
		{
			XnRegistrationHandle hRegistration = NULL;
			nRetVal = xnUSBRegisterToConnectivityEvents(XN_SENSOR_VENDOR_ID, ms_supportedProducts[i], OnConnectivityEvent, this, &hRegistration);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = m_aRegistrationHandles.AddLast(hRegistration);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	// set callbacks
	m_pCallbackPtr = pCallbackPtr;
	m_pCallbackData = pCallbackData;

	return nRetVal;
}

const XnChar* XnSensorIO::GetDevicePath()
{
	return m_strDeviceName;
}
