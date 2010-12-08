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
#include "Bayer.h"
#include <math.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
XnUInt8 Gamma[256] = {
	0,      1,     2,     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
	16,    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
	32,    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
	48,    49,    50,    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
	64,    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
	80,    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
	96,    97,    98,    99,    100,   101,   102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
	112,   113,   114,   115,   116,   117,   118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
	128,   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
	144,   145,   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
	160,   161,   162,   163,   164,   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
	176,   177,   178,   179,   180,   181,   182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
	192,   193,   194,   195,   196,   197,   198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
	208,   209,   210,   211,   212,   213,   214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
	224,   225,   226,   227,   228,   229,   230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
	240,   241,   242,   243,   244,   245,   246,   247,   248,   249,   250,   251,   252,   253,   254,   255};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
void BayerUpdateGamma(float fGammaCorr)
{
	for(XnUInt32 iG = 0; iG < 256;++iG)
		Gamma[iG] = XnUInt32(255*pow(XnDouble(iG)/255.0,(XnDouble)fGammaCorr) + 0.5);
}

static inline void WriteRGB(XnUInt8 *pBuffer, XnUInt8 nRed, XnUInt8 nGreen, XnUInt8 nBlue)
{
	pBuffer[BAYER_RED] = Gamma[nRed];
	pBuffer[BAYER_GREEN] = Gamma[nGreen];
	pBuffer[BAYER_BLUE] = Gamma[nBlue];
}

void Bayer2RGB888(const XnUInt8* pBayerImage, XnUInt8* pRGBImage, XnUInt32 nXRes, XnUInt32 nYRes, XnUInt32 nDownSampleStep, XnUInt32 nBadPixels)
{
	XnUInt8 nRed;
	XnUInt8 nGreen;
	XnUInt8 nBlue;
	const XnUInt8* pBayer;
	XnUInt8* pRGB;

	//if (nBadPixels > 1)
	//{
		//nBadPixels = 1;
	//}

	XnInt32 BAYER_LINE_LENGTH = nXRes;
	XnInt32 BAYER_LINE_LENGTH2 = BAYER_LINE_LENGTH*2;
	XnInt32 BAYER_RGB_LINE_LENGTH = nXRes*BAYER_BPP;
	XnInt32 BAYER_RGB_LINE_LENGTH2 = BAYER_RGB_LINE_LENGTH*2;

	const XnUInt8* pCurrBayer;
	XnUInt8* pCurrRGB;
	XnUInt32 nColCount;
	XnUInt32 nTotalColsCount = (nXRes-2) / 2;
	XnUInt32 nTotalRowsCount = (nYRes-4) / 2;

	pBayer = pBayerImage + BAYER_LINE_LENGTH - nBadPixels;
	pRGB = pRGBImage + BAYER_RGB_LINE_LENGTH;

	do {
		pCurrBayer = pBayer+ 1;
		pCurrRGB = pRGB + BAYER_BPP;

		nColCount = nTotalColsCount;

		do {

			nRed   = ((XnUInt32)pCurrBayer[-BAYER_LINE_LENGTH]+pCurrBayer[BAYER_LINE_LENGTH]) / 2;
			nBlue  = ((XnUInt32)pCurrBayer[-1]+pCurrBayer[1]) / 2;
			WriteRGB(pCurrRGB+0, nRed, pCurrBayer[0], nBlue);

			nRed   = ((XnUInt32)pCurrBayer[-BAYER_LINE_LENGTH+2]+pCurrBayer[BAYER_LINE_LENGTH+2]) / 2;
			nGreen = ((XnUInt32)pCurrBayer[0]+pCurrBayer[2]) / 2;
			WriteRGB(pCurrRGB+BAYER_BPP, nRed, nGreen, pCurrBayer[1]);

			nGreen = ((XnUInt32)pCurrBayer[BAYER_LINE_LENGTH-1]+pCurrBayer[BAYER_LINE_LENGTH+1]) / 2;
			nBlue  = ((XnUInt32)pCurrBayer[BAYER_LINE_LENGTH2-1]+pCurrBayer[BAYER_LINE_LENGTH2+1]) / 2;
			WriteRGB(pCurrRGB+BAYER_RGB_LINE_LENGTH, pCurrBayer[BAYER_LINE_LENGTH], nGreen, nBlue);

			nRed   = ((XnUInt32)pCurrBayer[BAYER_LINE_LENGTH]+pCurrBayer[BAYER_LINE_LENGTH+2]) / 2;
			nBlue  = ((XnUInt32)pCurrBayer[1]+pCurrBayer[BAYER_LINE_LENGTH2+1]) / 2;
			WriteRGB(pCurrRGB+BAYER_RGB_LINE_LENGTH+BAYER_BPP, nRed, pCurrBayer[BAYER_LINE_LENGTH+1], nBlue);

			pCurrBayer += 2;
			pCurrRGB += 2*BAYER_BPP;
		} while (--nColCount);

		pBayer += BAYER_LINE_LENGTH2;
		pRGB += BAYER_RGB_LINE_LENGTH2;
	} while (--nTotalRowsCount);
}
