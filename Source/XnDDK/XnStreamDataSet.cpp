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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStreamDataSetInternal.h"
#include <XnOS.h>
#include <XnDDKStatus.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_DDK_API XnStatus XnStreamDataSetCreate(XnStreamDataSet** ppStreamOutputSet)
{
	XN_VALIDATE_OUTPUT_PTR(ppStreamOutputSet);

	// allocate struct
	XN_VALIDATE_CALLOC(*ppStreamOutputSet, XnStreamDataSet, 1);
	XnStreamDataSet* pSet = (*ppStreamOutputSet);

	// allocate hash table
	pSet->pHash = XN_NEW(XnStreamDataHash);
	if (pSet->pHash == NULL)
	{
		XnStreamDataSetDestroy(ppStreamOutputSet);
		return XN_STATUS_ALLOC_FAILED;
	}

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnStreamDataSetDestroy(XnStreamDataSet** ppStreamOutputSet)
{
	XN_VALIDATE_INPUT_PTR(ppStreamOutputSet);

	XnStreamDataSet* pSet = (*ppStreamOutputSet);
	if (pSet != NULL)
	{
		for (XnStreamDataHash::Iterator it = pSet->pHash->Begin(); it != pSet->pHash->End(); ++it)
		{
			XnStreamData* pStreamData = it->Value();
			XnStreamDataDestroy(&pStreamData);
		}

		// free hash table
		XN_DELETE(pSet->pHash);

		// free struct
		XN_FREE_AND_NULL(*ppStreamOutputSet);
	}

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnStreamDataSetAdd(XnStreamDataSet* pStreamOutputSet, XnStreamData* pStreamOutput)
{
	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);
	XN_VALIDATE_INPUT_PTR(pStreamOutput);

	// make sure another object of this stream is not already in set
	XnStreamDataHash::Iterator it = pStreamOutputSet->pHash->End();
	if (XN_STATUS_NO_MATCH != pStreamOutputSet->pHash->Find(pStreamOutput->StreamName, it))
		return XN_STATUS_STREAM_OUTPUT_SET_ALREADY_IN_SET;

	return pStreamOutputSet->pHash->Set(pStreamOutput->StreamName, pStreamOutput);
}

XN_DDK_API XnStatus XnStreamDataSetRemove(XnStreamDataSet* pStreamOutputSet, XnStreamData* pStreamOutput)
{
	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);
	XN_VALIDATE_INPUT_PTR(pStreamOutput);

	for (XnStreamDataHash::Iterator it = pStreamOutputSet->pHash->Begin(); it != pStreamOutputSet->pHash->End(); ++it)
	{
		if (pStreamOutput == it->Value())
		{
			pStreamOutputSet->pHash->Remove(it);
			break;
		}
	}

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnStreamDataSetRemoveByName(XnStreamDataSet* pStreamOutputSet, const XnChar* StreamName)
{
	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);
	XN_VALIDATE_INPUT_PTR(StreamName);

	pStreamOutputSet->pHash->Remove(StreamName);

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnStreamDataSetGet(XnStreamDataSet* pStreamOutputSet, const XnChar* StreamName, XnStreamData** ppStreamOutput)
{
	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);
	XN_VALIDATE_INPUT_PTR(StreamName);
	XN_VALIDATE_OUTPUT_PTR(ppStreamOutput);

	XnStreamData* pData;
	XnStatus nRetVal = pStreamOutputSet->pHash->Get(StreamName, pData);
	XN_IS_STATUS_OK(nRetVal);
	
	*ppStreamOutput = pData;

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnStreamDataSetCopyToArray(const XnStreamDataSet* pStreamOutputSet, XnStreamData** apStreamOutputs, XnUInt32* pnCount)
{
	XN_VALIDATE_INPUT_PTR(pStreamOutputSet);
	XN_VALIDATE_INPUT_PTR(pnCount);
	
	// first check if we have enough space
	XnUInt32 nCount = pStreamOutputSet->pHash->Size();
	XnUInt32 nArraySize = *pnCount;

	*pnCount = nCount;

	if (nArraySize < nCount)
	{
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
	}

	// now copy
	XnUInt32 nIndex = 0;
	for (XnStreamDataHash::Iterator it = pStreamOutputSet->pHash->Begin(); it != pStreamOutputSet->pHash->End(); ++it)
	{
		apStreamOutputs[nIndex] = it->Value();
		nIndex++;
	}

	return (XN_STATUS_OK);
}

