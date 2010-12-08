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





#ifndef __XN_BACKWARDS_COMPATIBILITY36_H__
#define __XN_BACKWARDS_COMPATIBILITY36_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK.h>
#include <XnStreamParams.h>
#define _XN_IO_BC
#include <XnStream.h>

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------
XN_DDK_API XnStatus XnBCDepthFormatToOutputFormat(XnStreamDepthFormat nDepthFormat, XnOutputFormats* pnOutputFormat);
XN_DDK_API XnStatus XnBCOutputFormatToDepthFormat(XnOutputFormats nOutputFormat, XnStreamDepthFormat* pnDepthFormat);
XN_DDK_API XnStatus XnBCImageFormatToOutputFormat(XnStreamImageFormat nImageFormat, XnOutputFormats* pnOutputFormat);
XN_DDK_API XnStatus XnBCOutputFormatToImageFormat(XnOutputFormats nOutputFormat, XnStreamImageFormat* pnImageFormat);
XN_DDK_API XnStatus XnBCAudioFormatToOutputFormat(XnStreamAudioFormat nAudioFormat, XnOutputFormats* pnOutputFormat);
XN_DDK_API XnStatus XnBCOutputFormatToAudioFormat(XnOutputFormats nOutputFormat, XnStreamAudioFormat* pnAudioFormat);
XN_DDK_API XnStreamDepthFormat XnBCGetStreamDepthFormatFromString(const XnChar* cpDepthFormat);
XN_DDK_API XnStreamImageFormat XnBCGetStreamImageFormatFromString(const XnChar* cpImageFormat);
XN_DDK_API XnStreamAudioFormat XnBCGetStreamAudioFormatFromString(const XnChar* cpAudioFormat);
XN_DDK_API XN_STREAM_FLAGS_TYPE XnBCGetStreamFlagsFromString(XnChar* cpStreamFlags);

#endif //__XN_BACKWARDS_COMPATIBILITY36_H__
