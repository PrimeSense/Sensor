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

