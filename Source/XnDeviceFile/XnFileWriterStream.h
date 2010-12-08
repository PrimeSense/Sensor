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





#ifndef __XN_FILE_STREAM_WRITER_H__
#define __XN_FILE_STREAM_WRITER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnStreamWriterStream.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnFileWriterStream : public XnStreamWriterStream
{
public:
	XnFileWriterStream(const XnChar* strType, const XnChar* strName, XnDataPacker* pDataPacker);
	~XnFileWriterStream();

	XnStatus Init();

	inline XnUInt64 GetNumberOfFrames() const { return m_NumberOfFrames.GetValue(); }

	XnUInt32 m_nNumFramesPos;

protected:
	XnStatus WriteImpl(XnStreamData* pStreamData);

private:
	XnActualIntProperty m_NumberOfFrames;
};

#endif //__XN_FILE_STREAM_WRITER_H__
