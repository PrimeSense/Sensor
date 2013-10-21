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
#ifndef _XN_DEVICEFILE_H_
#define _XN_DEVICEFILE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#ifndef XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	#undef XN_DEVICE_EXPORT_PREFIX
	#define XN_DEVICE_EXPORT_PREFIX File_
#endif

#include <XnDDK/XnDeviceBaseProxy.h>
#include <XnDDK/XnDeviceBase.h>

// #include <XnOS.h>
// #include <XnDDK/XnStreamUtils.h>
// #include <XnDDK/XnShiftToDepth.h>
// #include <XnFormats/XnStreamPacking.h>
// #include <XnFormats/XnStreamPackingBC.h>
//#include <XnDevice.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DEVICE_MAJORVERSION 1
#define XN_DEVICE_MINORVERSION 0
#define XN_DEVICE_NAME "File"
#define XN_DEVICE_DESCRIPTION "Xiron I/O File Device"

#define XN_MASK_FILE	"DeviceFile"

#define XN_DEVICE_FILE_DEFAULT_DEPTH_COMP_FORMAT XN_STREAM_DEFAULT_DEPTH_COMPRESSION_FORMAT;
#define XN_DEVICE_FILE_DEFAULT_IMAGE_COMP_FORMAT XN_STREAM_DEFAULT_IMAGE_COMPRESSION_FORMAT;
#define XN_DEVICE_FILE_DEFAULT_MISC_COMP_FORMAT XN_STREAM_DEFAULT_MISC_COMPRESSION_FORMAT;
#define XN_DEVICE_FILE_DEFAULT_AUDIO_COMP_FORMAT XN_STREAM_DEFAULT_AUDIO_COMPRESSION_FORMAT;

#define XN_DEVICE_FILE_MAX_INTERNAL_BUFFER	(30 * 1024 * 1024)

/** The length of the Xiron data packer string prefix. */ 
#define XN_DEVICE_FILE_MAGIC_LEN 4

/** The string prefix of every Xiron stream. */ 
#define XN_DEVICE_FILE_MAGIC_V1 "XS10"
#define XN_DEVICE_FILE_MAGIC_V2 "XS20"
#define XN_DEVICE_FILE_MAGIC_V3 "XS30"
#define XN_DEVICE_FILE_MAGIC_V4 "XS40"

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

class XnDeviceFile : public XnDeviceBaseProxy
{
public:
	XnDeviceFile();
	~XnDeviceFile();

	static XnStatus GetDefinition(XnDeviceDefinition* pDeviceDefinition);
	static XnStatus Enumerate(XnConnectionString* aConnectionStrings, XnUInt32* pnCount);

	virtual XnStatus Init(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus Destroy();

	static XnStatus DestroyStreamData(XnStreamData** ppStreamData)
	{
		return XnDeviceBase::DestroyStreamData(ppStreamData);
	}
};

typedef struct XnDepthCutOff
{
	XnBool bEnabled;
	XnDepthPixel nMin;
	XnDepthPixel nMax;
} XnDepthCutOff;

#endif //_XN_DEVICEFILE_H_
