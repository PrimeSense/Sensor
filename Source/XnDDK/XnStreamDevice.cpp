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

XnStatus XnStreamDevice::Seek(XnUInt64 /*nTimestamp*/)
{
	return XN_STATUS_IO_DEVICE_FUNCTION_NOT_SUPPORTED;
}

XnStatus XnStreamDevice::SeekFrame(XnUInt32 /*nFrameID*/)
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

