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
#include "XnBackwardsCompatibility36.h"
#include <XnFormatsStatus.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnBCDepthFormatToOutputFormat(XnStreamDepthFormat nDepthFormat, XnOutputFormats* pnOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nDepthFormat)
	{
	case XN_DEPTH_FORMAT_RAW10:
	case XN_DEPTH_FORMAT_RAW12:
		*pnOutputFormat = XN_OUTPUT_FORMAT_DEPTH_VALUES;
		break;
	case XN_DEPTH_FORMAT_SHIFTS:
		*pnOutputFormat = XN_OUTPUT_FORMAT_SHIFT_VALUES;
		break;
	default:
		XN_LOG_ERROR_RETURN(XN_STATUS_IO_INVALID_STREAM_DEPTH_FORMAT, XN_MASK_DDK, "Failed to translate depth format %d to output format!", nDepthFormat);
	}

	return (XN_STATUS_OK);
}

XnStatus XnBCOutputFormatToDepthFormat(XnOutputFormats nOutputFormat, XnStreamDepthFormat* pnDepthFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nOutputFormat)
	{
	case XN_OUTPUT_FORMAT_DEPTH_VALUES:
		*pnDepthFormat = XN_DEPTH_FORMAT_RAW12;
		break;
	case XN_OUTPUT_FORMAT_SHIFT_VALUES:
		*pnDepthFormat = XN_DEPTH_FORMAT_SHIFTS;
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_INVALID_STREAM_DEPTH_FORMAT, XN_MASK_DDK, "Failed to translate output format %d to depth format!", nOutputFormat);
	}

	return(XN_STATUS_OK);
}

XnStatus XnBCImageFormatToOutputFormat(XnStreamImageFormat nImageFormat, XnOutputFormats* pnOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nImageFormat)
	{
	case XN_IMAGE_FORMAT_GRAYSCALE8:
		*pnOutputFormat = XN_OUTPUT_FORMAT_GRAYSCALE8;
		break;
	case XN_IMAGE_FORMAT_RGB24:
		*pnOutputFormat = XN_OUTPUT_FORMAT_RGB24;
		break;
	case XN_IMAGE_FORMAT_YUV422:
		*pnOutputFormat = XN_OUTPUT_FORMAT_YUV422;
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT, XN_MASK_DDK, "Failed to translate image format %d to output format!", nImageFormat);
	}

	return(XN_STATUS_OK);
}

XnStatus XnBCOutputFormatToImageFormat(XnOutputFormats nOutputFormat, XnStreamImageFormat* pnImageFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nOutputFormat)
	{
	case XN_OUTPUT_FORMAT_GRAYSCALE8:
		*pnImageFormat = XN_IMAGE_FORMAT_GRAYSCALE8;
		break;
	case XN_OUTPUT_FORMAT_RGB24:
		*pnImageFormat = XN_IMAGE_FORMAT_RGB24;
		break;
	case XN_OUTPUT_FORMAT_YUV422:
		*pnImageFormat = XN_IMAGE_FORMAT_YUV422;
		break;
	case XN_OUTPUT_FORMAT_GRAYSCALE16:
		*pnImageFormat = (XnStreamImageFormat)-1;
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT, XN_MASK_DDK, "Failed to translate output format %d to image format!", nOutputFormat);
	}

	return (XN_STATUS_OK);
}

XnStatus XnBCAudioFormatToOutputFormat(XnStreamAudioFormat nAudioFormat, XnOutputFormats* pnOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nAudioFormat)
	{
	case XN_AUDIO_FORMAT_PCM:
		*pnOutputFormat = XN_OUTPUT_FORMAT_PCM;
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT, XN_MASK_DDK, "Failed to translate audio format %d to output format!", nAudioFormat);
	}

	return (XN_STATUS_OK);
}

XnStatus XnBCOutputFormatToAudioFormat(XnOutputFormats nOutputFormat, XnStreamAudioFormat* pnAudioFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nOutputFormat)
	{
	case XN_OUTPUT_FORMAT_PCM:
		*pnAudioFormat = XN_AUDIO_FORMAT_PCM;
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT, XN_MASK_DDK, "Failed to translate audio format %d to output format!", nOutputFormat);
	}

	return (XN_STATUS_OK);
}

XnStreamDepthFormat XnBCGetStreamDepthFormatFromString(const XnChar* cpDepthFormat)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	if (cpDepthFormat == NULL)
	{
		return ((XnStreamDepthFormat)-1);
	}

	// Convert each string into the proper depth format enum
	if (strcmp(cpDepthFormat, "Raw12") == 0)
	{
		return (XN_DEPTH_FORMAT_RAW12);
	}
	else if (strcmp(cpDepthFormat, "Raw10") == 0)
	{
		return (XN_DEPTH_FORMAT_RAW10);
	}
	else if (strcmp(cpDepthFormat, "Shifts") == 0)
	{
		return (XN_DEPTH_FORMAT_SHIFTS);
	}
	else if (strcmp(cpDepthFormat, "Disabled") == 0)
	{
		return (XN_DEPTH_FORMAT_DISABLED);
	}

	// Unknown mode...
	return ((XnStreamDepthFormat)-1);
}

XnStreamImageFormat XnBCGetStreamImageFormatFromString(const XnChar* cpImageFormat)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	if (cpImageFormat == NULL)
	{
		return ((XnStreamImageFormat)-1);
	}

	// Convert each string into the proper image format enum
	if (strcmp(cpImageFormat, "RGB24") == 0)
	{
		return (XN_IMAGE_FORMAT_RGB24);
	}
	else if (strcmp(cpImageFormat, "Gray8") == 0)
	{
		return (XN_IMAGE_FORMAT_GRAYSCALE8);
	}
	else if (strcmp(cpImageFormat, "YUV422") == 0)
	{
		return (XN_IMAGE_FORMAT_YUV422);
	}
	else if (strcmp(cpImageFormat, "Disabled") == 0)
	{
		return (XN_IMAGE_FORMAT_DISABLED);
	}

	// Unknown mode...
	return ((XnStreamImageFormat)-1);
}

XnStreamAudioFormat XnBCGetStreamAudioFormatFromString(const XnChar* cpAudioFormat)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	if (cpAudioFormat == NULL)
	{
		return ((XnStreamAudioFormat)-1);
	}

	// Convert each string into the proper image format enum
	if (strcmp(cpAudioFormat, "PCM") == 0)
	{
		return (XN_AUDIO_FORMAT_PCM);
	}
	else if (strcmp(cpAudioFormat, "Disabled") == 0)
	{
		return (XN_AUDIO_FORMAT_DISABLED);
	}

	// Unknown mode...
	return ((XnStreamAudioFormat)-1);
}

XN_STREAM_FLAGS_TYPE XnBCGetStreamFlagsFromString(XnChar* cpStreamFlags)
{
	// Local function variables
	XnChar* cpToken = NULL;
	XN_STREAM_FLAGS_TYPE StreamFlags = 0;

	cpToken = strtok (cpStreamFlags, ";");
	while (cpToken != NULL)
	{
		if (strcmp(cpToken, "Mirror") == 0)
		{
			StreamFlags |= XN_STREAM_FLAG_MIRROR;
		}
		else
		{
			return ((XN_STREAM_FLAGS_TYPE)-1);
		}

		cpToken = strtok (NULL, ";");
	}

	// All is good...
	return (StreamFlags);
}
