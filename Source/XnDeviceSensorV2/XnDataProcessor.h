/****************************************************************************
*                                                                           *
*  PrimeSense Sensor 5.x Alpha                                              *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of PrimeSense Sensor.                                  *
*                                                                           *
*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
*                                                                           *
****************************************************************************/
#ifndef __XN_DATA_PROCESSOR_H__
#define __XN_DATA_PROCESSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"
#include "XnDeviceSensorProtocol.h"
#include <XnLog.h>

/**
* Base class for all data processors.
*/
class XnDataProcessor
{
public:
	XnDataProcessor(XnDevicePrivateData* pDevicePrivateData, const XnChar* csName);
	virtual ~XnDataProcessor();

//---------------------------------------------------------------------------
// Methods
//---------------------------------------------------------------------------
public:
	/** 
	* Initializes a Data Processor.
	*
	* @param	pDevicePrivateData	[in]	A pointer to the device.
	* @param	csName				[in]	The name of the stream.
	*/
	virtual XnStatus Init();

	/** 
	* Handles some data from this stream.
	*
	* @param	pHeader		[in]	A pointer to current packet header.
	* @param	pData		[in]	A pointer to the data.
	* @param	nDataOffset	[in]	The offset of this data chunk inside current packet.
	* @param	nDataSize	[in]	Size of the data in bytes.
	*/
	void ProcessData(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize);

	inline XnBool ShouldUseHostTimestamps() { return m_bUseHostTimestamps; }

//---------------------------------------------------------------------------
// Virtual Functions
//---------------------------------------------------------------------------
protected:
	virtual void ProcessPacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize) = 0;
	virtual void OnPacketLost();

//---------------------------------------------------------------------------
// Utility Functions
//---------------------------------------------------------------------------
protected:
	/*
	* Gets a calculated timestamp from the device timestamp.
	*
	* @param	nDeviceTimeStamp	[in]	The device TS to translate.
	*/ 
	virtual XnUInt64 CreateTimestampFromDevice(XnUInt32 nDeviceTimeStamp);

	XnUInt64 GetHostTimestamp();

//---------------------------------------------------------------------------
// Class Members
//---------------------------------------------------------------------------
protected:
	XnDevicePrivateData* m_pDevicePrivateData;
	/* The number of bytes received so far (since last time this member was reset). */
	XnUInt32 m_nBytesReceived;
	/* Stores last packet ID */
	XnUInt16 m_nLastPacketID;
	/* The name of the stream. */
	const XnChar* m_csName;

private:
	/* Data used for calculating timestamps. */
	XnTimeStampData m_TimeStampData;
	XnBool m_bUseHostTimestamps;
};

#endif //__XN_DATA_PROCESSOR_H__
