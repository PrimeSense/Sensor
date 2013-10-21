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
#ifndef __XN_SERVER_LOGGER_H__
#define __XN_SERVER_LOGGER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_SENSOR_SERVER_COMM_DUMP					"SensorServerComm"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnServerLogger
{
public:
	inline XnServerLogger() : m_dump(NULL)
	{
		m_dump = xnDumpFileOpen(XN_MASK_SENSOR_SERVER_COMM_DUMP, "%s.csv", XN_MASK_SENSOR_SERVER_COMM_DUMP);
		xnDumpFileWriteString(m_dump, "TS,Type,Size,Client\n");
	}

	inline void DumpMessage(const XnChar* strType, XnUInt32 nSize = 0, XnUInt32 nClientID = 0, const XnChar* strComment = "")
	{
		XnUInt64 nNow;
		xnOSGetHighResTimeStamp(&nNow);
		xnDumpFileWriteString(m_dump, "%llu,%s,%d,%d,%s\n", nNow, strType, nSize, nClientID, strComment);
	}

private:
	XnDumpFile* m_dump;
};

#endif // __XN_SERVER_LOGGER_H__

