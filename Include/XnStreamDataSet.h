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
