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





#ifndef __XN_CMOS_INFO_H__
#define __XN_CMOS_INFO_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnSensorFirmware.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct
{
	XnCmosBlankingInformation BlankingInfo;
	XnResolutions nRes;
	XnUInt32 nFPS;
} XnCmosBlankingData;

class XnCmosInfo
{
public:
	XnCmosInfo(XnSensorFirmware* pFirmware, XnDevicePrivateData* pDevicePrivateData);
	~XnCmosInfo();

	XnStatus SetCmosConfig(XnCMOSType nCmos, XnResolutions nResolution, XnUInt32 nFPS);

	inline const XnCmosBlankingCoefficients* GetBlankingCoefficients(XnCMOSType nCmos) const { return m_pCurrCmosBlankingInfo[nCmos]; } 

private:
	XN_DECLARE_LIST(XnCmosBlankingData, XnCmosBlankingDataList)

	XnSensorFirmware* m_pFirmware;
	XnDevicePrivateData* m_pDevicePrivateData;

	XnCmosBlankingDataList m_CmosBlankingInfo;
	XnCmosBlankingCoefficients* m_pCurrCmosBlankingInfo[XN_CMOS_COUNT];
};

#endif //__XN_CMOS_INFO_H__
