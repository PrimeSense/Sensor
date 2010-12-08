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





#ifndef __XN_SENSOR_FPS_H__
#define __XN_SENSOR_FPS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnFPSCalculator.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_SENSOR_FPS	"FramesTimes"

//---------------------------------------------------------------------------
// XnSensorFPS class
//---------------------------------------------------------------------------
class XnSensorFPS
{
public:
	XnSensorFPS();
	~XnSensorFPS();

	inline void MarkInputDepth(XnUInt32 nFrameID, XnUInt64 nTS) { Mark(&m_InputDepth, "DepthInput", nFrameID, nTS); }
	inline void MarkInputImage(XnUInt32 nFrameID, XnUInt64 nTS) { Mark(&m_InputImage, "ImageInput", nFrameID, nTS); }
	inline void MarkReadCalled() { Mark(&m_ReadCalls, "ReadCalled", 0, 0); }
	inline void MarkOutputDepth(XnUInt32 nFrameID, XnUInt64 nTS) { Mark(&m_OutputDepth, "DepthOutput", nFrameID, nTS); }
	inline void MarkOutputImage(XnUInt32 nFrameID, XnUInt64 nTS) { Mark(&m_OutputImage, "ImageOutput", nFrameID, nTS); }

private:
	void Mark(XnFPSData* pFPS, const XnChar* csName, XnUInt32 nFrameID, XnUInt64 nTS);

	XnFPSData m_InputDepth;
	XnFPSData m_InputImage;
	XnFPSData m_ReadCalls;
	XnFPSData m_OutputDepth;
	XnFPSData m_OutputImage;

	XnUInt64 m_nLastPrint;
	XnDump m_FramesDump;
};

#endif //__XN_SENSOR_FPS_H__
