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

