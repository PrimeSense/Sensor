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






#ifndef _XN_SHIFT_TO_DEPTH_H_
#define _XN_SHIFT_TO_DEPTH_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStreamFormats.h>
#include <XnStatus.h>
#include <XnDDK.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnShiftToDepthConfig
{
	/** The zero plane distance in depth units. */
	XnDepthPixel nZeroPlaneDistance;
	/** The zero plane pixel size */
	XnFloat fZeroPlanePixelSize;
	/** The distance between the emitter and the Depth Cmos */
	XnFloat fEmitterDCmosDistance;
	/** The maximum possible shift value from this device. */
	XnUInt32 nDeviceMaxShiftValue;
	/** The maximum possible depth from this device (as opposed to a cut-off). */
	XnUInt32 nDeviceMaxDepthValue;

	XnUInt32 nConstShift;
	XnUInt32 nPixelSizeFactor;
	XnUInt32 nParamCoeff;
	XnUInt32 nShiftScale;

	XnDepthPixel nDepthMinCutOff;
	XnDepthPixel nDepthMaxCutOff;

} XnShiftToDepthConfig;

typedef struct XnShiftToDepthTables
{
	XnBool bIsInitialized;
	/** The shift-to-depth table. */
	XnDepthPixel* pShiftToDepthTable;
	/** The number of entries in the shift-to-depth table. */
	XnUInt32 nShiftsCount;
	/** The depth-to-shift table. */
	XnUInt16* pDepthToShiftTable;
	/** The number of entries in the depth-to-shift table. */
	XnUInt32 nDepthsCount;
} XnShiftToDepthTables;

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
XN_DDK_API XnStatus XnShiftToDepthInit(XnShiftToDepthTables* pShiftToDepth, const XnShiftToDepthConfig* pConfig);
XN_DDK_API XnStatus XnShiftToDepthUpdate(XnShiftToDepthTables* pShiftToDepth, const XnShiftToDepthConfig* pConfig);
XN_DDK_API XnStatus XnShiftToDepthConvert(XnShiftToDepthTables* pShiftToDepth, XnUInt16* pInput, XnUInt32 nInputSize, XnDepthPixel* pOutput);
XN_DDK_API XnStatus XnShiftToDepthFree(XnShiftToDepthTables* pShiftToDepth);

#endif //_XN_SHIFT_TO_DEPTH_H_
