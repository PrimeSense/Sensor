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





#ifndef __XN_SENSOR_STREAM_HELPER_H__
#define __XN_SENSOR_STREAM_HELPER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "IXnSensorStream.h"
#include "XnSensorFirmware.h"
#include "XnSensorFixedParams.h"
#include <XnDDK/XnDeviceStream.h>
#include <XnDDK/XnDeviceModuleHolder.h>
#include "XnSharedMemoryBufferPool.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnSensorStreamHelper
{
public:
	XnSensorStreamHelper(XnSensorObjects* pObjects);
	~XnSensorStreamHelper();

	XnStatus Init(IXnSensorStream* pSensorStream, XnDeviceStream* pStream);
	XnStatus Free();

	XnStatus Configure();
	XnStatus FinalOpen();
	XnStatus Open();
	XnStatus Close();

	/**
	* Registers a property which affects the data processor. When any of these properties
	* changes, the data processor will be recreated.
	*/
	XnStatus RegisterDataProcessorProperty(XnActualIntProperty& Property);

	typedef XnStatus (*ConvertCallback)(XnUInt64 nSource, XnUInt64* pnDest);

	/** 
	* Maps a stream property to a firmware property. Later on, such a property can be used
	* in calls to ConfigureFirmware or SetStreamFirmwareParam.
	*/
	XnStatus MapFirmwareProperty(XnActualIntProperty& Property, XnActualIntProperty& FirmwareProperty, XnBool bAllowChangeWhileOpen, ConvertCallback pStreamToFirmwareFunc = 0);

	/**
	* Configures the firmware according to the property. This can only be done for properties
	* which were previously attached via the MapFirmwareProperty function.
	*/
	XnStatus ConfigureFirmware(XnActualIntProperty& Property);

	XnStatus BeforeSettingFirmwareParam(XnActualIntProperty& Property, XnUInt16 nValue);
	XnStatus AfterSettingFirmwareParam(XnActualIntProperty& Property);

	XnStatus BeforeSettingDataProcessorProperty();
	XnStatus AfterSettingDataProcessorProperty();

	XnStatus SimpleSetFirmwareParam(XnActualIntProperty& Property, XnUInt16 nValue);

	XnStatus UpdateFromFirmware(XnActualIntProperty& Property);

	inline XnSensorFirmware* GetFirmware() const { return m_pObjects->pFirmware; }
	inline XnFWVer GetFirmwareVersion() const { return GetFirmware()->GetInfo()->nFWVer; }
	inline XnSensorFixedParams* GetFixedParams() const { return m_pObjects->pFixedParams; }
	inline XnDevicePrivateData* GetPrivateData() const { return m_pObjects->pDevicePrivateData; }
	inline XnSensorFPS* GetFPS() const { return m_pObjects->pFPS; }
	inline XnCmosInfo* GetCmosInfo() const { return m_pObjects->pCmosInfo; }
	inline IXnSensorStream* GetSensorStream() { return m_pSensorStream; }

	inline XnStatus StartFirmwareTransaction() { return GetFirmware()->GetParams()->StartTransaction(); }
	inline XnStatus CommitFirmwareTransaction() { return GetFirmware()->GetParams()->CommitTransaction(); }
	inline XnStatus CommitFirmwareTransactionAsBatch() { return GetFirmware()->GetParams()->CommitTransactionAsBatch(); }
	inline XnStatus RollbackFirmwareTransaction() { return GetFirmware()->GetParams()->RollbackTransaction(); }

	XnStatus BatchConfig(const XnActualPropertiesHash& props);

private:
	IXnSensorStream* m_pSensorStream;
	XnDeviceStream* m_pStream;
	XnSensorObjects* m_pObjects;

	XnHash m_FirmwareProperties;
};

class XnSensorStreamHolder : public XnDeviceModuleHolder
{
public:
	XnSensorStreamHolder(XnDeviceStream* pStream, XnSensorStreamHelper* pHelper) : 
		XnDeviceModuleHolder(pStream), m_pHelper(pHelper) 
	{}

	inline XnDeviceStream* GetStream() { return (XnDeviceStream*)GetModule(); }
	inline XnSharedMemoryBufferPool* GetSharedBufferPool() { return m_pHelper->GetSensorStream()->GetSharedMemoryBuffer(); }

	XnStatus Configure() { return m_pHelper->Configure(); }
	XnStatus FinalOpen() { return m_pHelper->FinalOpen(); }

private:
	XnSensorStreamHelper* m_pHelper;
};

#endif //__XN_SENSOR_STREAM_HELPER_H__
