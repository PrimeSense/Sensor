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




#ifndef __XN_SHIFT_TO_DEPTH_STREAM_HELPER_H__
#define __XN_SHIFT_TO_DEPTH_STREAM_HELPER_H__

#include <XnDDK/XnDeviceModule.h>
#include <XnDDK/XnShiftToDepth.h>

class XN_DDK_CPP_API XnShiftToDepthStreamHelper
{
public:
	XnShiftToDepthStreamHelper(XnDeviceModule* pModule);
	virtual ~XnShiftToDepthStreamHelper();

	XnStatus Init();
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
