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
