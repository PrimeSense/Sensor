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
