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
#ifndef _XNV_CLOSEST_POINT_FINDER_H_
#define _XNV_CLOSEST_POINT_FINDER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDepthMap.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * Calculate Projective Closest Point - find the minimum value of depth and the
 * pixel it was found in.
 */
class XN_EE_FW_API XnVClosestPointFinder
{
public:
	XnVClosestPointFinder()
	{}

	/**
	* Find the closest point in the given depth map
	*
	* @param	[in]	dmMap		The depth map on which to run
	* @param	[out]	ptClosest	The closest point in the depth map
	*
	* @return XN_STATUS_OK on success, XNV_STATUS_NOT_ENOUGH_INFORMATION if no points exist in the depth map.
	*/
	XnStatus Run(const XnVDepthMap& dmMap, XnVPoint& ptClosest);
};

#endif //_XNV_CLOSEST_POINT_FINDER_H_

