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
