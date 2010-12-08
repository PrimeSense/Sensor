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
#include "XnDataPacker.h"
#include <XnDDK.h>
#include <XnLog.h>
#include <XnDevice.h>
#include "XnStreamDataInternal.h"
#include "XnPropertySetInternal.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_PACKED_STRING								1000
#define XN_PACKED_PROPERTY_SET_MODULES_END_MARKER		1001
#define XN_PACKED_PROPERTY_SET_PROPERTIES_END_MARKER	1002

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#pragma pack (push, 1)

typedef struct XnPackedDataHeader
{
	XnPackedDataType nType;
	XnUInt32 nSize;
} XnPackedDataHeader;

#pragma pack (pop)

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define XN_VALIDATE_OBJECT_TYPE(type)							\
	if (m_pCurrentHeader == NULL)								\
	{															\
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Cannot read an object before a call to ReadNextObject()!");	\
	}															\
	if (m_pCurrentHeader->nType != type)						\
	{															\
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Trying to read object of type %d when stream contains object of type %d!", type, m_pCurrentHeader->nType);	\
	}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDataPacker::XnDataPacker(XnIOStream* pStream, XnUInt32 nInternalBufferSize) :
	m_pStream(pStream),
	m_nBufferSize(nInternalBufferSize),
	m_nInternalBufferReadIndex(0),
	m_pCurrentHeader(NULL)
{
	ResetReadBuffer();
}

XnDataPacker::~XnDataPacker()
{
	Free();
}

XnStatus XnDataPacker::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// we allocate enough space for the data and the header
	nRetVal = m_InternalBuffer.Allocate(m_nBufferSize + sizeof(XnPackedDataHeader));
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::Free()
{
	m_InternalBuffer.Free();
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteNewStream(const XnChar* strType, const XnChar* strName, const XnPropertySet* pPropertySet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_NEW_STREAM);

	nRetVal = WriteStringToBuffer(strType);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WriteStringToBuffer(strName);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WritePropertySetImpl(pPropertySet);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadNewStreamName(XnChar* csType, XnChar* csName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_NEW_STREAM);

	// keep last read index (because we only want to know the stream name, without
	// actually removing anything from the buffer
	XnUInt32 nReadIndexBefore = m_nInternalBufferReadIndex;

	// read stream type
	nRetVal = ReadStringFromBuffer(csType);
	XN_IS_STATUS_OK(nRetVal);

	// read stream name
	nRetVal = ReadStringFromBuffer(csName);
	XN_IS_STATUS_OK(nRetVal);

	// restore read index
	m_nInternalBufferReadIndex = nReadIndexBefore;
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadNewStream(XnChar* csType, XnChar* csName, XnPropertySet* pPropertySet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_NEW_STREAM);

	nRetVal = ReadStringFromBuffer(csType);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadStringFromBuffer(csName);
	XN_IS_STATUS_OK(nRetVal);

	MoveToNextObject();

	nRetVal = ReadPropertySetImpl(pPropertySet);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WritePropertySetProperties(const XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	for (XnPropertySetData::Iterator it = pSet->pData->begin(); it != pSet->pData->end(); ++it)
	{
		XnActualPropertiesHash* pModule = it.Value();
		for (XnActualPropertiesHash::ConstIterator itProp = pModule->begin(); itProp != pModule->end(); ++itProp)
		{
			switch (itProp.Value()->GetType())
			{
			case XN_PROPERTY_TYPE_INTEGER:
				{
					XnActualIntProperty* pProp = (XnActualIntProperty*)itProp.Value();
					nRetVal = WritePropertyImpl(pProp->GetModule(), pProp->GetName(), pProp->GetValue());
					XN_IS_STATUS_OK(nRetVal);
					break;
				}
			case XN_PROPERTY_TYPE_REAL:
				{
					XnActualRealProperty* pProp = (XnActualRealProperty*)itProp.Value();
					nRetVal = WritePropertyImpl(pProp->GetModule(), pProp->GetName(), pProp->GetValue());
					XN_IS_STATUS_OK(nRetVal);
					break;
				}
			case XN_PROPERTY_TYPE_STRING:
				{
					XnActualStringProperty* pProp = (XnActualStringProperty*)itProp.Value();
					nRetVal = WritePropertyImpl(pProp->GetModule(), pProp->GetName(), pProp->GetValue());
					XN_IS_STATUS_OK(nRetVal);
					break;
				}
			case XN_PROPERTY_TYPE_GENERAL:
				{
					XnActualGeneralProperty* pProp = (XnActualGeneralProperty*)itProp.Value();
					nRetVal = WritePropertyImpl(pProp->GetModule(), pProp->GetName(), pProp->GetValue());
					XN_IS_STATUS_OK(nRetVal);
					break;
				}
			default:
				XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Unknown property type: %d", itProp.Value()->GetType());
			}
		}
	}

	StartWritingIntenalObject(XN_PACKED_PROPERTY_SET_PROPERTIES_END_MARKER);
	EndWritingInternalObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WritePropertySetImpl(const XnPropertySet* pPropertySet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_PROPERTY_SET);

	// Write module names
	for (XnPropertySetData::Iterator it = pPropertySet->pData->begin(); it != pPropertySet->pData->end(); ++it)
	{
		nRetVal = WriteString(it.Key());
		XN_IS_STATUS_OK(nRetVal);
	}

	StartWritingIntenalObject(XN_PACKED_PROPERTY_SET_MODULES_END_MARKER);
	EndWritingInternalObject();

	// write properties
	nRetVal = WritePropertySetProperties(pPropertySet);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WritePropertySet(const XnPropertySet* pPropertySet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = WritePropertySetImpl(pPropertySet);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadPropertySetImpl(XnPropertySet* pPropertySet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_PROPERTY_SET);

	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];

	// read property set marker
	MoveToNextObject();

	while (m_pCurrentHeader->nType != XN_PACKED_PROPERTY_SET_MODULES_END_MARKER)
	{
		nRetVal = ReadString(strModule);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnPropertySetAddModule(pPropertySet, strModule);
		XN_IS_STATUS_OK(nRetVal);
	}

	// read properties
	XnChar strName[XN_DEVICE_MAX_STRING_LENGTH];

	// read the modules end marker
	MoveToNextObject();

	while (m_pCurrentHeader->nType != XN_PACKED_PROPERTY_SET_PROPERTIES_END_MARKER)
	{
		// read property and add it to set
		switch (m_pCurrentHeader->nType)
		{
		case XN_PACKED_INT_PROPERTY:
			{
				XnUInt64 nValue;
				nRetVal = ReadPropertyImpl(strModule, strName, &nValue);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnPropertySetAddIntProperty(pPropertySet, strModule, strName, nValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PACKED_REAL_PROPERTY:
			{
				XnDouble dValue;
				nRetVal = ReadPropertyImpl(strModule, strName, &dValue);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnPropertySetAddRealProperty(pPropertySet, strModule, strName, dValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PACKED_STRING_PROPERTY:
			{
				XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
				nRetVal = ReadPropertyImpl(strModule, strName, strValue);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnPropertySetAddStringProperty(pPropertySet, strModule, strName, strValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PACKED_GENERAL_PROPERTY:
			{
				XnGeneralBuffer gbValue;
				nRetVal = ReadPropertyImpl(strModule, strName, &gbValue);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = XnPropertySetAddGeneralProperty(pPropertySet, strModule, strName, &gbValue);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Stream contains an object of type %d in the middle of a property set!", m_pCurrentHeader->nType);
		}
	} // props loop

	// read properties end marker
	MoveToNextObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadPropertySet(XnPropertySet* pPropertySet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = ReadPropertySetImpl(pPropertySet);
	XN_IS_STATUS_OK(nRetVal);

	ResetReadBuffer();
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteStreamRemoved(const XnChar* csStreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_STREAM_REMOVED);

	nRetVal = WriteStringToBuffer(csStreamName);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadStreamRemoved(XnChar* csStreamName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_STREAM_REMOVED);

	nRetVal = ReadStringFromBuffer(csStreamName);
	XN_IS_STATUS_OK(nRetVal);

	ResetReadBuffer();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_INT_PROPERTY);

	nRetVal = WriteStringToBuffer(csModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WriteStringToBuffer(csProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_InternalBuffer.Write((const XnUChar*)&nValue, sizeof(XnUInt64));
	XN_IS_STATUS_OK(nRetVal);

	EndWritingInternalObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteProperty(const XnChar* csModule, const XnChar* csProp, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = WritePropertyImpl(csModule, csProp, nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnUInt64* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_INT_PROPERTY);

	nRetVal = ReadStringFromBuffer(pcsModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadStringFromBuffer(pcsProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadInternalBuffer((XnUChar*)pnValue, sizeof(XnUInt64));
	XN_IS_STATUS_OK(nRetVal);

	MoveToNextObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnUInt64* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = ReadPropertyImpl(pcsModule, pcsProp, pnValue);
	XN_IS_STATUS_OK(nRetVal);
	
	ResetReadBuffer();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_REAL_PROPERTY);

	nRetVal = WriteStringToBuffer(csModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WriteStringToBuffer(csProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_InternalBuffer.Write((const XnUChar*)&dValue, sizeof(XnDouble));
	XN_IS_STATUS_OK(nRetVal);

	EndWritingInternalObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteProperty(const XnChar* csModule, const XnChar* csProp, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = WritePropertyImpl(csModule, csProp, dValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnDouble* pdValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_REAL_PROPERTY);

	nRetVal = ReadStringFromBuffer(pcsModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadStringFromBuffer(pcsProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadInternalBuffer((XnUChar*)pdValue, sizeof(XnDouble));
	XN_IS_STATUS_OK(nRetVal);

	MoveToNextObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnDouble* pdValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ReadPropertyImpl(pcsModule, pcsProp, pdValue);
	XN_IS_STATUS_OK(nRetVal);

	ResetReadBuffer();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, const XnChar* csValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_STRING_PROPERTY);

	nRetVal = WriteStringToBuffer(csModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WriteStringToBuffer(csProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WriteStringToBuffer(csValue);
	XN_IS_STATUS_OK(nRetVal);

	EndWritingInternalObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteProperty(const XnChar* csModule, const XnChar* csProp, const XnChar* csValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = WritePropertyImpl(csModule, csProp, csValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnChar* pcsValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_STRING_PROPERTY);

	nRetVal = ReadStringFromBuffer(pcsModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadStringFromBuffer(pcsProp);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadStringFromBuffer(pcsValue);
	XN_IS_STATUS_OK(nRetVal);

	MoveToNextObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnChar* pcsValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ReadPropertyImpl(pcsModule, pcsProp, pcsValue);
	XN_IS_STATUS_OK(nRetVal);

	ResetReadBuffer();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, XnGeneralBuffer gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_GENERAL_PROPERTY);

	nRetVal = WriteStringToBuffer(csModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = WriteStringToBuffer(csProp);
	XN_IS_STATUS_OK(nRetVal);

	// write buffer size
	nRetVal = m_InternalBuffer.Write((const XnUChar*)&gbValue.nDataSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// write buffer
	nRetVal = m_InternalBuffer.Write((const XnUChar*)gbValue.pData, gbValue.nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	EndWritingInternalObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteProperty(const XnChar* csModule, const XnChar* csProp, XnGeneralBuffer gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = WritePropertyImpl(csModule, csProp, gbValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnGeneralBuffer* pgbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_GENERAL_PROPERTY);

	nRetVal = ReadStringFromBuffer(pcsModule);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ReadStringFromBuffer(pcsProp);
	XN_IS_STATUS_OK(nRetVal);

	// convert data into general buffer form
	XnGeneralBuffer gb;
	nRetVal = ReadInternalBuffer((XnUChar*)&gb.nDataSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);
	gb.pData = (void*)(m_InternalBuffer.GetData() + m_nInternalBufferReadIndex);
	m_nInternalBufferReadIndex += gb.nDataSize;

	*pgbValue = gb;

	MoveToNextObject();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnGeneralBuffer* pgbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ReadPropertyImpl(pcsModule, pcsProp, pgbValue);
	XN_IS_STATUS_OK(nRetVal);

	ResetReadBuffer();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteStreamData(const XnStreamData* pStreamOutput, XnCodec* pCodec)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject(XN_PACKED_STREAM_DATA);

	// write stream name
	nRetVal = WriteStringToBuffer(pStreamOutput->StreamName);
	XN_IS_STATUS_OK(nRetVal);

	// write timestamp
	nRetVal = m_InternalBuffer.Write((const XnUChar*)&pStreamOutput->nTimestamp, sizeof(XnUInt64));
	XN_IS_STATUS_OK(nRetVal);

	// write frame-id
	nRetVal = m_InternalBuffer.Write((const XnUChar*)&pStreamOutput->nFrameID, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// write compression format
	XnCompressionFormats nFormat = pCodec->GetCompressionFormat();
	nRetVal = m_InternalBuffer.Write((const XnUChar*)&nFormat, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// write original data size
	nRetVal = m_InternalBuffer.Write((const XnUChar*)&pStreamOutput->nDataSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// leave space for compressed data size
	XnUInt32* pnCompressedDataSize = (XnUInt32*)m_InternalBuffer.GetUnsafeWritePointer();
	*pnCompressedDataSize = m_InternalBuffer.GetFreeSpaceInBuffer();
	m_InternalBuffer.UnsafeUpdateSize(sizeof(XnUInt32));

	// compress data
	nRetVal = pCodec->Compress((const XnUChar*)pStreamOutput->pData, pStreamOutput->nDataSize, m_InternalBuffer.GetUnsafeWritePointer(), pnCompressedDataSize);
	XN_IS_STATUS_OK(nRetVal);

	// update size (add compressed data)
	m_InternalBuffer.UnsafeUpdateSize(*pnCompressedDataSize);

	EndWritingInternalObject();

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadStreamDataProps(XnStreamData* pStreamOutput, XnCompressionFormats* pnCompression, XnUInt32* pnCompressedSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_STREAM_DATA);

	// keep last read index (because we only want to know the stream name, without
	// actually removing anything from the buffer
	XnUInt32 nReadIndexBefore = m_nInternalBufferReadIndex;

	// read stream name
	nRetVal = ReadStringFromBuffer(pStreamOutput->StreamName);
	XN_IS_STATUS_OK(nRetVal);

	// read timestamp
	nRetVal = ReadInternalBuffer((XnUChar*)&pStreamOutput->nTimestamp, sizeof(XnUInt64));
	XN_IS_STATUS_OK(nRetVal);

	// read frame-id
	nRetVal = ReadInternalBuffer((XnUChar*)&pStreamOutput->nFrameID, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// read compression format
	nRetVal = ReadInternalBuffer((XnUChar*)pnCompression, sizeof(XnCompressionFormats));
	XN_IS_STATUS_OK(nRetVal);

	// read data size
	nRetVal = ReadInternalBuffer((XnUChar*)&pStreamOutput->nDataSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// read compressed data size
	nRetVal = ReadInternalBuffer((XnUChar*)pnCompressedSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// restore read index
	m_nInternalBufferReadIndex = nReadIndexBefore;

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadStreamData(XnStreamData* pStreamOutput, XnCodec* pCodec)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_STREAM_DATA);

	// read stream name
	nRetVal = ReadStringFromBuffer(pStreamOutput->StreamName);
	XN_IS_STATUS_OK(nRetVal);

	// read timestamp
	nRetVal = ReadInternalBuffer((XnUChar*)&pStreamOutput->nTimestamp, sizeof(XnUInt64));
	XN_IS_STATUS_OK(nRetVal);

	// read frame-id
	nRetVal = ReadInternalBuffer((XnUChar*)&pStreamOutput->nFrameID, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// read compression format
	XnCompressionFormats nFormat;
	nRetVal = ReadInternalBuffer((XnUChar*)&nFormat, sizeof(XnCompressionFormats));
	XN_IS_STATUS_OK(nRetVal);

	if (nFormat != pCodec->GetCompressionFormat())
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Data in stream is packed with another codec than the one provided!");
	}

	// read original data size
	XnUInt32 nDataSize;
	nRetVal = ReadInternalBuffer((XnUChar*)&nDataSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	if (pStreamOutput->pInternal->bAllocated && pStreamOutput->pInternal->nAllocSize < nDataSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	pStreamOutput->nDataSize = nDataSize;

	// read compressed data size
	XnUInt32 nCompressedDataSize;
	nRetVal = ReadInternalBuffer((XnUChar*)&nCompressedDataSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// decompress
	nRetVal = pCodec->Decompress(m_InternalBuffer.GetData() + m_nInternalBufferReadIndex, nCompressedDataSize, (XnUChar*)pStreamOutput->pData, &pStreamOutput->nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	ResetReadBuffer();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteCustomData(XnUInt32 nObjectType, const void* pData, XnUInt32 nDataSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject((XnPackedDataType)nObjectType);

	// write buffer size
	nRetVal = m_InternalBuffer.Write((const XnUChar*)&nDataSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// write buffer
	nRetVal = m_InternalBuffer.Write((const XnUChar*)pData, nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	EndWritingInternalObject();

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadCustomData(XnUInt32 nObjectType, void* pData, XnUInt32* pnDataSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OBJECT_TYPE(nObjectType);

	// read buffer size
	XnUInt32 nSize = 0;
	nRetVal = ReadInternalBuffer((XnUChar*)&nSize, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	if (nSize > *pnDataSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	nRetVal = ReadInternalBuffer((XnUChar*)pData, nSize);
	XN_IS_STATUS_OK(nRetVal);

	*pnDataSize = nSize;

	ResetReadBuffer();

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteEnd()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	StartWritingIntenalObject(XN_PACKED_END);
	EndWritingInternalObject();

	nRetVal = FlushInternalBuffer();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteString(const XnChar* strString)
{
	XnStatus nRetVal = XN_STATUS_OK;

	StartWritingIntenalObject((XnPackedDataType)XN_PACKED_STRING);

	nRetVal = WriteStringToBuffer(strString);
	XN_IS_STATUS_OK(nRetVal);

	EndWritingInternalObject();
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadString(XnChar* pcsString)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_OBJECT_TYPE(XN_PACKED_STRING);

	nRetVal = ReadStringFromBuffer(pcsString);
	XN_IS_STATUS_OK(nRetVal);

	MoveToNextObject();
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::FixReadBCObjects()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// keep a pointer to current header (it might change if we read additional objects)
	XnPackedDataHeader* pHeader = m_pCurrentHeader;
	XnUInt32 nStartReadIndex = m_nInternalBufferReadIndex;

	switch (m_pCurrentHeader->nType)
	{
	case XN_PACKED_PROPERTY_SET:
		{
			// check if this is the old version (with size 0)
			if (m_nInternalBufferReadIndex != m_InternalBuffer.GetSize())
			{
				return (XN_STATUS_OK);
			}

			// It is old version. Fix it.
			// read all module names (+ modules end marker)
			while (TRUE)
			{
				nRetVal = ReadNextObjectImpl();
				XN_IS_STATUS_OK(nRetVal);

				if (m_pCurrentHeader->nType == XN_PACKED_PROPERTY_SET_MODULES_END_MARKER)
				{
					break;
				}

				// make sure this is a string
				XN_VALIDATE_OBJECT_TYPE(XN_PACKED_STRING);
			}

			// read properties (+ properties end marker)
			while (TRUE)
			{
				nRetVal = ReadNextObjectImpl();
				XN_IS_STATUS_OK(nRetVal);

				if (m_pCurrentHeader->nType == XN_PACKED_PROPERTY_SET_PROPERTIES_END_MARKER)
				{
					break;
				}

				// make sure this is a property
				if (m_pCurrentHeader->nType != XN_PACKED_INT_PROPERTY &&
					m_pCurrentHeader->nType != XN_PACKED_REAL_PROPERTY &&
					m_pCurrentHeader->nType != XN_PACKED_STRING_PROPERTY &&
					m_pCurrentHeader->nType != XN_PACKED_GENERAL_PROPERTY)
				{
					XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Trying to read object of type property when stream contains object of type %d!", m_pCurrentHeader->nType);
				}
			}

			break;
		}
	case XN_PACKED_NEW_STREAM:
		{
			// check if this is old version (bad size). Read both strings
			XnChar strTemp[XN_DEVICE_MAX_STRING_LENGTH];
			nRetVal = ReadStringFromBuffer(strTemp);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = ReadStringFromBuffer(strTemp);
			XN_IS_STATUS_OK(nRetVal);

			if (m_nInternalBufferReadIndex != m_InternalBuffer.GetSize())
			{
				m_nInternalBufferReadIndex = nStartReadIndex;
				return (XN_STATUS_OK);
			}

			// read next object
			nRetVal = ReadNextObjectImpl();
			XN_IS_STATUS_OK(nRetVal);

			// make sure this is a property set
			XN_VALIDATE_OBJECT_TYPE(XN_PACKED_PROPERTY_SET);

			break;
		}
	default:
		// other objects are not BC
		return (XN_STATUS_OK);
	}

	// move back to start
	m_nInternalBufferReadIndex = nStartReadIndex;
	// update size accordingly
	pHeader->nSize = m_InternalBuffer.GetUnsafeWritePointer() - (XnUChar*)pHeader - sizeof(XnPackedDataHeader);
	// now place header back at root object
	m_pCurrentHeader = pHeader;

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadNextObjectImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read header
	m_pCurrentHeader = (XnPackedDataHeader*)m_InternalBuffer.GetUnsafeWritePointer();
	nRetVal = m_pStream->ReadData((XnUChar*)m_pCurrentHeader, sizeof(XnPackedDataHeader));
	XN_IS_STATUS_OK(nRetVal);

	m_InternalBuffer.UnsafeUpdateSize(sizeof(XnPackedDataHeader));
	m_nInternalBufferReadIndex += sizeof(XnPackedDataHeader);

	// check size
	if (m_InternalBuffer.GetFreeSpaceInBuffer() < m_pCurrentHeader->nSize)
	{
		return (XN_STATUS_INTERNAL_BUFFER_TOO_SMALL);
	}

	// read data
	nRetVal = m_pStream->ReadData(m_InternalBuffer.GetUnsafeWritePointer(), m_pCurrentHeader->nSize);
	XN_IS_STATUS_OK(nRetVal);

	m_InternalBuffer.UnsafeUpdateSize(m_pCurrentHeader->nSize);

	// fix BC objects
	nRetVal = FixReadBCObjects();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadNextObject(XnPackedDataType* pType)
{
	XnStatus nRetVal = XN_STATUS_OK;

	ResetReadBuffer();

	nRetVal = ReadNextObjectImpl();
	XN_IS_STATUS_OK(nRetVal);

	*pType = m_pCurrentHeader->nType;

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadInternalBuffer(XnUChar* pData, XnUInt32 nDataSize)
{
	if ((m_InternalBuffer.GetSize() - m_nInternalBufferReadIndex) < nDataSize)
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;

	// read
	xnOSMemCopy(pData, m_InternalBuffer.GetData() + m_nInternalBufferReadIndex, nDataSize);

	// update read index
	m_nInternalBufferReadIndex += nDataSize;

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::WriteStringToBuffer(const XnChar* csString)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get length (including null termination)
	XnUInt32 nLength = strlen(csString) + 1;
	if (nLength >= XN_DEVICE_MAX_STRING_LENGTH)
		return (XN_STATUS_INTERNAL_BUFFER_TOO_SMALL);

	// write length
	nRetVal = m_InternalBuffer.Write((const XnUChar*)&nLength, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	// write string
	nRetVal = m_InternalBuffer.Write((const XnUChar*)csString, nLength);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnDataPacker::ReadStringFromBuffer(XnChar* pcsString)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// read length
	XnUInt32 nLength;
	nRetVal = ReadInternalBuffer((XnUChar*)&nLength, sizeof(XnUInt32));
	XN_IS_STATUS_OK(nRetVal);

	//read string
	nRetVal = ReadInternalBuffer((XnUChar*)pcsString, nLength);
	XN_IS_STATUS_OK(nRetVal);

	// null terminate the string
	pcsString[nLength] = '\0';

	return (XN_STATUS_OK);
}

void XnDataPacker::StartWritingIntenalObject(XnUInt32 nType)
{
	m_pCurrentHeader = (XnPackedDataHeader*)m_InternalBuffer.GetUnsafeWritePointer();
	m_InternalBuffer.UnsafeUpdateSize(sizeof(XnPackedDataHeader));
	m_pCurrentHeader->nType = (XnPackedDataType)nType;
	m_pCurrentHeader->nSize = 0;
}

void XnDataPacker::EndWritingInternalObject()
{
	m_pCurrentHeader->nSize = m_InternalBuffer.GetUnsafeWritePointer() - (XnUChar*)m_pCurrentHeader - sizeof(XnPackedDataHeader);
}

XnStatus XnDataPacker::FlushInternalBuffer()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// make sure root header contains entire size (without the header itself)
	XnPackedDataHeader* pHeader = (XnPackedDataHeader*)m_InternalBuffer.GetData();
	pHeader->nSize = m_InternalBuffer.GetSize() - sizeof(XnPackedDataHeader);

	nRetVal = m_pStream->WriteData(m_InternalBuffer.GetData(), m_InternalBuffer.GetSize());
	XN_IS_STATUS_OK(nRetVal);

	// reset buffer
	m_InternalBuffer.Reset();

	return (XN_STATUS_OK);
}

void XnDataPacker::MoveToNextObject()
{
	m_pCurrentHeader = (XnPackedDataHeader*)(m_InternalBuffer.GetData() + m_nInternalBufferReadIndex);
	m_nInternalBufferReadIndex += sizeof(XnPackedDataHeader);
}

void XnDataPacker::ResetReadBuffer()
{
	m_InternalBuffer.Reset();
	m_pCurrentHeader = NULL;
	m_nInternalBufferReadIndex = 0;
}
