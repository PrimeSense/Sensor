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






#ifndef _XNV_UPSCALE_H_
#define _XNV_UPSCALE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDepthMap.h"
#include "XnVImageMap.h"
//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * Upscale an XnVDepthMap instance.
 * Scaling is done in powers of 2, and is done by using the depth from a certain pixel
 * and copying it to the entire nxn depth box, where n is the scale factor.
 */
class XN_EE_FW_API XnVUpscale
{
public:
	/**
	* Upscale methods:
	* MULTIPLY - use the same value for entire upscaled 2nx2n
	* GRADUAL - use a gradiant of neighbors. Not Supported.
	*/
	typedef enum
	{
		XNV_USM_MULTIPLY,
		XNV_USM_GRADUAL,
		XNV_USM_UNKNOWN
	} XnVUpscaleMethod;

	XnVUpscale()
	{}
	/**
	* Upscale a depth map in a factor (which must be a power of 2).
	*
	* @param	[in]	dmOrigMap	The original depth map
	* @param	[out]	dmNewMap	The upscaled depth map
	* @param	[in]	nFactor		The factor by which to scale
	* @param	[in]	eUMethod	The scale method. XNV_USM_MULTIPLY is the only one implemented.
	*/
	XnStatus Run(const XnVDepthMap& dmOrigMap, XnVDepthMap& dmNewMap, XnUInt16 nFactor, XnVUpscaleMethod eUMethod);

protected:
	XnStatus GetShiftSize(XnUInt16 nFactor, XnInt8& nShiftSize) const;

	XnStatus UseOne(const XnVDepthMap& dmOrigMap, XnVDepthMap& dmNewMap, XnUInt8 nShiftSize);
};

#endif //_XNV_UPSCALE_H_

