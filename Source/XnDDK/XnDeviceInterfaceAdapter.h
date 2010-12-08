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





#ifndef __XN_DEVICE_INTERFACE_ADAPTER_H__
#define __XN_DEVICE_INTERFACE_ADAPTER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "IXnDevice.h"
#include "XnDeviceManager.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnDeviceInterfaceAdapter : public IXnDevice
{
public:
	XnDeviceInterfaceAdapter(XnDeviceInterfaceFunctions* pFuncs) :
		m_pFuncs(pFuncs)
	{}

	~XnDeviceInterfaceAdapter()
	{
		Destroy();
	}

	virtual XnStatus Init(const XnDeviceConfig* pDeviceConfig)
	{
		return m_pFuncs->Create(&m_Handle, pDeviceConfig);		
	}

	virtual XnStatus Destroy()
	{
		return m_pFuncs->Destroy(&m_Handle);
	}

	virtual XnStatus GetSupportedStreams(const XnChar** aStreamNames, XnUInt32* pnStreamNamesCount)
	{
		return m_pFuncs->GetSupportedStreams(m_Handle, aStreamNames, pnStreamNamesCount);
	}

	virtual XnStatus CreateStream(const XnChar* StreamType, const XnChar* StreamName = NULL, const XnPropertySet* pInitialValues = NULL)
	{
		return m_pFuncs->CreateStream(m_Handle, StreamType, StreamName, pInitialValues);
	}

	virtual XnStatus DestroyStream(const XnChar* StreamName)
	{
		return m_pFuncs->DestroyStream(m_Handle, StreamName);
	}

	virtual XnStatus OpenStream(const XnChar* StreamName)
	{
		return m_pFuncs->OpenStream(m_Handle, StreamName);
	}

	virtual XnStatus CloseStream(const XnChar* StreamName)
	{
		return m_pFuncs->CloseStream(m_Handle, StreamName);
	}

	virtual XnStatus DoesModuleExist(const XnChar* ModuleName, XnBool* pbDoesExist)
	{
		return m_pFuncs->DoesModuleExist(m_Handle, ModuleName, pbDoesExist);
	}

	virtual XnStatus OpenAllStreams()
	{
		return m_pFuncs->OpenAllStreams(m_Handle);
	}

	virtual XnStatus CloseAllStreams()
	{
		return m_pFuncs->CloseAllStreams(m_Handle);
	}

	virtual XnStatus RegisterToStreamsChange(XnDeviceOnStreamsChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
	{
		return m_pFuncs->RegisterToStreamsChange(m_Handle, Handler, pCookie, phCallback);
	}

	virtual XnStatus UnregisterFromStreamsChange(XnCallbackHandle hCallback)
	{
		return m_pFuncs->UnregisterFromStreamsChange(m_Handle, hCallback);
	}

	virtual XnStatus CreateStreamData(const XnChar* StreamName, XnStreamData** ppStreamData)
	{
		return m_pFuncs->CreateStreamData(m_Handle, StreamName, ppStreamData);
	}

	virtual XnStatus RegisterToNewStreamData(XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
	{
		return m_pFuncs->RegisterToNewStreamData(m_Handle, Handler, pCookie, phCallback);
	}

	virtual XnStatus UnregisterFromNewStreamData(XnCallbackHandle hCallback)
	{
		return m_pFuncs->UnregisterFromNewStreamData(m_Handle, hCallback);
	}

	virtual XnStatus IsNewDataAvailable(const XnChar* StreamName, XnBool* pbNewDataAvailable, XnUInt64* pnTimestamp)
	{
		return m_pFuncs->IsNewDataAvailable(m_Handle, StreamName, pbNewDataAvailable, pnTimestamp);
	}

	virtual XnStatus ReadStream(XnStreamData* pStreamOutput)
	{
		return m_pFuncs->ReadStream(m_Handle, pStreamOutput);
	}

	virtual XnStatus Read(XnStreamDataSet* pStreamOutputSet)
	{
		return m_pFuncs->Read(m_Handle, pStreamOutputSet);
	}

	virtual XnStatus WriteStream(XnStreamData* pStreamOutput)
	{
		return m_pFuncs->WriteStream(m_Handle, pStreamOutput);
	}

	virtual XnStatus Write(XnStreamDataSet* pStreamOutputSet)
	{
		return m_pFuncs->Write(m_Handle, pStreamOutputSet);
	}

	virtual XnStatus Tell(XnUInt64* pnTimestamp)
	{
		return m_pFuncs->Tell(m_Handle, pnTimestamp);
	}

	virtual XnStatus Seek(XnUInt64 nTimestamp)
	{
		return m_pFuncs->Seek(m_Handle, nTimestamp);
	}

	virtual XnStatus DoesPropertyExist(const XnChar* ModuleName, const XnChar* PropertyName, XnBool* pbDoesExist)
	{
		return m_pFuncs->DoesPropertyExist(m_Handle, ModuleName, PropertyName, pbDoesExist);
	}

	virtual XnStatus GetPropertyType(const XnChar* ModuleName, const XnChar* PropertyName, XnPropertyType* pnType)
	{
		return m_pFuncs->GetPropertyType(m_Handle, ModuleName, PropertyName, pnType);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64 nValue)
	{
		return m_pFuncs->SetIntProperty(m_Handle, ModuleName, PropertyName, nValue);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble dValue)
	{
		return m_pFuncs->SetRealProperty(m_Handle, ModuleName, PropertyName, dValue);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, const XnChar* csValue)
	{
		return m_pFuncs->SetStringProperty(m_Handle, ModuleName, PropertyName, csValue);
	}

	virtual XnStatus SetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnGeneralBuffer Value)
	{
		return m_pFuncs->SetGeneralProperty(m_Handle, ModuleName, PropertyName, Value);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnUInt64* pnValue)
	{
		return m_pFuncs->GetIntProperty(m_Handle, ModuleName, PropertyName, pnValue);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnDouble* pdValue)
	{
		return m_pFuncs->GetRealProperty(m_Handle, ModuleName, PropertyName, pdValue);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnChar* csValue)
	{
		return m_pFuncs->GetStringProperty(m_Handle, ModuleName, PropertyName, csValue);
	}

	virtual XnStatus GetProperty(const XnChar* ModuleName, const XnChar* PropertyName, XnGeneralBuffer* pValue)
	{
		return m_pFuncs->GetGeneralProperty(m_Handle, ModuleName, PropertyName, pValue);
	}

	virtual XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* csSectionName)
	{
		return m_pFuncs->LoadConfigFromFile(m_Handle, csINIFilePath, csSectionName);
	}

	virtual XnStatus GetAllProperties(XnPropertySet* pSet, XnBool bNoStreams = FALSE, const XnChar* strModule = NULL)
	{
		return m_pFuncs->GetAllProperties(m_Handle, pSet, bNoStreams, strModule);
	}

	virtual XnStatus RegisterToPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle* phCallback)
	{
		return m_pFuncs->RegisterToPropertyChange(m_Handle, Module, PropertyName, Handler, pCookie, phCallback);
	}

	virtual XnStatus UnRegisterFromPropertyChange(const XnChar* Module, const XnChar* PropertyName, XnCallbackHandle hCallback)
	{
		return m_pFuncs->UnregisterFromPropertyChange(m_Handle, Module, PropertyName, hCallback);
	}

private:
	XnDeviceInterfaceFunctions* m_pFuncs;
	XnDeviceHandle m_Handle;
};

#endif //__XN_DEVICE_INTERFACE_ADAPTER_H__