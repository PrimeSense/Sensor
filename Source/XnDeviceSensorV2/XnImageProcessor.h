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





#ifndef __XN_IMAGE_PROCESSOR_H__
#define __XN_IMAGE_PROCESSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFrameStreamProcessor.h"
#include "XnSensorImageStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
class XnImageProcessor : public XnFrameStreamProcessor
{
public:
	XnImageProcessor(XnSensorImageStream* pStream, XnSensorStreamHelper* pHelper, XnBool bCompressedOutput = FALSE);
	virtual ~XnImageProcessor();

	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader);
	virtual void OnFrameReady(XnUInt32 nFrameID, XnUInt64 nFrameTS);

	//---------------------------------------------------------------------------
	// Helper Functions
	//---------------------------------------------------------------------------
	inline XnSensorImageStream* GetStream()
	{
		return (XnSensorImageStream*)XnFrameStreamProcessor::GetStream();
	}

	XnUInt32 GetActualXRes() { return m_nActualXRes; }
	XnUInt32 GetActualYRes() { return m_nActualYRes; }

private:
	XnUInt32 CalculateExpectedSize();
	void CalcActualRes();
	static XnStatus XN_CALLBACK_TYPE ActualResChangedCallback(const XnProperty* pSender, void* pCookie);

	XnUInt32 m_nActualXRes;
	XnUInt32 m_nActualYRes;

	XnCallbackHandle m_hXResCallback;
	XnCallbackHandle m_hYResCallback;
	XnCallbackHandle m_hXCropCallback;
	XnCallbackHandle m_hYCropCallback;
	XnCallbackHandle m_hCropEnabledCallback;

	XnBool m_bCompressedOutput;
};

#endif //__XN_IMAGE_PROCESSOR_H__
