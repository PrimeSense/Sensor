/*****************************************************************************
*                                                                            *
*  PrimeSense Sensor 5.x Alpha                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Sensor                                    *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#ifndef __XN_STREAM_OUTPUT_SET_H__
#define __XN_STREAM_OUTPUT_SET_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStreamData.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnStreamDataSet; // Forward Declaration
typedef struct XnStreamDataSet XnStreamDataSet;

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------

/**
* Creates a new stream output set object.
*
* @param	ppStreamOutputSet	[out]	A pointer to the newly created object.
*/
XN_DDK_API XnStatus XnStreamDataSetCreate(XnStreamDataSet** ppStreamOutputSet);

/**
* Destroys a previously created stream output set object.
*
* @param	ppStreamOutputSet	[in]	A previously created stream output set object.
*/
XN_DDK_API XnStatus XnStreamDataSetDestroy(XnStreamDataSet** ppStreamOutputSet);

/**
* Adds a stream output object to the set. A set cannot contain more than one stream output for the same stream.
*
* @param	pStreamOutputSet	[in]	The set.
* @param	pStreamOutput		[in]	A stream output object to add to the set
*/
XN_DDK_API XnStatus XnStreamDataSetAdd(XnStreamDataSet* pStreamOutputSet, XnStreamData* pStreamOutput);

/**
* Removes a stream output object from the set.
*
* @param	pStreamOutputSet	[in]	The set.
* @param	pStreamOutput		[in]	The stream output set to remove.
*/
XN_DDK_API XnStatus XnStreamDataSetRemove(XnStreamDataSet* pStreamOutputSet, XnStreamData* pStreamOutput);

/**
* Removes a stream output object from the set, by name.
*
* @param	pStreamOutputSet	[in]	The set.
* @param	StreamName			[in]	The stream which its object should be removed.
*/
XN_DDK_API XnStatus XnStreamDataSetRemoveByName(XnStreamDataSet* pStreamOutputSet, const XnChar* StreamName);

/**
* Gets a stream output object from the set by the name of its stream.
*
* @param	pStreamOutputSet	[in]	The set.
* @param	StreamName			[in]	The stream name.
* @param	ppStreamOutput		[out]	The stream output object related to that stream.
*/
XN_DDK_API XnStatus XnStreamDataSetGet(XnStreamDataSet* pStreamOutputSet, const XnChar* StreamName, XnStreamData** ppStreamOutput);

/**
* Gets the set as an array of pointers to objects.
*
* @param	pStreamOutputSet	[in]		The set.
* @param	apStreamOutputs		[out]		An array of pointers to be filled.
* @param	pnCount				[in/out]	The size of the array.
*/
XN_DDK_API XnStatus XnStreamDataSetCopyToArray(const XnStreamDataSet* pStreamOutputSet, XnStreamData** apStreamOutputs, XnUInt32* pnCount);

#endif //__XN_STREAM_OUTPUT_SET_H__
