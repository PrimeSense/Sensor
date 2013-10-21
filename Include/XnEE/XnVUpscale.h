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

