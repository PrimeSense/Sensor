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
#include "Uncomp.h"
#include <XnProfiling.h>
#include <XnFormatsStatus.h>

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define XN_CHECK_UNC_IMAGE_OUTPUT(x, y)					\
		if (x > y)										\
		{												\
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);	\
		}

#define XN_IMAGE_OUTPUT(pOutput, pOutputEnd, nValue)	\
	XN_CHECK_UNC_IMAGE_OUTPUT(pOutput, pOutputEnd)		\
	*pOutput = nValue;									\
	++pOutput;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnStreamUncompressYUVImagePS(const XnUInt8* pInput, const XnUInt32 nInputSize,
										  XnUInt8* pOutput, XnUInt32* pnOutputSize, XnUInt16 nLineSize,
										  XnUInt32* pnActualRead, XnBool bLastPart)
{
	// Input is made of 4-bit elements.
	const XnUInt8* pInputOrig = pInput;
	const XnUInt8* pInputEnd = pInput + nInputSize;
	XnUInt8* pOrigOutput = pOutput;
	XnUInt8* pOutputEnd = pOutput + (*pnOutputSize);
	XnUInt8 nLastFullValue[4] = {0};	

	// NOTE: we use variables of type uint32 instead of uint8 as an optimization (better CPU usage)
	XnUInt32 nTempValue = 0;	
	XnUInt32 cInput = 0;
	XnBool bReadByte = TRUE;

	if (nInputSize < sizeof(XnUInt8))
	{
		printf("Buffer too small!\n");
		return (XN_STATUS_IO_COMPRESSED_BUFFER_TOO_SMALL);
	}

	const XnUInt8* pInputLastPossibleStop = pInputOrig;
	XnUInt8* pOutputLastPossibleStop = pOrigOutput;

	*pnActualRead = 0;
	*pnOutputSize = 0;

	XnUInt32 nChannel = 0;
	XnUInt32 nCurLineSize = 0;

	while (pInput < pInputEnd)
	{
		cInput = *pInput;

		if (bReadByte)
		{
			bReadByte = FALSE;

			if (cInput < 0xd0) // 0x0 to 0xc are diffs
			{
				// take high_element only
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += (cInput >> 4) - 6;
			}
			else if (cInput < 0xe0) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not used, so this must be 0xf - full
			{
				// take two more elements
				nTempValue = (cInput & 0x0f) << 4;

				if (++pInput == pInputEnd)
					break;

				nTempValue += (*pInput >> 4);
				nLastFullValue[nChannel] = nTempValue;
			}
		}
		else
		{
			// take low-element
			cInput &= 0x0f;
			bReadByte = TRUE;
			pInput++;

			if (cInput < 0xd) // 0x0 to 0xc are diffs
			{
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += cInput - 6;
			}
			else if (cInput < 0xe) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not in use, so this must be 0xf - full
			{
				if (pInput == pInputEnd)
					break;

				// take two more elements
				nLastFullValue[nChannel] = *pInput;
				pInput++;
			}
		}

		// write output
		if (pOutput > pOutputEnd)
		{
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		}

		*pOutput = nLastFullValue[nChannel];
		pOutput++;

		nChannel++;
		switch (nChannel)
		{
		case 2:
			nLastFullValue[3] = nLastFullValue[1];
			break;
		case 4:
			nLastFullValue[1] = nLastFullValue[3];
			nChannel = 0;
			break;
		}

		nCurLineSize++;
		if (nCurLineSize == nLineSize)
		{
			pInputLastPossibleStop = pInput;
			pOutputLastPossibleStop = pOutput;

			nLastFullValue[0] = nLastFullValue[1] = nLastFullValue[2] = nLastFullValue[3] = 0;
			nCurLineSize = 0;
		}
	}

	if (bLastPart == TRUE)
	{
		*pnOutputSize = (pOutput - pOrigOutput) * sizeof(XnUInt8);
		*pnActualRead += (pInput - pInputOrig) * sizeof(XnUInt8);
	}
	else if ((pOutputLastPossibleStop != pOrigOutput) && (pInputLastPossibleStop != pInputOrig))
	{
		*pnOutputSize = (pOutputLastPossibleStop - pOrigOutput) * sizeof(XnUInt8);
		*pnActualRead += (pInputLastPossibleStop - pInputOrig) * sizeof(XnUInt8);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressImageNew(const XnUInt8* pInput, const XnUInt32 nInputSize,
									XnUInt8* pOutput, XnUInt32* pnOutputSize, XnUInt16 nLineSize,
									XnUInt32* pnActualRead, XnBool bLastPart)
{
	// Input is made of 4-bit elements.
	const XnUInt8* pInputOrig = pInput;
	const XnUInt8* pInputEnd = pInput + nInputSize;
	XnUInt8* pOrigOutput = pOutput;
	XnUInt8* pOutputEnd = pOutput + (*pnOutputSize);
	XnUInt8 nLastFullValue[4] = {0};	

	// NOTE: we use variables of type uint32 instead of uint8 as an optimization (better CPU usage)
	XnUInt32 nTempValue = 0;	
	XnUInt32 cInput = 0;
	XnBool bReadByte = TRUE;

	if (nInputSize < sizeof(XnUInt8))
	{
		printf("Buffer too small!\n");
		return (XN_STATUS_IO_COMPRESSED_BUFFER_TOO_SMALL);
	}

	const XnUInt8* pInputLastPossibleStop = pInputOrig;
	XnUInt8* pOutputLastPossibleStop = pOrigOutput;

	*pnActualRead = 0;
	*pnOutputSize = 0;

	XnUInt32 nChannel = 0;
	XnUInt32 nCurLineSize = 0;

	while (pInput < pInputEnd)
	{
		cInput = *pInput;

		if (bReadByte)
		{
			bReadByte = FALSE;

			if (cInput < 0xd0) // 0x0 to 0xc are diffs
			{
				// take high_element only
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += (cInput >> 4) - 6;
			}
			else if (cInput < 0xe0) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not used, so this must be 0xf - full
			{
				// take two more elements
				nTempValue = (cInput & 0x0f) << 4;

				if (++pInput == pInputEnd)
					break;

				nTempValue += (*pInput >> 4);
				nLastFullValue[nChannel] = nTempValue;
			}
		}
		else
		{
			// take low-element
			cInput &= 0x0f;
			bReadByte = TRUE;
			pInput++;

			if (cInput < 0xd) // 0x0 to 0xc are diffs
			{
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += cInput - 6;
			}
			else if (cInput < 0xe) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not in use, so this must be 0xf - full
			{
				if (pInput == pInputEnd)
					break;

				// take two more elements
				nLastFullValue[nChannel] = *pInput;
				pInput++;
			}
		}

		// write output
		if (pOutput > pOutputEnd)
		{
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		}

		*pOutput = nLastFullValue[nChannel];
		pOutput++;

		nChannel++;
		switch (nChannel)
		{
		case 2:
			nChannel = 0;
			break;
		}

		nCurLineSize++;
		if (nCurLineSize == nLineSize)
		{
			pInputLastPossibleStop = pInput;
			pOutputLastPossibleStop = pOutput;

			nLastFullValue[0] = nLastFullValue[1] = nLastFullValue[2] = nLastFullValue[3] = 0;
			nCurLineSize = 0;
		}
	}

	if (bLastPart == TRUE)
	{
		*pnOutputSize = (pOutput - pOrigOutput) * sizeof(XnUInt8);
		*pnActualRead += (pInput - pInputOrig) * sizeof(XnUInt8);
	}
	else if ((pOutputLastPossibleStop != pOrigOutput) && (pInputLastPossibleStop != pInputOrig))
	{
		*pnOutputSize = (pOutputLastPossibleStop - pOrigOutput) * sizeof(XnUInt8);
		*pnActualRead += (pInputLastPossibleStop - pInputOrig) * sizeof(XnUInt8);
	}

	// All is good...
	return (XN_STATUS_OK);
}
