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
#include "XnStreamDevice.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamDevice::XnStreamDevice(const XnChar* strName, XnUInt32 nInternalBufferSize) :
	XnDeviceBase(strName, FALSE),
	m_pDataPacker(NULL),
	m_nInternalBufferSize(nInternalBufferSize)
{}

XnStreamDevice::~XnStreamDevice()
{
	DestroyImpl(FALSE);
}

XnStatus XnStreamDevice::Destroy()
{
	XnDeviceBase::Destroy();
	DestroyImpl(TRUE);
	return XN_STATUS_OK;
}

void XnStreamDevice::DestroyImpl(XnBool bHasVTable)
{
	if (m_pDataPacker != NULL)
	{
		XN_DELETE(m_pDataPacker);
		m_pDataPacker = NULL;
	}

	if (m_pIOStream != NULL && bHasVTable)
	{
		// we can't destroy IO stream if we no longer have V-table
		DestroyIOStreamImpl(m_pIOStream);
		m_pIOStream = NULL;
	}
}

XnStatus XnStreamDevice::Seek(XnUInt64 nTimestamp)
{
	return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED;
}

XnStatus XnStreamDevice::SeekFrame(XnUInt32 nFrameID)
{
	return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED;
}

XnStatus XnStreamDevice::InitPacker(const XnChar *strConnectionString)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// create the stream
	nRetVal = CreateIOStreamImpl(strConnectionString, m_pIOStream);
	XN_IS_STATUS_OK(nRetVal);

	// create the data packer
	m_pDataPacker = XN_NEW(XnDataPacker, m_pIOStream, m_nInternalBufferSize);
	if (m_pDataPacker == NULL)
	{
		DestroyIOStreamImpl(m_pIOStream);
		return XN_STATUS_ALLOC_FAILED;
	}

	nRetVal = m_pDataPacker->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		DestroyIOStreamImpl(m_pIOStream);
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnStreamDevice::FindStream(const XnChar *strName, XnStreamDeviceStreamHolder **ppHolder)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceModuleHolder* pHolder;
	nRetVal = XnDeviceBase::FindStream(strName, &pHolder);
	XN_IS_STATUS_OK(nRetVal);

	// we create all streams, so we know their type
	*ppHolder = (XnStreamDeviceStreamHolder*)pHolder;

	return (XN_STATUS_OK);
}

