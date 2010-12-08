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






#ifndef _XNV_STATIC_SHIFT_MAP_H_
#define _XNV_STATIC_SHIFT_MAP_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVGeneralStaticMap.h"
#include "XnVDepthMap.h"
#include "XnVDevice.h"

/**
 * This is a specific Static Map implementation, which implements XnVGeneralStaticMap.
 * It uses a measure based on the noise of the depth values as a threshold.
 * This value affects the depth differently in different depth values - the larger the depth,
 * the larger the noise described by the same threshold.
 */

class XN_EE_FW_API XnVStaticMap : public XnVGeneralStaticMap
{
public:
	XnVStaticMap(XnVDevice& Device, const XnChar* strINIFile = NULL);
	~XnVStaticMap();

	/**
	* Add a depth map to the static map.
	* If previous static map existed and is in the same resolutions, the current depth map will be added.
	* If previous static map existed and is in different resolutions, the current depth map will replace.
	*
	* @param	[in]	dmMap	A depth map to add to the static.
	*/
	XnStatus AddMap(const XnVDepthMap& dmMap);

	/**
	* This threshold is added as the noise relative to the depth in each pixel.
	* The default value is 3.
	*
	* @param	[in]	nThreshold	The new Shift Threshold
	*/
	void SetShiftThreshold(XnInt16 nThreshold);
	XnInt16 GetShiftThreshold() const;

protected:
	/**
	* Do specific things requires in this type of GeneralStaticMap when it is Reset.
	*/
	XnStatus DoReset();

	XnStatus Init(XnUInt32 nNewSize);
	XnDepthPixel DepthToNoise(XnDepthPixel nDepth, XnInt16 nShiftThreshold);

	XnBool CreateShiftToDepthTables();
	XnBool m_bAllocatedTables;

	XnUInt32* m_pAccumulationMap;
	XnUInt16* m_pCounterMap;

	XnUInt16* m_pShiftToDepth;
	XnUInt16* m_pDepthToShift;
	XnVDevice* m_pCreatorDevice;

	XnInt16 m_nShiftThreshold;

	XnUInt32 m_nMaxDepth;
};

#endif
