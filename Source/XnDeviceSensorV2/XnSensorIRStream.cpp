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






//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensorInit.h"
#include "XnSensorIRStream.h"
#include "XnIRProcessor.h"
#include <XnOS.h>
#include "XnCmosInfo.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_IR_STREAM_DEFAULT_FPS				30
#define XN_IR_STREAM_DEFAULT_RESOLUTION			XN_RESOLUTION_QVGA
#define XN_IR_STREAM_DEFAULT_OUTPUT_FORMAT		XN_OUTPUT_FORMAT_RGB24
#define XN_IR_STREAM_DEFAULT_MIRROR				FALSE

#define XN_IR_MAX_BUFFER_SIZE					(XN_SXGA_X_RES * XN_SXGA_Y_RES * sizeof(XnRGB24Pixel))

//---------------------------------------------------------------------------
// XnSensorIRStream class
//---------------------------------------------------------------------------
XnSensorIRStream::XnSensorIRStream(const XnChar* strDeviceName, const XnChar* StreamName, XnSensorObjects* pObjects, XnUInt32 nBufferCount) : 
	XnIRStream(StreamName, FALSE),
	m_Helper(pObjects),
	m_BufferPool(nBufferCount, strDeviceName, StreamName, XN_IR_MAX_BUFFER_SIZE),
	m_SharedBufferName(XN_STREAM_PROPERTY_SHARED_BUFFER_NAME, m_BufferPool.GetSharedMemoryName()),
	m_FirmwareCropSizeX("FirmwareCropSizeX", 0, StreamName),
	m_FirmwareCropSizeY("FirmwareCropSizeY", 0, StreamName),
	m_FirmwareCropOffsetX("FirmwareCropOffsetX", 0, StreamName),
	m_FirmwareCropOffsetY("FirmwareCropOffsetY", 0, StreamName),
	m_FirmwareCropEnabled("FirmwareCropEnabled", FALSE, StreamName),
	m_ActualRead(XN_STREAM_PROPERTY_ACTUAL_READ_DATA, FALSE)
{
	m_ActualRead.UpdateSetCallback(SetActualReadCallback, this);
}

XnStatus XnSensorIRStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = SetBufferPool(&m_BufferPool);
	XN_IS_STATUS_OK(nRetVal);

	// init base
	nRetVal = XnIRStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	// add properties
	XN_VALIDATE_ADD_PROPERTIES(this, &m_SharedBufferName, &m_ActualRead);

	// set base properties default values
	nRetVal = ResolutionProperty().UnsafeUpdateValue(XN_IR_STREAM_DEFAULT_RESOLUTION);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FPSProperty().UnsafeUpdateValue(XN_IR_STREAM_DEFAULT_FPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = OutputFormatProperty().UnsafeUpdateValue(XN_IR_STREAM_DEFAULT_OUTPUT_FORMAT);
	XN_IS_STATUS_OK(nRetVal);

	// init helper
	nRetVal = m_Helper.Init(this, this);
	XN_IS_STATUS_OK(nRetVal);

	// data processor
	nRetVal = m_Helper.RegisterDataProcessorProperty(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	// register for mirror
	XnCallbackHandle hCallbackDummy;
	nRetVal = IsMirroredProperty().OnChangeEvent().Register(IsMirroredChangedCallback, this, &hCallbackDummy);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::Free()
{
	m_Helper.Free();
	XnIRStream::Free();
	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::MapPropertiesToFirmware()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(ResolutionProperty(), GetFirmwareParams()->m_IRResolution, FALSE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(FPSProperty(), GetFirmwareParams()->m_IRFPS, FALSE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropSizeX, GetFirmwareParams()->m_IRCropSizeX, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropSizeY, GetFirmwareParams()->m_IRCropSizeY, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetX, GetFirmwareParams()->m_IRCropOffsetX, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetY, GetFirmwareParams()->m_IRCropOffsetY, TRUE));
	XN_IS_STATUS_OK(m_Helper.MapFirmwareProperty(m_FirmwareCropEnabled, GetFirmwareParams()->m_IRCropEnabled, TRUE));

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::ConfigureStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp);

	nRetVal = SetActualRead(TRUE);
	XN_IS_STATUS_OK(nRetVal);

	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(ResolutionProperty()));
	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(FPSProperty()));

	// IR mirror is always off in firmware
	nRetVal = GetFirmwareParams()->m_IRMirror.SetValue(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	// CMOS
	if (GetResolution() != XN_RESOLUTION_SXGA)
	{
		nRetVal = m_Helper.GetCmosInfo()->SetCmosConfig(XN_CMOS_TYPE_DEPTH, GetResolution(), GetFPS());
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::SetActualRead(XnBool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_ActualRead.GetValue() != bRead)
	{
		if (bRead)
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Creating USB IR read thread...");
			nRetVal = xnUSBInitReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp, GetHelper()->GetPrivateData()->pSpecificImageUsb->nChunkReadBytes, XN_SENSOR_USB_IMAGE_BUFFERS, XN_SENSOR_READ_THREAD_TIMEOUT, XnDeviceSensorProtocolUsbEpCb, GetHelper()->GetPrivateData()->pSpecificImageUsb);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Shutting down IR image read thread...");
			xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp);
		}

		nRetVal = m_ActualRead.UnsafeUpdateValue(bRead);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::OpenStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_IR);
	XN_IS_STATUS_OK(nRetVal);

	// Cropping
	if (m_FirmwareCropEnabled.GetValue() == TRUE)
	{
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropSizeX));
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropSizeY));
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropOffsetX));
		XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropOffsetY));
	}
	XN_IS_STATUS_OK(m_Helper.ConfigureFirmware(m_FirmwareCropEnabled));


	nRetVal = XnIRStream::Open();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}


XnStatus XnSensorIRStream::CloseStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_OFF);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnIRStream::Close();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SetActualRead(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::SetOutputFormat(XnOutputFormats nOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nOutputFormat)
	{
	case XN_OUTPUT_FORMAT_RGB24:
	case XN_OUTPUT_FORMAT_GRAYSCALE16:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unsupported IR output format: %d", nOutputFormat);
	}

	nRetVal = m_Helper.BeforeSettingDataProcessorProperty();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnIRStream::SetOutputFormat(nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingDataProcessorProperty();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::SetFPS(XnUInt32 nFPS)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.BeforeSettingFirmwareParam(FPSProperty(), nFPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnIRStream::SetFPS(nFPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(FPSProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::SetResolution(XnResolutions nResolution)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nResolution)
	{
	case XN_RESOLUTION_QVGA:
	case XN_RESOLUTION_VGA:
		break;
	case XN_RESOLUTION_SXGA:
		if (m_Helper.GetFirmwareVersion() < XN_SENSOR_FW_VER_5_1)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_IO_INVALID_STREAM_IMAGE_RESOLUTION, XN_MASK_DEVICE_SENSOR, "IR resolution is not supported by this firmware!");
		}
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unsupported IR resolution: %d", nResolution);
	}

	nRetVal = m_Helper.BeforeSettingFirmwareParam(ResolutionProperty(), nResolution);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnIRStream::SetResolution(nResolution);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::SetCropping(const XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateCropping(pCropping);
	XN_IS_STATUS_OK(nRetVal);

	xnOSEnterCriticalSection(GetLock());

	if (m_Helper.GetFirmwareVersion() > XN_SENSOR_FW_VER_3_0)
	{
		nRetVal = m_Helper.StartFirmwareTransaction();
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}

		// mirror is done by software (meaning AFTER cropping, which is bad). So we need to flip the cropping area
		// to match requested area.
		XnUInt16 nXOffset = pCropping->nXOffset;
		if (IsMirrored())
		{
			nXOffset = GetXRes() - pCropping->nXOffset - pCropping->nXSize;
		}

		if (pCropping->bEnabled)
		{
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropSizeX, pCropping->nXSize);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropSizeY, pCropping->nYSize);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropOffsetX, nXOffset);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropOffsetY, pCropping->nYOffset);
		}

		if (nRetVal == XN_STATUS_OK)
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropEnabled, pCropping->bEnabled);

		if (nRetVal != XN_STATUS_OK)
		{
			m_Helper.RollbackFirmwareTransaction();
			m_Helper.UpdateFromFirmware(m_FirmwareCropEnabled);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetY);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeY);
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}

		nRetVal = m_Helper.CommitFirmwareTransactionAsBatch();
		if (nRetVal != XN_STATUS_OK)
		{
			m_Helper.UpdateFromFirmware(m_FirmwareCropEnabled);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetY);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeY);
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}
	}

	nRetVal = XnIRStream::SetCropping(pCropping);


	xnOSLeaveCriticalSection(GetLock());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::CalcRequiredSize(XnUInt32* pnRequiredSize) const
{
	// in IR, in all resolutions except SXGA, we get additional 8 lines
	XnUInt32 nYRes = GetYRes();
	if (GetResolution() != XN_RESOLUTION_SXGA)
	{
		nYRes += 8;
	}

	*pnRequiredSize = GetXRes() * nYRes * GetBytesPerPixel();
	return XN_STATUS_OK;
}

XnStatus XnSensorIRStream::ReallocTripleFrameBuffer()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (IsOpen())
	{
		// before actually replacing buffer, lock the processor (so it will not continue to 
		// use old buffer)
		nRetVal = m_Helper.GetFirmware()->GetStreams()->LockStreamProcessor(GetType(), this);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = XnIRStream::ReallocTripleFrameBuffer();
	if (nRetVal != XN_STATUS_OK)
	{
		m_Helper.GetFirmware()->GetStreams()->UnlockStreamProcessor(GetType(), this);
		return (nRetVal);
	}

	if (IsOpen())
	{
		nRetVal = m_Helper.GetFirmware()->GetStreams()->UnlockStreamProcessor(GetType(), this);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::CropImpl(XnStreamData* pStreamOutput, const XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// if firmware cropping is disabled, crop
	if (m_FirmwareCropEnabled.GetValue() == FALSE)
	{
		nRetVal = XnIRStream::CropImpl(pStreamOutput, pCropping);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::CreateDataProcessor(XnDataProcessor** ppProcessor)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDataProcessor* pNew;
	XN_VALIDATE_NEW_AND_INIT(pNew, XnIRProcessor, this, &m_Helper);

	*ppProcessor = pNew;

	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::OnIsMirroredChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// if cropping is on, we need to flip it
	XnCropping cropping = *GetCropping();
	if (cropping.bEnabled)
	{
		nRetVal = SetCropping(&cropping);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnSensorIRStream::IsMirroredChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnSensorIRStream* pThis = (XnSensorIRStream*)pCookie;
	return pThis->OnIsMirroredChanged();
}

XnStatus XN_CALLBACK_TYPE XnSensorIRStream::SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie)
{
	XnSensorIRStream* pThis = (XnSensorIRStream*)pCookie;
	return pThis->SetActualRead(nValue == TRUE);
}

