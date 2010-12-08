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





#ifndef __XN_IO_FILE_STREAM_H__
#define __XN_IO_FILE_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnIOStream.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XN_CORE_CPP_API XnIOFileStream : public XnIOStream
{
public:
	XnIOFileStream(const XnChar* pcsFileName, XnUInt32 nFlags);
	~XnIOFileStream() { Free(); }

	virtual XnStatus WriteData(const XnUChar* pData, XnUInt32 nDataSize);
	virtual XnStatus ReadData(XnUChar* pData, XnUInt32 nDataSize);
	virtual XnStatus Init();
	virtual XnStatus Free();

	XnStatus Tell(XnUInt32* pnOffset);
	XnStatus Seek(XnUInt32 nOffset);

private:
	const XnChar* m_pcsFileName;
	XnUInt32 m_nFlags;
	XN_FILE_HANDLE m_hFile;
};

#endif //__XN_IO_FILE_STREAM_H__