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





#ifndef __XN_SENSOR_FIRMWARE_PARAMS_H__
#define __XN_SENSOR_FIRMWARE_PARAMS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnActualIntProperty.h>
#include "XnParams.h"
#include <XnHash.h>
#include "XnFirmwareInfo.h"
#include "XnFirmwareCommands.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* Holds all firmware params
*/
class XnSensorFirmwareParams
{
public:
	XnSensorFirmwareParams(XnFirmwareInfo* pInfo, XnFirmwareCommands* pCommands);
	~XnSensorFirmwareParams();

	//---------------------------------------------------------------------------
	// Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	void Free();
	XnStatus UpdateAllProperties();
	XnStatus StartTransaction();
	XnStatus CommitTransaction();
	XnStatus CommitTransactionAsBatch();
	XnStatus RollbackTransaction();

	XnActualIntProperty m_FrameSyncEnabled;
	XnActualIntProperty m_RegistrationEnabled;
	XnActualIntProperty m_Stream0Mode;
	XnActualIntProperty m_Stream1Mode;
	XnActualIntProperty m_Stream2Mode;
	XnActualIntProperty m_AudioStereo;
	XnActualIntProperty m_AudioSampleRate;
	XnActualIntProperty m_AudioLeftChannelGain;
	XnActualIntProperty m_AudioRightChannelGain;
	XnActualIntProperty m_ImageFormat;
	XnActualIntProperty m_ImageResolution;
	XnActualIntProperty m_ImageFPS;
	XnActualIntProperty m_ImageQuality;
	XnActualIntProperty m_ImageFlickerDetection;
	XnActualIntProperty m_ImageCropSizeX;
	XnActualIntProperty m_ImageCropSizeY;
	XnActualIntProperty m_ImageCropOffsetX;
	XnActualIntProperty m_ImageCropOffsetY;
	XnActualIntProperty m_ImageCropEnabled;
	XnActualIntProperty m_DepthFormat;
	XnActualIntProperty m_DepthResolution;
	XnActualIntProperty m_DepthFPS;
	XnActualIntProperty m_DepthGain;
	XnActualIntProperty m_DepthHoleFilter;
	XnActualIntProperty m_DepthMirror;
	XnActualIntProperty m_DepthDecimation;
	XnActualIntProperty m_DepthCropSizeX;
	XnActualIntProperty m_DepthCropSizeY;
	XnActualIntProperty m_DepthCropOffsetX;
	XnActualIntProperty m_DepthCropOffsetY;
	XnActualIntProperty m_DepthCropEnabled;
	XnActualIntProperty m_IRFormat;
	XnActualIntProperty m_IRResolution;
	XnActualIntProperty m_IRFPS;
	XnActualIntProperty m_IRCropSizeX;
	XnActualIntProperty m_IRCropSizeY;
	XnActualIntProperty m_IRCropOffsetX;
	XnActualIntProperty m_IRCropOffsetY;
	XnActualIntProperty m_IRCropEnabled;
	XnActualIntProperty m_ImageMirror;
	XnActualIntProperty m_IRMirror;
	XnActualIntProperty m_ReferenceResolution;
	XnActualIntProperty m_DepthWhiteBalance;
	XnActualIntProperty m_GMCMode;
	XnActualIntProperty m_ImageSharpness;
	XnActualIntProperty m_ImageAutoWhiteBalance;
	XnActualIntProperty m_ImageColorTemperature;
	XnActualIntProperty m_ImageBacklightCompensation;
	XnActualIntProperty m_ImageAutoExposure;
	XnActualIntProperty m_ImageExposureBar;
	XnActualIntProperty m_ImageLowLightCompensation;
	XnActualIntProperty m_ImageGain;

private:
	typedef struct XnFirmwareParam
	{
		XnActualIntProperty* pProperty;
		XnUInt16 nFirmwareParam;
		XnFWVer MinVer;
		XnFWVer MaxVer;
		XnUInt16 nValueIfNotSupported;
	} XnFirmwareParam;

	XN_DECLARE_DEFAULT_HASH(XnActualIntProperty*, XnFirmwareParam, XnFirmwareParamsHash)
	XN_DECLARE_LIST(XnActualIntProperty*, XnActualIntPropertyList)
	XN_DECLARE_DEFAULT_HASH(XnActualIntProperty*, XnUInt32, XnPropertyToValueHash)

	XnStatus AddFirmwareParam(XnActualIntProperty& Property, XnUInt16 nFirmwareParam, XnFWVer nMinVer = XN_SENSOR_FW_VER_UNKNOWN, XnFWVer nMaxVer = XN_SENSOR_FW_VER_UNKNOWN, XnUInt16 nValueIfNotSupported = 0);
	XnStatus AddFirmwareAudioParam(XnActualIntProperty& Property, XnUInt16 nFirmwareParam, XnFWVer nMinVer = XN_SENSOR_FW_VER_3_0, XnFWVer nMaxVer = XN_SENSOR_FW_VER_UNKNOWN, XnUInt16 nValueIfNotSupported = 0);

	XnStatus UpdateProperty(XnFirmwareParam* pParam);

	XnStatus SetFirmwareParam(XnActualIntProperty* pProperty, XnUInt64 nValue);
	XnStatus SetFirmwareAudioParam(XnActualIntProperty* pProperty, XnUInt64 nValue);
	XnStatus SetImageResolution(XnUInt64 nValue);
	XnStatus SetImageFormat(XnUInt64 nValue);
	XnStatus SetStreamMode(XnActualIntProperty* pProperty, XnUInt64 nValue);
	XnStatus RecalculateReferenceResolution();
	XnStatus GetFirmwareParam(XnActualIntProperty* pProperty, XnFirmwareParam** ppParam);

	XnStatus SetFirmwareParamImpl(XnActualIntProperty* pProperty, XnUInt64 nValue);
	XnStatus CheckFirmwareParam(XnActualIntProperty* pProperty, XnUInt64 nValue, XnFirmwareParam** ppParam);

	static XnStatus XN_CALLBACK_TYPE SetFirmwareParamCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareAudioParamCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetImageResolutionCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetImageFormatCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetStreamModeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReferenceResolutionPropertyValueChanged(const XnProperty* pSender, void* pCookie);

	XnFirmwareParamsHash m_AllFirmwareParams;

	XnFirmwareInfo* m_pInfo;
	XnFirmwareCommands* m_pCommands;
	XnBool m_bInTransaction;
	XnActualIntPropertyList m_TransactionOrder; // the transaction according to the order in which it was set
	XnPropertyToValueHash m_Transaction; // maps a property to its new value
};

#endif //__XN_SENSOR_FIRMWARE_PARAMS_H__