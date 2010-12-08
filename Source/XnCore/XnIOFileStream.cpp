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
#include <XnIOFileStream.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnIOFileStream::XnIOFileStream(const XnChar* pcsFileName, XnUInt32 nFlags) :
	m_pcsFileName(pcsFileName), m_nFlags(nFlags)
{
}

XnStatus XnIOFileStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnOSOpenFile(m_pcsFileName, m_nFlags, &m_hFile);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnIOFileStream::Free()
{
	return xnOSCloseFile(&m_hFile);
}

XnStatus XnIOFileStream::WriteData(const XnUChar *pData, XnUInt32 nDataSize)
{
	return xnOSWriteFile(m_hFile, pData, nDataSize);
}

XnStatus XnIOFileStream::ReadData(XnUChar *pData, XnUInt32 nDataSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt32 nReadSize = nDataSize;
	nRetVal = xnOSReadFile(m_hFile, pData, &nReadSize);
	XN_IS_STATUS_OK(nRetVal);

	if (nReadSize != nDataSize)
	{
		return XN_STATUS_OS_FILE_READ_FAILED;
	}

	return (XN_STATUS_OK);
}

XnStatus XnIOFileStream::Tell(XnUInt32* pnOffset)
{
	return xnOSTellFile(m_hFile, pnOffset);
}

XnStatus XnIOFileStream::Seek(XnUInt32 nOffset)
{
	return xnOSSeekFile(m_hFile, XN_OS_SEEK_SET, nOffset);
}
