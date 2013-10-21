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

	XnUInt64 m_nNumFramesPos;

protected:
	XnStatus WriteImpl(XnStreamData* pStreamData);

private:
	XnActualIntProperty m_NumberOfFrames;
};

#endif //__XN_FILE_STREAM_WRITER_H__
