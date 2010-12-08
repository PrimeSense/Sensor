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
#include "XnIONetworkStream.h"
#include <XnLog.h>

#define XN_MASK_IO_NET_STREAM "IoNetStream"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnIONetworkStream::XnIONetworkStream(XN_SOCKET_HANDLE hSocket) :
	m_hSocket(hSocket),
	m_bIsConnected(TRUE),
	m_nReadTimeout(XN_WAIT_INFINITE)
{
}

XnStatus XnIONetworkStream::Init()
{
	return (XN_STATUS_OK);
}

XnStatus XnIONetworkStream::Free()
{
	m_bIsConnected = FALSE;
	//We don't close the socket here because we don't own it - whoever opened it should be the one to close it.
	return XN_STATUS_OK;
}

XnStatus XnIONetworkStream::WriteData(const XnUChar *pData, XnUInt32 nDataSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnOSSendNetworkBuffer(m_hSocket, (const XnChar*)pData, nDataSize);
	if (nRetVal != XN_STATUS_OK)
	{
		m_bIsConnected = FALSE;
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnIONetworkStream::ReadData(XnUChar *pData, XnUInt32 nDataSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt32 nTotalRead = 0;

	// read until we get all the data we want
	while (nTotalRead < nDataSize)
	{
		XnUInt32 nReadSize = nDataSize - nTotalRead;
		nRetVal = xnOSReceiveNetworkBuffer(m_hSocket, (XnChar*)(pData + nTotalRead), &nReadSize, m_nReadTimeout);
		if (nRetVal != XN_STATUS_OK)
		{
			if (nRetVal == XN_STATUS_OS_NETWORK_CONNECTION_CLOSED)
			{
				xnLogVerbose(XN_MASK_IO_NET_STREAM, "Network connection was closed gracefully");
				m_bIsConnected = FALSE;
			}
			else if (nRetVal != XN_STATUS_OS_NETWORK_TIMEOUT)
			{
				xnLogError(XN_MASK_IO_NET_STREAM, "Got an error while reading network buffer: %s", xnGetStatusString(nRetVal));
				m_bIsConnected = FALSE;
			}
			return (nRetVal);
		}

		nTotalRead += nReadSize;
	}

	return (XN_STATUS_OK);
}


void XnIONetworkStream::SetReadTimeout(XnUInt32 nMicrosecondsReadTimeout)
{
	m_nReadTimeout = nMicrosecondsReadTimeout;
}