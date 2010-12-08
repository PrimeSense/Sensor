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





#ifndef __XN_DEVICE_BASE_PROXY_H__
#define __XN_DEVICE_BASE_PROXY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "IXnDevice.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_VALIDATE_ACTUAL_DEVICE		\
	if (m_pActual == NULL) return XN_STATUS_ERROR;

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** 
* A base class for all proxies. By default, every call gets transfered to the actual device.
* Deriving classes may add additional behavior.
*/
class XN_DDK_CPP_API XnDeviceBaseProxy : public IXnDevice
{
public:
	virtual XnStatus Init(const XnDeviceConfig* pDeviceConfig)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->Init(pDeviceConfig);
	}

	virtual XnStatus Destroy()
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->Destroy();
	}

	virtual XnStatus GetSupportedStreams(const XnChar** aStreamNames, XnUInt32* pnStreamNamesCount)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetSupportedStreams(aStreamNames, pnStreamNamesCount);
	}

	virtual XnStatus CreateStream(const XnChar* StreamType, const XnChar* StreamName = NULL, const XnPropertySet* pInitialValues = NULL)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->CreateStream(StreamType, StreamName, pInitialValues);
	}

	virtual XnStatus DestroyStream(const XnChar* StreamName)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->DestroyStream(StreamName);
	}

	virtual XnStatus OpenStream(const XnChar* StreamName)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->OpenStream(StreamName);
	}

	virtual XnStatus CloseStream(const XnChar* StreamName)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->CloseStream(StreamName);
	}

	virtual XnStatus GetStreamNames(const XnChar** pstrNames, XnUInt32* pnNamesCount)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetStreamNames(pstrNames, pnNamesCount);
	}

	virtual XnStatus DoesModuleExist(const XnChar* ModuleName, XnBool* pbDoesExist)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->DoesModuleExist(ModuleName, pbDoesExist);
	}

	virtual XnStatus OpenAllStreams()
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->OpenAllStreams();
	}

	virtual XnStatus CloseAllStreams()
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->CloseAllStreams();
	}

	virtual XnStatus RegisterToStreamsChange(XnDeviceOnStreamsChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->RegisterToStreamsChange(Handler, pCookie, phCallback);
	}

	virtual XnStatus UnregisterFromStreamsChange(XnCallbackHandle hCallback)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->UnregisterFromStreamsChange(hCallback);
	}

	virtual XnStatus CreateStreamData(const XnChar* StreamName, XnStreamData** ppStreamData)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->CreateStreamData(StreamName, ppStreamData);
	}

	virtual XnStatus RegisterToNewStreamData(XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->RegisterToNewStreamData(Handler, pCookie, phCallback);
	}

	virtual XnStatus UnregisterFromNewStreamData(XnCallbackHandle hCallback)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->UnregisterFromNewStreamData(hCallback);
	}

	virtual XnStatus IsNewDataAvailable(const XnChar* StreamName, XnBool* pbNewDataAvailable, XnUInt64* pnTimestamp)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->IsNewDataAvailable(StreamName, pbNewDataAvailable, pnTimestamp);
	}

	virtual XnStatus ReadStream(XnStreamData* pStreamOutput)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->ReadStream(pStreamOutput);
	}

	virtual XnStatus Read(XnStreamDataSet* pStreamOutputSet)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->Read(pStreamOutputSet);
	}

	virtual XnStatus WriteStream(XnStreamData* pStreamOutput)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->WriteStream(pStreamOutput);
	}

	virtual XnStatus Write(XnStreamDataSet* pStreamOutputSet)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->Write(pStreamOutputSet);
	}

	virtual XnStatus Tell(XnUInt64* pnTimestamp)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->Tell(pnTimestamp);
	}

	virtual XnStatus Seek(XnUInt64 nTimestamp)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->Seek(nTimestamp);
	}

	virtual XnStatus TellFrame(XnUInt32* pnFrameID)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->TellFrame(pnFrameID);
	}

	virtual XnStatus SeekFrame(XnUInt32 nFrameID)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->SeekFrame(nFrameID);
	}

	virtual XnStatus DoesPropertyExist(const XnChar* ModuleName, const XnChar* PropertyName, XnBool* pbDoesExist)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->DoesPropertyExist(ModuleName, PropertyName, pbDoesExist);
	}

	virtual XnStatus GetPropertyType(const XnChar* ModuleName, const XnChar* PropertyName, XnPropertyType* pnType)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetPropertyType(ModuleName, PropertyName, pnType);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->SetProperty(ModuleName, PropertyName, nValue);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->SetProperty(ModuleName, PropertyName, dValue);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->SetProperty(ModuleName, PropertyName, csValue);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& Value)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->SetProperty(ModuleName, PropertyName, Value);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetProperty(ModuleName, PropertyName, pnValue);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetProperty(ModuleName, PropertyName, pdValue);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnChar* csValue)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetProperty(ModuleName, PropertyName, csValue);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnGeneralBuffer& gbValue)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetProperty(ModuleName, PropertyName, gbValue);
	}

	virtual XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->LoadConfigFromFile(csINIFilePath, csSectionName);
	}

	virtual XnStatus BatchConfig(const XnPropertySet* pChangeSet)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->BatchConfig(pChangeSet);
	}

	virtual XnStatus GetAllProperties(XnPropertySet* pSet, XnBool bNoStreams = FALSE, const XnChar* strModule = NULL)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->GetAllProperties(pSet, bNoStreams, strModule);
	}

	virtual XnStatus RegisterToPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->RegisterToPropertyChange(Module, PropertyName, Handler, pCookie, phCallback);
	}

	virtual XnStatus UnregisterFromPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnCallbackHandle hCallback)
	{
		XN_VALIDATE_ACTUAL_DEVICE;
		return m_pActual->UnregisterFromPropertyChange(Module, PropertyName, hCallback);
	}

protected:
	XnDeviceBaseProxy(IXnDevice* pActual)
	{ 
		ReplaceActualDevice(pActual); 
	}

	IXnDevice* GetActual() { return m_pActual; }

	void ReplaceActualDevice(IXnDevice* pActual)
	{
		m_pActual = pActual;
	}

private:
	IXnDevice* m_pActual;
};

#endif //__XN_DEVICE_BASE_PROXY_H__