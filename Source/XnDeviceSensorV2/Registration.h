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






#ifndef _XN_REGISTRATION_H_
#define _XN_REGISTRATION_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"

#define XN_REG_PARAB_COEFF 4
#define XN_REG_X_SCALE	16

class XnSensorDepthStream; // Forward Declaration

class XnRegistration
{
public:
	XnRegistration();
	~XnRegistration() { Free(); }

	XnStatus Init(XnDevicePrivateData* pDevicePrivateData, XnSensorDepthStream* pDepthStream, XnUInt16* pDepthToShiftTable);
	XnStatus Free();
	void Apply(XnDepthPixel* pDM);

	inline XnBool IsInitialized() { return m_bInitialized; }

private:
	XnStatus BuildRegTable();
	XnStatus BuildRegTable1000();
	XnStatus BuildRegTable1080();
	void Apply1000(XnDepthPixel* pInput, XnDepthPixel* pOutput);
	void Apply1080(XnDepthPixel* pInput, XnDepthPixel* pOutput);

	XnBool m_bInitialized;

	XnDevicePrivateData* m_pDevicePrivateData;
	XnSensorDepthStream* m_pDepthStream;
	XnUInt16* m_pDepthToShiftTable;
	XnBool m_bD2SAlloc;
	XnUInt16* m_pRegistrationTable;
	XnRegistrationPaddingInformation m_padInfo;
	XnDepthPixel* m_pTempBuffer;
	XnDouble m_dShiftFactor;
	XnBool m_b1000;
};

#endif //_XN_RGBREG_H_
