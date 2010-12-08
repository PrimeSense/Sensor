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
#include "XnCodecFactory.h"
#include "XnIntProperty.h"
#include <XnFormats/XnUncompressedCodec.h>
#include <XnFormats/Xn16zCodec.h>
#include <XnFormats/Xn16zEmbTablesCodec.h>
#include <XnFormats/Xn8zCodec.h>
#include <XnFormats/XnJpegCodec.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnCodecFactory::Create(XnCompressionFormats nFormat, XnDeviceModule* pStream, const XnChar* StreamName, XnCodec** ppCodec)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnCodec* pCodec = NULL;

	switch (nFormat)
	{
	case XN_COMPRESSION_NONE:
		{
			XN_VALIDATE_NEW_AND_INIT(pCodec, XnUncompressedCodec);
		}
		break;
	case XN_COMPRESSION_16Z:
		{
			XN_VALIDATE_NEW_AND_INIT(pCodec, Xn16zCodec);
		}
		break;
	case XN_COMPRESSION_16Z_EMB_TABLE:
		{
			// first we need to find max depth
			XnUInt64 nMaxDepth;

			nRetVal = pStream->GetProperty(XN_STREAM_PROPERTY_MAX_DEPTH, &nMaxDepth);
			XN_IS_STATUS_OK(nRetVal);

			XN_VALIDATE_NEW_AND_INIT(pCodec, Xn16zEmbTablesCodec, (XnDepthPixel)nMaxDepth);
		}
		break;
	case XN_COMPRESSION_COLOR_8Z:
		{
			XN_VALIDATE_NEW_AND_INIT(pCodec, Xn8zCodec);
		}
		break;
	case XN_COMPRESSION_JPEG:
		{
			// check what is the output format
			XnUInt64 nOutputFormat;
			nRetVal = pStream->GetProperty(XN_STREAM_PROPERTY_OUTPUT_FORMAT, &nOutputFormat);
			XN_IS_STATUS_OK(nRetVal);

			XnBool bRGB = FALSE;

			switch (nOutputFormat)
			{
			case XN_OUTPUT_FORMAT_GRAYSCALE8:
				bRGB = FALSE;
				break;
			case XN_OUTPUT_FORMAT_RGB24:
				bRGB = TRUE;
				break;
			default:
				XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Codec factory currently supports JPEG codec only for streams of type Gray8 or RGB24!");
			}

			// take X and Y res
			XnUInt64 nXRes, nYRes;
			nRetVal = pStream->GetProperty(XN_STREAM_PROPERTY_X_RES, &nXRes);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = pStream->GetProperty(XN_STREAM_PROPERTY_Y_RES, &nYRes);
			XN_IS_STATUS_OK(nRetVal);

			XN_VALIDATE_NEW_AND_INIT(pCodec, XnJpegCodec, bRGB, (XnUInt32)nXRes, (XnUInt32)nYRes);
		}
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Codec factory does not support compression type %d", nFormat);
	}

	*ppCodec = pCodec;
	return (XN_STATUS_OK);
}

void XnCodecFactory::Destroy(XnCodec* pCodec)
{
	XN_DELETE(pCodec);
}

