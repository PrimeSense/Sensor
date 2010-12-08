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






#ifndef _XNV_DEPTH_MAP_H_
#define _XNV_DEPTH_MAP_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDepthRepresentation.h"
#include "XnVDepthMatrix.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a specific representation of the depth, and implements XnVDepthRepresentation.
 * This object holds the depth using XnVDepthMatrix.
 * A XnVDepthMap instance should be initialized either by XnVStreamDetails or
 * by another XnVDepthMap instance using the Clone method.
 * If initialization was done using a buffer and resolution, it may not be valid
 * for certain algorithms, which need the internal XnVStreamDetails
 * and/or XnVRealWorldTranslator.
 */

class XN_EE_CORE_API XnVDepthMap : public XnVDepthRepresentation
{
public:
	/**
	* Create a Depth Map.
	* Depth map is invalid - must later be Initialize()d or Clone()d.
	*/
	XnVDepthMap();
	/**
	* Create a Depth Map.
	* Buffer used will be the input stream's buffer.
	*
	* @param	[in]	pStream					Stream describing the depth map.
	* @param	[in]	bIsLive					TRUE if the map is connected to a stream and updated. After each map update, UnderlyingStreamUpdated should be called.
	* @param	[in]	bWaitForDataUpdate		TRUE to make the map update its properties only after a call to UnderlyingStreamUpdated.
	*/
	XnVDepthMap(XnVStream* pStream, XnBool bIsLive = FALSE, XnBool bWaitForDataUpdate = FALSE);
	/**
	* Create a Depth Map.
	* Buffer used will be the one supplied as parameter.
	*
	* @param	[in]	pBuffer	The buffer to be used as the depth map.
	* @param	[in]	nXRes	X resolution of depth map.
	* @param	[in]	nYRes	Y resolution of depth map.
	*/
	XnVDepthMap(XnDepthPixel* pBuffer, XnUInt16 nXRes, XnUInt16 nYRes);

	virtual ~XnVDepthMap();

	/**
	* Clone the depth map into another.
	* Buffer used in other depth map is allocated and copied from this one.
	*
	* @param	[out]	dmOther	The other depth map, that will have the same data as this one.
	*/
	XnStatus Clone(XnVDepthMap& dmOther) const;
	/**
	* Initialize the depth map based on another. It will take the other depth map's meta data
	* such as stream information.
	* Buffer used is allocate and cleared.
	*
	* @param	[in]	dmOther		The other depth map, which will be the base for this one.
	* @param	[in]	nXRes		X resolution for this depth map.
	* @param	[in]	nYRes		Y resolution for this depth map.
	* @param	[in]	nXOffset	X-axis offset into the other depth map.
	* @param	[in]	nYOffset	Y-axis offset into the other depth map.
	*/
	XnStatus Initialize(const XnVDepthMap& dmOther, XnUInt16 nXRes, XnUInt16 nYRes, XnUInt16 nXOffset = 0, XnUInt16 nYOffset = 0);

	XnDepthPixel* Data();
	const XnDepthPixel* Data() const;
	XnDepthPixel& operator[](XnUInt32 nIndex);
	const XnDepthPixel& operator[](XnUInt32 nIndex) const;
	XnDepthPixel& operator()(XnInt32 nX, XnInt32 nY);
	const XnDepthPixel& operator()(XnInt32 nX, XnInt32 nY) const;

	void Clear();

	XnStatus SetBuffer(void* pBuffer, XnUInt16 nXRes, XnUInt16 nYRes);
	XnStatus ReAdjust(XnUInt16 nXRes, XnUInt16 nYRes);

	XnStatus CopyFrom(const XnVDepthMap& dm, XnUInt64 nTimestamp = 0);

protected:
	XnStatus ReadPropertiesFromStream();

	XnVDepthMatrix m_pDepth;
};

#endif //_XNV_DEPTH_MAP_H_

