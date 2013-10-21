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
#ifndef __XN_SHIFT_TO_DEPTH_STREAM_HELPER_H__
#define __XN_SHIFT_TO_DEPTH_STREAM_HELPER_H__

#include <XnDDK/XnDeviceModule.h>
#include <XnDDK/XnShiftToDepth.h>

class XN_DDK_CPP_API XnShiftToDepthStreamHelper
{
public:
	XnShiftToDepthStreamHelper();
	virtual ~XnShiftToDepthStreamHelper();

	XnStatus Init(XnDeviceModule* pModule);
	XnStatus Free();

	inline XnDepthPixel* GetShiftToDepthTable() const { return m_ShiftToDepthTables.pShiftToDepthTable; }
	inline XnUInt16* GetDepthToShiftTable() const { return m_ShiftToDepthTables.pDepthToShiftTable; }

protected:
	inline XnActualGeneralProperty& ShiftToDepthTableProperty() { return m_ShiftToDepthTable; }
	inline XnActualGeneralProperty& DepthToShiftTableProperty() { return m_DepthToShiftTable; }

private:
	XnStatus RaiseChangeEvents();
	XnStatus InitShiftToDepth();
	XnStatus OnShiftToDepthPropertyValueChanged();
	XnStatus OnDeviceS2DTablesSizeChanged();
	XnStatus GetShiftToDepthConfig(XnShiftToDepthConfig& Config);
	XnStatus GetShiftToDepthTableImpl(const XnGeneralBuffer& gbValue) const;
	XnStatus GetDepthToShiftTableImpl(const XnGeneralBuffer& gbValue) const;

	// callbacks
	static XnStatus XN_CALLBACK_TYPE GetShiftToDepthTableCallback(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetDepthToShiftTableCallback(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ShiftToDepthPropertyValueChangedCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE DeviceS2DTablesSizeChangedCallback(const XnProperty* pSender, void* pCookie);

	XnActualGeneralProperty m_ShiftToDepthTable;
	XnActualGeneralProperty m_DepthToShiftTable;
	XnShiftToDepthTables m_ShiftToDepthTables;
	XnDeviceModule* m_pModule;
	XnBool m_bPropertiesAdded;
};

#endif //__XN_SHIFT_TO_DEPTH_STREAM_HELPER_H__
