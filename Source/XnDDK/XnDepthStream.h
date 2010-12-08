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
	XnActualIntProperty m_NoDepthValue;
	XnActualIntProperty m_ShadowValue;

	XnShiftToDepthStreamHelper m_S2DHelper;
};

#endif //__XN_DEPTH_STREAM_H__