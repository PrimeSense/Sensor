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





#ifndef __XN_DATA_PACKER_H__
#define __XN_DATA_PACKER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnIOStream.h>
#include <XnGeneralBuffer.h>
#include <XnStreamData.h>
#include <XnFormats/XnCodec.h>
#include <XnCore/XnBuffer.h>
#include <XnPropertySet.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef enum
{
	XN_PACKED_PROPERTY_SET = 1,
	XN_PACKED_NEW_STREAM = 2,
	XN_PACKED_INT_PROPERTY = 3,
	XN_PACKED_REAL_PROPERTY = 4,
	XN_PACKED_STRING_PROPERTY = 5,
	XN_PACKED_GENERAL_PROPERTY = 6,
	XN_PACKED_STREAM_REMOVED = 7,
	XN_PACKED_STREAM_DATA = 8,
	XN_PACKED_END = 9,
	XN_PACKED_CUSTOM_MESSAGE = 2000,
} XnPackedDataType;

struct XnPackedDataHeader; // Forward declaration

/**
* A helper class for packing and unpacking PS data from IO streams.
*/
class XN_DDK_CPP_API XnDataPacker
{
public:
	/**
	* Creates a DataPacker object. 
	* 
	* @param	pStream				[in]	The stream to read/write data from/to.
	* @param	nInternalBufferSize	[in]	The internal buffer size. This should be the maximum written/read object size.
	*/
	XnDataPacker(XnIOStream* pStream, XnUInt32 nInternalBufferSize);

	/**
	* Destroys a DataPacker object.
	*/
	~XnDataPacker();

	/**
	* Initialized the DataPacker object.
	*/
	XnStatus Init();

	/**
	* Frees the DataPacker object.
	*/
	XnStatus Free();

	/**
	* Writes a new stream into the stream.
	*
	* @param	strType			[in]	The type of the stream.
	* @param	strName			[in]	The name of the stream.
	* @param	pPropertySet	[in]	The property set to write.
	*/
	XnStatus WriteNewStream(const XnChar* strType, const XnChar* strName, const XnPropertySet* pPropertySet);

	/**
	* Writes stream removal into the stream.
	*
	* @param	csStreamName	[in]	The name of the stream.
	*/
	XnStatus WriteStreamRemoved(const XnChar* csStreamName);

	/**
	* Writes a property set into the stream. 
	*
	* @param	pPropertySet	[in]	The property set to write.
	*/
	XnStatus WritePropertySet(const XnPropertySet* pPropertySet);

	/**
	* Writes an int property into the stream.
	*
	* @param	csModuleName	[in]	The name of the module.
	* @param	csProp			[in]	The name of the property.
	* @param	nValue			[in]	The value of the property.
	*/
	XnStatus WriteProperty(const XnChar* csModule, const XnChar* csProp, XnUInt64 nValue);
	/**
	* Writes a real property into the stream.
	*
	* @param	csModuleName	[in]	The name of the module.
	* @param	csProp			[in]	The name of the property.
	* @param	dValue			[in]	The value of the property.
	*/
	XnStatus WriteProperty(const XnChar* csModule, const XnChar* csProp, XnDouble dValue);
	/**
	* Writes a string property into the stream.
	*
	* @param	csModuleName	[in]	The name of the module.
	* @param	csProp			[in]	The name of the property.
	* @param	csValue			[in]	The value of the property.
	*/
	XnStatus WriteProperty(const XnChar* csModule, const XnChar* csProp, const XnChar* csValue);
	/**
	* Writes a general property into the stream.
	*
	* @param	csModuleName	[in]	The name of the module.
	* @param	csProp			[in]	The name of the property.
	* @param	gbValue			[in]	The value of the property.
	*/
	XnStatus WriteProperty(const XnChar* csModule, const XnChar* csProp, XnGeneralBuffer gbValue);
	/**
	* Writes a stream data object into the stream, compressing the data using the given codec.
	*
	* @param	pStreamOutput	[in]	The stream output object to write.
	* @param	pCodec			[in]	The codec that should be used to compress the data.
	*/
	XnStatus WriteStreamData(const XnStreamData* pStreamOutput, XnCodec* pCodec);

	/**
	* Writes a custom data object into the stream.
	*
	* @param	nObjectType		[in]	The type of the object being written.
	* @param	pData			[in]	A pointer to the data to be written.
	* @param	nDataSize		[in]	The size of the written data.
	*/
	XnStatus WriteCustomData(XnUInt32 nObjectType, const void* pData, XnUInt32 nDataSize);

	/**
	* Writes a marker telling this is the end of the stream.
	*/
	XnStatus WriteEnd();

	/**
	* Reads a single object from the IO stream into the internal buffer, and returns the type
	* of that object.
	*
	* @param	pType	[out]	The type of the object that was read from the stream.
	*/
	XnStatus ReadNextObject(XnPackedDataType* pType);

	/**
	* Reads a new stream name event from the internal buffer.
	*
	* @param	csType	[in/out]	A string to be filled with the stream type.
	* @param	csName	[in/out]	A string to be filled with the stream name.
	*/
	XnStatus ReadNewStreamName(XnChar* csType, XnChar* csName);

	/**
	* Reads a new module event from the internal buffer.
	*
	* @param	csModuleName	[in/out]	A string to be filled with the module name.
	* @param	pPropertySet	[in/out]	A property set to be filled with the stream properties.
	*/
	XnStatus ReadNewStream(XnChar* csType, XnChar* csName, XnPropertySet* pPropertySet);

	/**
	* Reads a stream removal event from the internal buffer.
	*
	* @param	csStreamName	[in/out]	A string to be filled with the stream name.
	*/
	XnStatus ReadStreamRemoved(XnChar* csStreamName);

	/**
	* Reads a property set from the internal buffer.
	*
	* @param	pPropertySet	[in/out]	A property set to be filled with the stream properties.
	*/
	XnStatus ReadPropertySet(XnPropertySet* pPropertySet);

	/**
	* Reads an int property from the internal buffer.
	*
	* @param	pcsModule		[in/out]	A string to be filled with the module name.
	* @param	pcsProp			[in/out]	A string to be filled with the property name.
	* @param	pnValue			[out]		The value of the property.
	*/
	XnStatus ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnUInt64* pnValue);
	/**
	* Reads a real property from the internal buffer.
	*
	* @param	pcsModule		[in/out]	A string to be filled with the module name.
	* @param	pcsProp			[in/out]	A string to be filled with the property name.
	* @param	pdValue			[out]		The value of the property.
	*/
	XnStatus ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnDouble* pdValue);
	/**
	* Reads a string property from the internal buffer.
	*
	* @param	pcsModule		[in/out]	A string to be filled with the module name.
	* @param	pcsProp			[in/out]	A string to be filled with the property name.
	* @param	pcsValue		[in/out]	A string to be filled with the property value.
	*/
	XnStatus ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnChar* pcsValue);

	/**
	* Reads a general property from the internal buffer.
	*
	* @param	pcsModule		[in/out]	A string to be filled with the module name.
	* @param	pcsProp			[in/out]	A string to be filled with the property name.
	* @param	pgbValue		[out]		A pointer to a general buffer, that, upon return will point to the data. The user should copy this data.
	*/
	XnStatus ReadProperty(XnChar* pcsModule, XnChar* pcsProp, XnGeneralBuffer* pgbValue);

	/**
	* Reads properties of a stream data.
	*
	* @param	pStreamOutput		[in/out]	A stream data object. All its fields will be populated, except for the actual data.
	* @param	pnCompression		[out]		Compression format.
	* @param	pnCompressedSize	[out]		Compressed Data Size.
	*/
	XnStatus ReadStreamDataProps(XnStreamData* pStreamOutput, XnCompressionFormats* pnCompression, XnUInt32* pnCompressedSize);

	/**
	* Reads a stream data object from the internal buffer, decompressing the data using the given codec.
	*
	* @param	pStreamOutput		[in/out]	A stream output object to be filled from the stream.
	* @param	pCodec				[in]		The codec that should be used to decompress the data.
	*/
	XnStatus ReadStreamData(XnStreamData* pStreamOutput, XnCodec* pCodec);

	/**
	* Reads a custom data from the internal buffer.
	*
	* @param	nObjectType		[in]		The expected object type.
	* @param	pData			[in]		A pointer to a buffer to be filled with data.
	* @param	nDataSize		[in/out]	in: Max allowed size, out: Received size
	*/
	XnStatus ReadCustomData(XnUInt32 nObjectType, void* pData, XnUInt32* pnDataSize);

private:
	/** Starts an internal object. */
	void StartWritingIntenalObject(XnUInt32 nType);
	/** Ends an internal object. */
	void EndWritingInternalObject();
	/** End an entire object and write it to stream. */
	XnStatus FlushInternalBuffer();

	/* Reads the next object into the internal buffer (appending it) */
	XnStatus ReadNextObjectImpl();
	/* If read object is BC, make it look like a new one. */
	XnStatus FixReadBCObjects();

	void MoveToNextObject();

	XnStatus WritePropertySetImpl(const XnPropertySet* pPropertySet);
	XnStatus WritePropertySetProperties(const XnPropertySet* pPropertySet);
	XnStatus WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, XnUInt64 nValue);
	XnStatus WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, XnDouble dValue);
	XnStatus WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, const XnChar* csValue);
	XnStatus WritePropertyImpl(const XnChar* csModule, const XnChar* csProp, XnGeneralBuffer gbValue);
	XnStatus WriteString(const XnChar* pcsString);

	XnStatus ReadPropertySetImpl(XnPropertySet* pSet);
	XnStatus ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnUInt64* pnValue);
	XnStatus ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnDouble* pdValue);
	XnStatus ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnChar* pcsValue);
	XnStatus ReadPropertyImpl(XnChar* pcsModule, XnChar* pcsProp, XnGeneralBuffer* pgbValue);
	XnStatus ReadString(XnChar* pcsString);

	XnStatus ReadInternalBuffer(XnUChar* pData, XnUInt32 nDataSize);
	XnStatus WriteStringToBuffer(const XnChar* csString);
	XnStatus ReadStringFromBuffer(XnChar* pcsString);

	void ResetReadBuffer();

	XnIOStream* m_pStream;
	XnUInt32 m_nBufferSize;
	XnBuffer m_InternalBuffer;
	XnPackedDataHeader* m_pCurrentHeader;
	XnUInt32 m_nInternalBufferReadIndex;
};

#endif //__XN_DATA_PACKER_H__