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

