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
#include "XnCmosInfo.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnCmosInfo::XnCmosInfo(XnSensorFirmware* pFirmware, XnDevicePrivateData* pDevicePrivateData) :
	m_pFirmware(pFirmware),
	m_pDevicePrivateData(pDevicePrivateData)
{}

XnCmosInfo::~XnCmosInfo()
{
}

XnStatus XnCmosInfo::SetCmosConfig(XnCMOSType nCmos, XnResolutions nResolution, XnUInt32 nFPS)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_pFirmware->GetInfo()->nFWVer >= XN_SENSOR_FW_VER_5_1)
	{
		// take blanking info
		XnCmosBlankingInformation* pInfo = NULL;

		// search the list if we already have this info
		for (XnCmosBlankingDataList::Iterator it = m_CmosBlankingInfo.begin(); it != m_CmosBlankingInfo.end(); ++it)
		{
			XnCmosBlankingData& data = *it;
			if (data.nRes == nResolution && data.nFPS == nFPS)
			{
				pInfo = &data.BlankingInfo;
				break;
			}
		}

		if (pInfo == NULL)
		{
			// not found in list. fetch it from FW
			XnCmosBlankingData data;
			data.nRes = nResolution;
			data.nFPS = nFPS;

			nRetVal = XnHostProtocolAlgorithmParams(m_pDevicePrivateData, XN_HOST_PROTOCOL_ALGORITHM_BLANKING, &data.BlankingInfo, sizeof(XnCmosBlankingInformation), nResolution, nFPS);
			XN_IS_STATUS_OK(nRetVal);

			// add to list
			nRetVal = m_CmosBlankingInfo.AddFirst(data);
			XN_IS_STATUS_OK(nRetVal);

			// take its info (take a pointer to the object in the list, and not to the one on the stack)
			pInfo = &m_CmosBlankingInfo.begin()->BlankingInfo;
		}

		m_pCurrCmosBlankingInfo[nCmos] = &pInfo->Coefficients[nCmos];
	}

	return (XN_STATUS_OK);
}
