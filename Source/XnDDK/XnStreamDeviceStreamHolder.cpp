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
#include "XnStreamDeviceStreamHolder.h"
#include <XnFormats/XnUncompressedCodec.h>
#include <XnFormats/Xn16zCodec.h>
#include <XnFormats/Xn16zEmbTablesCodec.h>
#include <XnFormats/Xn8zCodec.h>
#include <XnFormats/XnJpegCodec.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStreamDeviceStreamHolder::XnStreamDeviceStreamHolder(XnDeviceStream* pStream, XnBool bCompressionIsReadOnly) :
	XnDeviceModuleHolder(pStream, TRUE),
	m_Compression(XN_STREAM_PROPERTY_COMPRESSION, XN_COMPRESSION_NONE),
	m_pCodec(NULL)
{
	if (!bCompressionIsReadOnly)
	{
		m_Compression.UpdateSetCallbackToDefault();
	}
}

XnStatus XnStreamDeviceStreamHolder::Init(const XnActualPropertiesHash* pProps)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_ADD_PROPERTIES(GetStream(), &m_Compression);

	nRetVal = XnDeviceModuleHolder::Init(pProps);
	XN_IS_STATUS_OK(nRetVal);

	// create codec
	nRetVal = m_CodecProperties.Set(&m_Compression, &m_Compression);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Compression.OnChangeEvent().Register(CodecPropertyChangedCallback, this, NULL);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ChooseCodec();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnStreamDeviceStreamHolder::Free()
{
	XN_DELETE(m_pCodec);
	m_pCodec = NULL;

	return XnDeviceModuleHolder::Free();
}

XnStatus XnStreamDeviceStreamHolder::ChooseCodec()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// create new codec (we also need to register on all the properties)
	XnCodec* pCodec;
	XnPropertiesList CodecProps;

	switch (GetCompression())
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
			XnIntProperty* pDeviceMaxDepthProp;
			nRetVal = GetStream()->GetProperty(XN_STREAM_PROPERTY_DEVICE_MAX_DEPTH, &pDeviceMaxDepthProp);
			XN_IS_STATUS_OK(nRetVal);

			XnUInt64 nMaxDepth;
			nRetVal = pDeviceMaxDepthProp->GetValue(&nMaxDepth);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = CodecProps.AddLast(pDeviceMaxDepthProp);
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
			XnIntProperty* pOutputFormatProp;
			nRetVal = GetStream()->GetProperty(XN_STREAM_PROPERTY_OUTPUT_FORMAT, &pOutputFormatProp);
			XN_IS_STATUS_OK(nRetVal);

			XnUInt64 nOutputFormat;
			nRetVal = pOutputFormatProp->GetValue(&nOutputFormat);
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

			nRetVal = CodecProps.AddLast(pOutputFormatProp);
			XN_IS_STATUS_OK(nRetVal);

			// X res
			XnIntProperty* pXResProp;
			nRetVal = GetStream()->GetProperty(XN_STREAM_PROPERTY_X_RES, &pXResProp);
			XN_IS_STATUS_OK(nRetVal);

			XnUInt64 nXRes;
			nRetVal = pXResProp->GetValue(&nXRes);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = CodecProps.AddLast(pXResProp);
			XN_IS_STATUS_OK(nRetVal);

			// Y res
			XnIntProperty* pYResProp;
			nRetVal = GetStream()->GetProperty(XN_STREAM_PROPERTY_Y_RES, &pYResProp);
			XN_IS_STATUS_OK(nRetVal);

			XnUInt64 nYRes;
			nRetVal = pYResProp->GetValue(&nYRes);
			XN_IS_STATUS_OK(nRetVal);

			// Cropping
			XnGeneralProperty* pCroppingProp;
			nRetVal = GetStream()->GetProperty(XN_STREAM_PROPERTY_CROPPING, &pCroppingProp);
			XN_IS_STATUS_OK(nRetVal);

			XnCropping cropping;
			nRetVal = pCroppingProp->GetValue(XN_PACK_GENERAL_BUFFER(cropping));
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = CodecProps.AddLast(pCroppingProp);
			XN_IS_STATUS_OK(nRetVal);

			// calc x,y
			if (cropping.bEnabled)
			{
				nXRes = cropping.nXSize;
				nYRes = cropping.nYSize;
			}

			XN_VALIDATE_NEW_AND_INIT(pCodec, XnJpegCodec, bRGB, (XnUInt32)nXRes, (XnUInt32)nYRes);
		}
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Codec factory does not support compression type %d", GetCompression());
	}

	// register to new props
	for (XnPropertiesList::Iterator it = CodecProps.begin(); it != CodecProps.end(); ++it)
	{
		XnProperty* pProp = *it;

		XnPropertiesHash::Iterator hashIt = m_CodecProperties.end();
		nRetVal = m_CodecProperties.Find(pProp, hashIt);
		if (nRetVal == XN_STATUS_NO_MATCH)
		{
			XnCallbackHandle hCallbackDummy;
			nRetVal = pProp->OnChangeEvent().Register(CodecPropertyChangedCallback, this, &hCallbackDummy);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = m_CodecProperties.Set(pProp, NULL);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	// replace it
	XN_DELETE(m_pCodec);
	m_pCodec = pCodec;

	return (XN_STATUS_OK);
}

XnStatus XnStreamDeviceStreamHolder::CodecPropertyChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnStreamDeviceStreamHolder* pThis = (XnStreamDeviceStreamHolder*)pCookie;
	return pThis->ChooseCodec();
}
