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
#include "XnShiftToDepth.h"
#include <XnOS.h>
#include "XnDDK.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_DDK_API XnStatus XnShiftToDepthInit(XnShiftToDepthTables* pShiftToDepth, const XnShiftToDepthConfig* pConfig)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pConfig);

	XN_VALIDATE_ALIGNED_CALLOC(pShiftToDepth->pShiftToDepthTable, XnDepthPixel, pConfig->nDeviceMaxShiftValue+1, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALIGNED_CALLOC(pShiftToDepth->pDepthToShiftTable, XnUInt16, pConfig->nDeviceMaxDepthValue+1, XN_DEFAULT_MEM_ALIGN);
	pShiftToDepth->bIsInitialized = TRUE;

	// store allocation sizes
	pShiftToDepth->nShiftsCount = pConfig->nDeviceMaxShiftValue + 1;
	pShiftToDepth->nDepthsCount = pConfig->nDeviceMaxDepthValue + 1;

	return XnShiftToDepthUpdate(pShiftToDepth, pConfig);
}

XN_DDK_API XnStatus XnShiftToDepthUpdate(XnShiftToDepthTables* pShiftToDepth, const XnShiftToDepthConfig* pConfig)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pConfig);

	// check max shift wasn't changed (if so, memory should be re-allocated)
	if (pConfig->nDeviceMaxShiftValue > pShiftToDepth->nShiftsCount)
		return XN_STATUS_DEVICE_INVALID_MAX_SHIFT;

	// check max depth wasn't changed (if so, memory should be re-allocated)
	if (pConfig->nDeviceMaxDepthValue > pShiftToDepth->nDepthsCount)
		return XN_STATUS_DEVICE_INVALID_MAX_DEPTH;

	XnUInt32 nIndex = 0;
	XnInt16  nShiftValue = 0;
	XnDouble dFixedRefX = 0;
	XnDouble dMetric = 0;
	XnDouble dDepth = 0;
	XnDouble dPlanePixelSize = pConfig->fZeroPlanePixelSize;
	XnDouble dPlaneDsr = pConfig->nZeroPlaneDistance;
	XnDouble dPlaneDcl = pConfig->fEmitterDCmosDistance;
	XnInt32 nConstShift = pConfig->nParamCoeff * pConfig->nConstShift;

	dPlanePixelSize *= pConfig->nPixelSizeFactor;
	nConstShift /= pConfig->nPixelSizeFactor;

	XnDepthPixel* pShiftToDepthTable = pShiftToDepth->pShiftToDepthTable;
	XnUInt16* pDepthToShiftTable = pShiftToDepth->pDepthToShiftTable;

	xnOSMemSet(pShiftToDepthTable, 0, pShiftToDepth->nShiftsCount * sizeof(XnDepthPixel));
	xnOSMemSet(pDepthToShiftTable, 0, pShiftToDepth->nDepthsCount * sizeof(XnUInt16));

	XnUInt16 nLastDepth = 0;
	XnUInt16 nLastIndex = 0;

	for (nIndex = 1; nIndex < pConfig->nDeviceMaxShiftValue; nIndex++)
	{
		nShiftValue = nIndex;

		dFixedRefX = (XnDouble)(nShiftValue - nConstShift) / (XnDouble)pConfig->nParamCoeff;
		dFixedRefX -= 0.375;
		dMetric = dFixedRefX * dPlanePixelSize;
		dDepth = pConfig->nShiftScale * ((dMetric * dPlaneDsr / (dPlaneDcl - dMetric)) + dPlaneDsr);

		// check cut-offs
		if ((dDepth > pConfig->nDepthMinCutOff) && (dDepth < pConfig->nDepthMaxCutOff))
		{
			pShiftToDepthTable[nIndex] = (XnUInt16)dDepth;

			for (XnUInt16 i = nLastDepth; i < dDepth; i++)
				pDepthToShiftTable[i] = nLastIndex;

			nLastIndex = nIndex;
			nLastDepth = (XnUInt16)dDepth;
		}
	}

	for (XnUInt16 i = nLastDepth; i <= pConfig->nDeviceMaxDepthValue; i++)
		pDepthToShiftTable[i] = nLastIndex;

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnShiftToDepthConvert(XnShiftToDepthTables* pShiftToDepth, XnUInt16* pInput, XnUInt32 nInputSize, XnDepthPixel* pOutput)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);

	XnUInt16* pInputEnd = pInput + nInputSize;
	XnDepthPixel* pShiftToDepthTable = pShiftToDepth->pShiftToDepthTable;

	while (pInput != pInputEnd)
	{
		pOutput[0] = pShiftToDepthTable[pInput[0]];

		pInput++;
		pOutput++;
	}

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnShiftToDepthFree(XnShiftToDepthTables* pShiftToDepth)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);

	if (pShiftToDepth->bIsInitialized)
	{
		XN_ALIGNED_FREE_AND_NULL(pShiftToDepth->pDepthToShiftTable);
		XN_ALIGNED_FREE_AND_NULL(pShiftToDepth->pShiftToDepthTable);
		pShiftToDepth->bIsInitialized = FALSE;
	}

	return XN_STATUS_OK;
}

