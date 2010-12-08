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






#ifndef _XN_BAYER_H_
#define _XN_BAYER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define BAYER_RED 0
#define BAYER_GREEN 1
#define BAYER_BLUE 2
#define BAYER_BPP 3

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
void BayerUpdateGamma(float fGammaCorr);
void Bayer2RGB888(const XnUInt8* pBayerImage, XnUInt8* pRGBImage, XnUInt32 nXRes, XnUInt32 nYRes, XnUInt32 nDownSampleStep, XnUInt32 nBadPixels);

#endif //_XN_BAYER_H_
