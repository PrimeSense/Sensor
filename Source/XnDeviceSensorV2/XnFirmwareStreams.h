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





#ifndef __XN_FIRMWARE_STREAMS_H__
#define __XN_FIRMWARE_STREAMS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDataProcessorHolder.h"
#include <XnStringsHash.h>
#include <XnDDK/XnDeviceStream.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_STREAM_NAME_GMC "GMC"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnFirmwareStreams
{
public:
	XnFirmwareStreams(XnDevicePrivateData* pDevicePrivateData);
	~XnFirmwareStreams();

	XnStatus Init();

	XnStatus ClaimStream(const XnChar* strType, XnResolutions nRes, XnUInt32 nFPS, XnDeviceStream* pOwner);
	XnStatus ReleaseStream(const XnChar* strType, XnDeviceStream* pOwner);
	XnStatus LockStreamProcessor(const XnChar* strType, XnDeviceStream* pOwner);
	XnStatus UnlockStreamProcessor(const XnChar* strType, XnDeviceStream* pOwner);
	XnStatus ReplaceStreamProcessor(const XnChar* strType, XnDeviceStream* pOwner, XnDataProcessor* pProcessor);
	XnBool IsClaimed(const XnChar* strType, XnDeviceStream* pStream);

	void ProcessPacketChunk(XnSensorProtocolResponseHeader* pHeader, XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize);

private:
	XnStatus CheckClaimStream(const XnChar* strType, XnResolutions nRes, XnUInt32 nFPS, XnDeviceStream* pOwner);

	XnDevicePrivateData* m_pDevicePrivateData;

	class XnFirmwareStreamData
	{
	public:
		XnDataProcessorHolder* pProcessorHolder;
		const XnChar* strType;
		XnResolutions nRes;
		XnUInt32 nFPS;
		XnDeviceStream* pOwner;
	};

	XN_DECLARE_STRINGS_HASH(XnFirmwareStreamData, XnFirmwareStreamsHash)

	XnFirmwareStreamsHash m_FirmwareStreams;

	XnDataProcessorHolder m_DepthProcessor;
	XnDataProcessorHolder m_ImageProcessor;
	XnDataProcessorHolder m_AudioProcessor;

};

#endif //__XN_FIRMWARE_STREAMS_H__