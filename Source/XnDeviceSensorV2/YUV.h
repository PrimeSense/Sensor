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






#ifndef _XN_YUV_H_
#define _XN_YUV_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define YUV422_U  0
#define YUV422_Y1 1
#define YUV422_V  2
#define YUV422_Y2 3
#define YUV422_BPP 4

#define YUV420_U   0
#define YUV420_Y1  1
#define YUV420_Y2  2
#define YUV420_V   3
#define YUV420_Y3  4
#define YUV420_Y4  5
#define YUV420_BPP 6

#define YUV_RED   0
#define YUV_GREEN 1
#define YUV_BLUE  2
#define YUV_RGB_BPP 3

/* The size of an input element in the stream. */
#define XN_YUV_TO_RGB_INPUT_ELEMENT_SIZE	8
/* The size of an output element in the stream. */
#define XN_YUV_TO_RGB_OUTPUT_ELEMENT_SIZE	12

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32* pnActualRead, XnUInt32* pnRGBSize);
void YUV420ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize);

#endif //_XN_BAYER_H_
