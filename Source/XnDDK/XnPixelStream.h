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
#ifndef __XN_PIXEL_STREAM_H__
#define __XN_PIXEL_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnFrameStream.h>
#include <XnDDK/XnActualGeneralProperty.h>
#include <XnArray.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** Represents a stream that is pixel based (meaning, its output data is a matrix of data). */
class XN_DDK_CPP_API XnPixelStream : public XnFrameStream
{
public:
	XnPixelStream(const XnChar* csType, const XnChar* csName, XnBool bAllowCustomResolutions);
	~XnPixelStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline XnResolutions GetResolution() const { return (XnResolutions)m_Resolution.GetValue(); }
	inline XnUInt32 GetXRes() const { return (XnUInt32)m_XRes.GetValue(); }
	inline XnUInt32 GetYRes() const { return (XnUInt32)m_YRes.GetValue(); }
	inline XnUInt32 GetBytesPerPixel() const { return (XnUInt32)m_BytesPerPixel.GetValue(); }
	inline const XnCropping* GetCropping() const { return (XnCropping*)m_Cropping.GetValue().pData; }

protected:
	XnStatus AddSupportedModes(XnCmosPreset* aPresets, XnUInt32 nCount);

	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& IsPixelStreamProperty() { return m_IsPixelStream; }
	inline XnActualIntProperty& ResolutionProperty() { return m_Resolution; }
	inline XnActualIntProperty& XResProperty() { return m_XRes; }
	inline XnActualIntProperty& YResProperty() { return m_YRes; }
	inline XnActualIntProperty& BytesPerPixelProperty() { return m_BytesPerPixel; }
	inline XnActualGeneralProperty& CroppingProperty() { return m_Cropping; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetResolution(XnResolutions nResolution);
	virtual XnStatus SetXRes(XnUInt32 nXRes);
	virtual XnStatus SetYRes(XnUInt32 nYRes);
	virtual XnStatus SetCropping(const XnCropping* pCropping);

	//---------------------------------------------------------------------------
	// Virtual Methods
	//---------------------------------------------------------------------------

	/** Crops the stream output. */
	virtual XnStatus CropImpl(XnStreamData* pStreamOutput, const XnCropping* pCropping);

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus ReadImpl(XnStreamData* pStreamOutput);
	XnStatus Mirror(XnStreamData* pStreamOutput) const;
	XnStatus CalcRequiredSize(XnUInt32* pnRequiredSize) const;

	XnStatus ValidateCropping(const XnCropping* pCropping);

private:
	class XN_DDK_CPP_API XnResolutionProperty : public XnActualIntProperty
	{
	public:
		XnResolutionProperty(const XnChar* strName, XnUInt64 nInitialValue = 0, const XnChar* strModule = "");
		XnBool ConvertValueToString(XnChar* csValue, const void* pValue) const;
	};

	XnStatus OnResolutionChanged();
	XnStatus OnOutputFormatChanged();
	XnStatus FixCropping();
	XnStatus GetSupportedModes(XnCmosPreset* aPresets, XnUInt32& nCount);

	static XnStatus XN_CALLBACK_TYPE SetResolutionCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetXResCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetYResCallback(XnActualIntProperty* pSenser, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCroppingCallback(XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ResolutionValueChangedCallback(const XnProperty* pSenser, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE OutputFormatValueChangedCallback(const XnProperty* pSenser, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE FixCroppingCallback(const XnProperty* pSenser, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadCroppingFromFileCallback(XnGeneralProperty* pSender, const XnChar* csINIFile, const XnChar* csSection);
	static XnStatus XN_CALLBACK_TYPE GetSupportedModesCallback(const XnGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnActualIntProperty m_IsPixelStream;
	XnResolutionProperty m_Resolution;
	XnActualIntProperty m_XRes;
	XnActualIntProperty m_YRes;
	XnActualIntProperty m_BytesPerPixel;
	XnActualGeneralProperty m_Cropping;

	XnCropping m_CroppingData;

	XnActualIntProperty m_SupportedModesCount;
	XnGeneralProperty m_SupportedModes;

	XnArray<XnCmosPreset> m_supportedModesData;
	XnBool m_bAllowCustomResolutions;
};

#endif //__XN_PIXEL_STREAM_H__