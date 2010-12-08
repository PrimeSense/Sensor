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





//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStreamDataInternal.h"
#include <XnOS.h>
#include <XnDDKStatus.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_DDK_API XnStatus XnStreamDataCreateNoBuffer(XnStreamData** ppStreamOutput, const XnChar* StreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_OUTPUT_PTR(ppStreamOutput);

	// allocate struct
	XN_VALIDATE_CALLOC(*ppStreamOutput, XnStreamData, 1);
	XnStreamData* pStreamOutput = *ppStreamOutput;

	// allocate internal data
	pStreamOutput->pInternal = (XnStreamDataInternal*)xnOSCalloc(1, sizeof(XnStreamDataInternal));
	if (pStreamOutput->pInternal == NULL)
	{
		XnStreamDataDestroy(ppStreamOutput);
		return (XN_STATUS_ALLOC_FAILED);
	}

	// fill internal data
	pStreamOutput->pInternal->bAllocated = FALSE;
	pStreamOutput->pInternal->nAllocSize = 0;
	pStreamOutput->pInternal->UpdateMode = XN_STREAM_DATA_UPDATE_AUTOMATICALLY;
	pStreamOutput->pInternal->Callback = NULL;
	pStreamOutput->pInternal->pLockedBuffer = NULL;

	// take name
	xnOSStrCopy(pStreamOutput->StreamName, StreamName, XN_DEVICE_MAX_STRING_LENGTH);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnStreamDataCreate(XnStreamData** ppStreamOutput, const XnChar* StreamName, XnUInt32 nAllocSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnStreamDataCreateNoBuffer(ppStreamOutput, StreamName);
	XN_IS_STATUS_OK(nRetVal);

	XnStreamData* pStreamOutput = *ppStreamOutput;

	// allocate buffer
	nRetVal = XnStreamDataUpdateSize(pStreamOutput, nAllocSize);
	if (nRetVal != XN_STATUS_OK)
	{
		XnStreamDataDestroy(ppStreamOutput);
		return nRetVal;
	}
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnStreamDataUpdateSize(XnStreamData* pStreamOutput, XnUInt32 nAllocSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pStreamOutput);

	// allocate new memory
	void* pNew = xnOSMallocAligned(nAllocSize, XN_DEFAULT_MEM_ALIGN);
	if (pNew == NULL)
		return (XN_STATUS_ALLOC_FAILED);

	// zero it
	xnOSMemSet(pNew, 0, nAllocSize);

	// free the buffer if it is allocated
	XN_ALIGNED_FREE_AND_NULL(pStreamOutput->pData);

	// and now set new buffer
	pStreamOutput->pData = pNew;

	// and size
	pStreamOutput->pInternal->nAllocSize = nAllocSize;

	pStreamOutput->pInternal->bAllocated = TRUE;

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnStreamDataDestroy(XnStreamData** ppStreamOutput)
{
	XN_VALIDATE_INPUT_PTR(ppStreamOutput);
	XnStreamData* pStreamOutput = *ppStreamOutput;

	if (pStreamOutput != NULL)
	{
		// only free buffer if allocated
		if (pStreamOutput->pInternal->nAllocSize != 0)
		{
			xnOSFreeAligned(pStreamOutput->pData);
		}

		pStreamOutput->pData = NULL;

		XN_FREE_AND_NULL(pStreamOutput->pInternal);

		XN_FREE_AND_NULL(*ppStreamOutput);
	}

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnStreamDataSetUpdateMode(XnStreamData* pStreamOutput, XnStreamDataUpdateMode UpdateMode, XnStreamOutputNotificationCallback Callback, void* pCallbackData)
{
	XN_VALIDATE_INPUT_PTR(pStreamOutput);

	if (UpdateMode == XN_STREAM_DATA_UPDATE_NOTIFY && Callback == NULL)
		return XN_STATUS_DEVICE_BAD_PARAM;

	pStreamOutput->pInternal->UpdateMode = UpdateMode;
	pStreamOutput->pInternal->Callback = Callback;
	pStreamOutput->pInternal->pCallbackData = pCallbackData;

	return XN_STATUS_OK;
}

void XnStreamOutputCallCallbackFunction(XnStreamData* pStreamOutput, XnUInt32 nNeededSize)
{
	if (pStreamOutput->pInternal->Callback != NULL)
	{
		pStreamOutput->pInternal->Callback(pStreamOutput, pStreamOutput->pInternal->pCallbackData, nNeededSize);
	}
}

XN_DDK_API XnStatus XnStreamDataCheckSize(XnStreamData* pStreamOutput, XnUInt32 nNeededSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_INPUT_PTR(pStreamOutput);

	// if not allocated, then nothing to check
	if (!pStreamOutput->pInternal->bAllocated)
		return (XN_STATUS_OK);

	if (nNeededSize <= pStreamOutput->pInternal->nAllocSize)
		return (XN_STATUS_OK);

	// oh no! we don't have enough space

	if (pStreamOutput->pInternal->UpdateMode == XN_STREAM_DATA_UPDATE_AUTOMATICALLY)
	{
		// reallocate
		nRetVal = XnStreamDataUpdateSize(pStreamOutput, nNeededSize);
		XN_IS_STATUS_OK(nRetVal);

		// and call callback
		XnStreamOutputCallCallbackFunction(pStreamOutput, nNeededSize);
		return XN_STATUS_OK;
	}
	else if (pStreamOutput->pInternal->UpdateMode == XN_STREAM_DATA_UPDATE_NOTIFY)
	{
		// let client decide
		XnStreamOutputCallCallbackFunction(pStreamOutput, nNeededSize);

		// check if it was fixed
		if (nNeededSize <= pStreamOutput->pInternal->nAllocSize)
			return XN_STATUS_OK;
	}

	// if we got here, problem still exists
	return XN_STATUS_STREAM_OUTPUT_BUFFER_TOO_SMALL;
}
