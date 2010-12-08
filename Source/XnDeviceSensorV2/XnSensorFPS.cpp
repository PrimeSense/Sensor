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
#include "XnSensorFPS.h"
#include "XnDeviceSensor.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/* The number of frames to average FPS over */
#define XN_SENSOR_FPS_FRAME_COUNT	180

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnSensorFPS::XnSensorFPS() :
	m_FramesDump(XN_DUMP_CLOSED)
{
	xnFPSInit(&m_InputDepth, XN_SENSOR_FPS_FRAME_COUNT);
	xnFPSInit(&m_InputImage, XN_SENSOR_FPS_FRAME_COUNT);
	xnFPSInit(&m_ReadCalls, XN_SENSOR_FPS_FRAME_COUNT);
	xnFPSInit(&m_OutputDepth, XN_SENSOR_FPS_FRAME_COUNT);
	xnFPSInit(&m_OutputImage, XN_SENSOR_FPS_FRAME_COUNT);

	xnDumpInit(&m_FramesDump, XN_MASK_SENSOR_FPS, "TS,Type,FrameID,FrameTS\n", "FramesTimes.csv");
}

XnSensorFPS::~XnSensorFPS()
{
	xnFPSFree(&m_InputDepth);
	xnFPSFree(&m_InputImage);
	xnFPSFree(&m_ReadCalls);
	xnFPSFree(&m_OutputDepth);
	xnFPSFree(&m_OutputImage);

	xnDumpClose(&m_FramesDump);
}

void XnSensorFPS::Mark(XnFPSData* pFPS, const XnChar* csName, XnUInt32 nFrameID, XnUInt64 nTS)
{
	if (!xnLogIsEnabled(XN_MASK_SENSOR_FPS, XN_LOG_VERBOSE))
		return;

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);

	xnFPSMarkFrame(pFPS, nNow);

	xnDumpWriteString(m_FramesDump, "%llu,%s,%u,%llu\n", nNow, csName, nFrameID, nTS);

	// get current time in seconds
	nNow /= 1000000;

	if (nNow != m_nLastPrint)
	{
		m_nLastPrint = nNow;
		xnLogVerbose(XN_MASK_SENSOR_FPS, "[FPS] InputFrames - I: %5.2f, D: %5.2f, OutputFrames - I: %5.2f, D: %5.2f",
			xnFPSCalc(&m_InputImage), xnFPSCalc(&m_InputDepth), xnFPSCalc(&m_OutputImage), xnFPSCalc(&m_OutputDepth));
	}
}
