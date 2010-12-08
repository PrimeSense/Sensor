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
#include "XnUncompressedCodec.h"
#include "Xn16zCodec.h"
#include "Xn16zEmbTablesCodec.h"
#include "Xn8zCodec.h"
#include "XnJpegCodec.h"
#include "XnNiCodec.h"

XnCompressionFormats XnCodec::GetCompressionFormatFromCodecID(XnCodecID codecID)
{
	switch (codecID)
	{
	case XN_CODEC_UNCOMPRESSED:
		return XN_COMPRESSION_NONE;
	case XN_CODEC_16Z:
		return XN_COMPRESSION_16Z;
	case XN_CODEC_16Z_EMB_TABLES:
		return XN_COMPRESSION_16Z_EMB_TABLE;
	case XN_CODEC_8Z:
		return XN_COMPRESSION_COLOR_8Z;
	case XN_CODEC_JPEG:
		return XN_COMPRESSION_JPEG;
	default:
		return (XnCompressionFormats)-1;
	}
}

XnCodecID XnCodec::GetCodecIDFromCompressionFormat(XnCompressionFormats format)
{
	switch (format)
	{
	case XN_COMPRESSION_16Z:
		return XN_CODEC_16Z;
	case XN_COMPRESSION_16Z_EMB_TABLE:
		return XN_CODEC_16Z_EMB_TABLES;
	case XN_COMPRESSION_JPEG:
		return XN_CODEC_JPEG;
	case XN_COMPRESSION_NONE:
		return XN_CODEC_UNCOMPRESSED;
	case XN_COMPRESSION_COLOR_8Z:
		return XN_CODEC_8Z;
	default:
		return XN_CODEC_NULL;
	}
}
