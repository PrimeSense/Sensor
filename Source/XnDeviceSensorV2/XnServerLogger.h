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
	inline XnServerLogger() : m_dump(XN_DUMP_CLOSED)
	{
		xnDumpInit(&m_dump, XN_MASK_SENSOR_SERVER_COMM_DUMP, "TS,Type,Size,Client\n", "%s.csv", XN_MASK_SENSOR_SERVER_COMM_DUMP);
	}

	inline void DumpMessage(const XnChar* strType, XnUInt32 nSize = 0, XnUInt32 nClientID = 0, const XnChar* strComment = "")
	{
		XnUInt64 nNow;
		xnOSGetHighResTimeStamp(&nNow);
		xnDumpWriteString(m_dump, "%llu,%s,%d,%d,%s\n", nNow, strType, nSize, nClientID, strComment);
	}

private:
	XnDump m_dump;
};

#endif // __XN_SERVER_LOGGER_H__

