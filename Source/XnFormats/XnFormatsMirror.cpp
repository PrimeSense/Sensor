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
#include <XnPlatform.h>
#include <XnCore.h>
#include "XnFormats.h"
#include <XnOS.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MIRROR_MAX_LINE_SIZE	1920*3

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnMirrorOneBytePixels(XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nLineSize)
{
	// Local function variables
	XnUInt8* pSrc = pBuffer;
	XnUInt8 pLineBuffer[XN_MIRROR_MAX_LINE_SIZE];
	XnUInt8* pSrcEnd = pSrc + nBufferSize;
	XnUInt8* pDest = NULL;
	XnUInt8* pDestVal = &pLineBuffer[0] + nLineSize - 1;
	XnUInt8* pDestEnd = &pLineBuffer[0] - 1;

	if (nLineSize > XN_MIRROR_MAX_LINE_SIZE)
	{
		return (XN_STATUS_INTERNAL_BUFFER_TOO_SMALL);
	}

	while (pSrc < pSrcEnd)
	{
		xnOSMemCopy(pLineBuffer, pSrc, nLineSize);

		pDest = pDestVal;
		while (pDest != pDestEnd)
		{
			*pSrc = *pDest;

			pSrc++;
			pDest--;
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnMirrorTwoBytePixels(XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nLineSize)
{
	// Local function variables
	XnUInt16* pSrc = (XnUInt16*)pBuffer;
	XnUInt16 pLineBuffer[XN_MIRROR_MAX_LINE_SIZE];
	XnUInt16* pSrcEnd = pSrc + nBufferSize / sizeof(XnUInt16);
	XnUInt16* pDest = NULL;
	XnUInt16* pDestVal = &pLineBuffer[0] + nLineSize - 1;
	XnUInt16* pDestEnd = &pLineBuffer[0] - 1;
	XnUInt16 nMemCpyLineSize = nLineSize * sizeof(XnUInt16);
	XnUInt16 nValue;

	if (nLineSize > XN_MIRROR_MAX_LINE_SIZE)
	{
		return (XN_STATUS_INTERNAL_BUFFER_TOO_SMALL);
	}

	while (pSrc < pSrcEnd)
	{
		xnOSMemCopy(pLineBuffer, pSrc, nMemCpyLineSize);

		pDest = pDestVal;
		while (pDest != pDestEnd)
		{
			nValue = pDest[0];
			pSrc[0] = nValue;

			pDest--;
			pSrc++;
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnMirrorThreeBytePixels(XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nLineSize)
{
	// Local function variables
	XnUInt8* pSrc = pBuffer;
	XnUInt8 pLineBuffer[XN_MIRROR_MAX_LINE_SIZE];
	XnUInt8* pSrcEnd = pSrc + nBufferSize;
	XnUInt8* pDest = NULL;
	XnUInt8* pDestVal = &pLineBuffer[0] + nLineSize * 3 - 1;
	XnUInt8* pDestEnd = &pLineBuffer[0] - 1;
	XnUInt16 nMemCpyLineSize = nLineSize * 3;

	if (nMemCpyLineSize > XN_MIRROR_MAX_LINE_SIZE)
	{
		return (XN_STATUS_INTERNAL_BUFFER_TOO_SMALL);
	}

	while (pSrc < pSrcEnd)
	{
		xnOSMemCopy(pLineBuffer, pSrc, nMemCpyLineSize);

		pDest = pDestVal;
		while (pDest != pDestEnd)
		{
			*pSrc = *(pDest-2);
			*(pSrc+1) = *(pDest-1);
			*(pSrc+2) = *pDest;

			pSrc+=3;
			pDest-=3;
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnMirrorYUV422Pixels(XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nLineSize)
{
	// Local function variables
	XnUInt8* pSrc = pBuffer;
	XnUInt8 pLineBuffer[XN_MIRROR_MAX_LINE_SIZE];
	XnUInt8* pSrcEnd = (XnUInt8*)pSrc + nBufferSize;
	XnUInt8* pDest = NULL;
	XnUInt8* pDestVal = &pLineBuffer[(nLineSize/2-1)*sizeof(XnUInt32)]; // last element
	XnUInt8* pDestEnd = &pLineBuffer[0]; // first element
	XnUInt32 nMemCpyLineSize = nLineSize/2*sizeof(XnUInt32);

	if (nMemCpyLineSize > XN_MIRROR_MAX_LINE_SIZE)
	{
		return (XN_STATUS_INTERNAL_BUFFER_TOO_SMALL);
	}

	while (pSrc < pSrcEnd)
	{
		xnOSMemCopy(pLineBuffer, pSrc, nMemCpyLineSize);
		pDest = pDestVal;

		while (pDest >= pDestEnd)
		{
			pSrc[0] = pDest[0]; // u
			pSrc[1] = pDest[3]; // y1 <-> y2
			pSrc[2] = pDest[2]; // v
			pSrc[3] = pDest[1]; // y2 <-> y1

			pSrc += 4;
			pDest -= 4;
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

typedef XnStatus (*XnMirrorFunc)(XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nXRes);

typedef struct
{
	XnMirrorFunc MirrorFunctions[XN_OUTPUT_FORMATS_COUNT];
} XnMirrorFunctions;

XnMirrorFunctions XnMirrorGetFunctionsMap()
{
	XnMirrorFunctions result;
	xnOSMemSet(&result, 0, sizeof(XnMirrorFunctions));
	result.MirrorFunctions[XN_OUTPUT_FORMAT_SHIFT_VALUES] = XnMirrorTwoBytePixels;
	result.MirrorFunctions[XN_OUTPUT_FORMAT_DEPTH_VALUES] = XnMirrorTwoBytePixels;
	result.MirrorFunctions[XN_OUTPUT_FORMAT_GRAYSCALE8] = XnMirrorOneBytePixels;
	result.MirrorFunctions[XN_OUTPUT_FORMAT_GRAYSCALE16] = XnMirrorTwoBytePixels;
	result.MirrorFunctions[XN_OUTPUT_FORMAT_YUV422] = XnMirrorYUV422Pixels;
	result.MirrorFunctions[XN_OUTPUT_FORMAT_RGB24] = XnMirrorThreeBytePixels;
	return result;
}

XN_FORMATS_API XnStatus XnFormatsMirrorPixelData(XnOutputFormats nOutputFormat, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nXRes)
{
	static XnMirrorFunctions FunctionsMap = XnMirrorGetFunctionsMap();

	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pBuffer);

	XnMirrorFunc pFunc = FunctionsMap.MirrorFunctions[nOutputFormat];
	if (pFunc == NULL)
	{
		xnLogError(XN_MASK_FORMATS, "Mirror was not implemented for output format %d", nOutputFormat);
		return XN_STATUS_ERROR;
	}

	// perform mirror
	nRetVal = pFunc(pBuffer, nBufferSize, nXRes);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}