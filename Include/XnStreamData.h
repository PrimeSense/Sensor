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





#ifndef __XN_STREAM_OUTPUT_H__
#define __XN_STREAM_OUTPUT_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnStatus.h>
#include <XnStreamParams.h>
#include <XnDDK.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

struct XnStreamDataInternal; // Forward Declaration
typedef struct XnStreamDataInternal XnStreamDataInternal;

typedef enum XnStreamDataUpdateMode
{
	/* Stream output object will automatically reallocate buffer if needed. */
	XN_STREAM_DATA_UPDATE_AUTOMATICALLY,
	/* Stream output will let the client a chance to reallocate buffer. */
	XN_STREAM_DATA_UPDATE_NOTIFY,
	/* Stream output will do nothing. */
	XN_STREAM_DATA_UPDATE_OFF,
} XnStreamDataUpdateMode;

typedef struct XnStreamData
{
	/* Name of the stream this object is related to. */
	XnChar StreamName[XN_DEVICE_MAX_STRING_LENGTH];
	/* Timestamp of current data. */
	XnUInt64 nTimestamp;
	/* Frame ID of current data. For non-frame-based streams, this value is undetermined. */
	XnUInt32 nFrameID;
	/* A pointer to the data itself. This pointer should be converted according to the stream. */
	void* pData;
	/* The number of bytes written in the data buffer. */
	XnUInt32 nDataSize;
	/* True when then data is new, False otherwise. */
	XnBool bIsNew;
	/* Internal use only. */
	XnStreamDataInternal* pInternal;
} XnStreamData;

typedef void (XN_CALLBACK_TYPE* XnStreamOutputNotificationCallback)(XnStreamData* pStreamOutput, void* pCallbackData, XnUInt32 nNeededSize);

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------

/**
* Creates a new stream output object.
*
* @param	ppStreamOutput	[out]	A pointer to the newly created object.
* @param	StreamName		[in]	The name of the stream that this buffer will be used for.
* @param	nAllocSize		[in]	The number of bytes to allocate for the pData buffer.
*/
XN_DDK_API XnStatus XnStreamDataCreate(XnStreamData** ppStreamOutput, const XnChar* StreamName, XnUInt32 nAllocSize);

/**
* Destroys a previously created stream output object.
*
* @param	ppStreamOutput		[in]	A previously created stream output object.
*/
XN_DDK_API XnStatus XnStreamDataDestroy(XnStreamData** ppStreamOutput);

/**
* Updates the allocated size of a stream output object.
*
* @param	pStreamOutput	[in]	The stream output object.
* @param	nAllocSize		[in]	The number of bytes to allocate for the pData buffer.
*/
XN_DDK_API XnStatus XnStreamDataUpdateSize(XnStreamData* pStreamOutput, XnUInt32 nAllocSize);

/**
* Sets the update mode of a stream output object.
*
* @param	pStreamOutput	[in]	A stream output object.
* @param	UpdateMode		[in]	The update mode to use.
* @param	Callback		[in]	A callback to call when an update is required. if update mode is not NOTIFY, this can be null.
* @param	pCallbackData	[in]	[Optional] Data to be passed to the callback function.
*/
XN_DDK_API XnStatus XnStreamDataSetUpdateMode(XnStreamData* pStreamOutput, XnStreamDataUpdateMode UpdateMode, XnStreamOutputNotificationCallback Callback, void* pCallbackData);

/**
* Checks if the stream output object has enough space for data.
*
* @param	pStreamOutput	[in]	A stream output object.
* @param	nNeededSize		[in]	The number of needed bytes.
*/
XN_DDK_API XnStatus XnStreamDataCheckSize(XnStreamData* pStreamOutput, XnUInt32 nNeededSize);

#endif //__XN_STREAM_OUTPUT_H__