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





#ifndef __I_XN_SENSOR_STREAM_H__
#define __I_XN_SENSOR_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStatus.h>
#include <XnStreamParams.h>
#include "XnSharedMemoryBufferPool.h"

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
class XnDataProcessor;

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class IXnSensorStream
{
public:
	~IXnSensorStream() {}

	virtual void GetFirmwareStreamConfig(XnResolutions* pnRes, XnUInt32* pnFPS) = 0;
	virtual XnStatus ConfigureStreamImpl() = 0;
	virtual XnStatus OpenStreamImpl() = 0;
	virtual XnStatus CloseStreamImpl() = 0;
	virtual XnStatus CreateDataProcessor(XnDataProcessor** ppProcessor) = 0;
	virtual XnStatus MapPropertiesToFirmware() = 0;
	virtual XnSharedMemoryBufferPool* GetSharedMemoryBuffer() = 0;
};

#endif //__I_XN_SENSOR_STREAM_H__
