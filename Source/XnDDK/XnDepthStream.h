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
#ifndef __XN_DEPTH_STREAM_H__
#define __XN_DEPTH_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnPixelStream.h>
#include <XnDDK/XnActualRealProperty.h>
#include <XnDDK/XnShiftToDepthStreamHelper.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

class XN_DDK_CPP_API XnDepthStream : public XnPixelStream
{
public:
	XnDepthStream(const XnChar* csName, XnBool bAllowCustomResolutions, XnDepthPixel nDeviceMaxDepth, XnUInt16 nDeviceMaxShift);
	~XnDepthStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	XnStatus Free();

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline XnDepthPixel GetMinDepth() const { return (XnDepthPixel)m_MinDepth.GetValue(); }
	inline XnDepthPixel GetMaxDepth() const { return (XnDepthPixel)m_MaxDepth.GetValue(); }
	inline XnUInt32 GetConstShift() const { return (XnUInt32)m_ConstShift.GetValue(); }
	inline XnUInt32 GetPixelSizeFactor() const { return (XnUInt32)m_PixelSizeFactor.GetValue(); }
	inline XnUInt16 GetMaxShift() const { return (XnUInt16)m_MaxShift.GetValue(); }
	inline XnDepthPixel GetDeviceMaxDepth() const { return (XnDepthPixel)m_DeviceMaxDepth.GetValue(); }
	inline XnUInt32 GetParamCoefficient() const { return (XnUInt32)m_ParamCoefficient.GetValue(); }
	inline XnUInt32 GetShiftScale() const { return (XnUInt32)m_ShiftScale.GetValue(); }
	inline XnDepthPixel GetZeroPlaneDistance() const { return (XnDepthPixel)m_ZeroPlaneDistance.GetValue(); }
	inline XnDouble GetZeroPlanePixelSize() const { return m_ZeroPlanePixelSize.GetValue(); }
	inline XnDouble GetEmitterDCmosDistance() const { return m_EmitterDCmosDistance.GetValue(); }
	inline XnDouble GetDCmosRCmosDistance() const { return m_GetDCmosRCmosDistance.GetValue(); }
	inline XnDepthPixel GetNoDepthValue() const { return (XnDepthPixel)m_NoDepthValue.GetValue(); }
	inline XnDepthPixel GetShadowValue() const { return (XnDepthPixel)m_ShadowValue.GetValue(); }

	inline XnDepthPixel* GetShiftToDepthTable() const { return m_S2DHelper.GetShiftToDepthTable(); }
	inline XnUInt16* GetDepthToShiftTable() const { return m_S2DHelper.GetDepthToShiftTable(); }

protected:
	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& MinDepthProperty() { return m_MinDepth; }
	inline XnActualIntProperty& MaxDepthProperty() { return m_MaxDepth; }
	inline XnActualIntProperty& ConstShiftProperty() { return m_ConstShift; }
	inline XnActualIntProperty& PixelSizeFactorProperty() { return m_PixelSizeFactor; }
	inline XnActualIntProperty& MaxShiftProperty() { return m_MaxShift; }
	inline XnActualIntProperty& DeviceMaxDepthProperty() { return m_DeviceMaxDepth; }
	inline XnActualIntProperty& ParamCoefficientProperty() { return m_ParamCoefficient; }
	inline XnActualIntProperty& ShiftScaleProperty() { return m_ShiftScale; }
	inline XnActualIntProperty& ZeroPlaneDistanceProperty() { return m_ZeroPlaneDistance; }
	inline XnActualRealProperty& ZeroPlanePixelSizeProperty() { return m_ZeroPlanePixelSize; }
	inline XnActualRealProperty& EmitterDCmosDistanceProperty() { return m_EmitterDCmosDistance; }
	inline XnActualRealProperty& GetDCmosRCmosDistanceProperty() { return m_GetDCmosRCmosDistance; }
	inline XnActualIntProperty& NoDepthValueProperty() { return m_NoDepthValue; }
	inline XnActualIntProperty& ShadowValueProperty() { return m_ShadowValue; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetMinDepth(XnDepthPixel nMinDepth);
	virtual XnStatus SetMaxDepth(XnDepthPixel nMaxDepth);

protected:
	//---------------------------------------------------------------------------
	// Helper functions
	//---------------------------------------------------------------------------
	XnStatus ValidateDepthValue(XnDepthPixel nDepth);

private:
	XnStatus OnOutputFormatChanged();

	// callbacks
	static XnStatus XN_CALLBACK_TYPE SetMinDepthCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetMaxDepthCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE OutputFormatValueChangedCallback(const XnProperty* pSender, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnActualIntProperty m_MinDepth;
	XnActualIntProperty m_MaxDepth;
	XnActualIntProperty m_ConstShift;
	XnActualIntProperty m_PixelSizeFactor;
	XnActualIntProperty m_MaxShift;
	XnActualIntProperty m_DeviceMaxDepth;
	XnActualIntProperty m_ParamCoefficient;
	XnActualIntProperty m_ShiftScale;
	XnActualIntProperty m_ZeroPlaneDistance;
	XnActualRealProperty m_ZeroPlanePixelSize;
	XnActualRealProperty m_EmitterDCmosDistance;
	XnActualRealProperty m_GetDCmosRCmosDistance;
	XnActualIntProperty m_NoDepthValue;
	XnActualIntProperty m_ShadowValue;

	XnShiftToDepthStreamHelper m_S2DHelper;
};

#endif //__XN_DEPTH_STREAM_H__