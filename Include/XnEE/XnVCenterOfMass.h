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






#ifndef _XNV_CENTER_OF_MASS_H_
#define _XNV_CENTER_OF_MASS_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDepthMap.h"
#include "XnVStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * Calculate projective Center of Mass - the average of all pixels that have depth.
 */
class XN_EE_FW_API XnVCenterOfMass
{
public:
	XnVCenterOfMass()
	{}

	/**
	* Calculates the center of mass of the given depth map
	*
	* @param	dmMap	[in]	The depth map on which to run
	* @param	ptCoM	[out]	The center of mass found.
	*
	* @return XN_STATUS_OK on success, XNV_STATUS_NOT_ENOUGH_INFORMATION if no points exist in the depth map.
	*/
	XnStatus Run(const XnVDepthMap& dmMap, XnVPoint& ptCoM);
};

#endif //_XNV_CENTER_OF_MASS_H_

