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
#ifndef _XNV_DOWNSCALE_H_
#define _XNV_DOWNSCALE_H_

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
 * Downscale the XnVDepthMap or XnVImageMap.
 * Scaling is done in powers of 2, and can be done in one of two methods:
 * The first algorithm uses the representative method.
 * For each nxn pixel box (n being the scale requested) the top left pixel is used.
 * The second algorithm uses the average method.
 * For each nxn pixel box the average of the data (depth in XnVDepthMap,
 * color in XnVImageMap) is used.
 */
class XN_EE_FW_API XnVDownscale
{
public:
	/**
	* Downscale methods:
	* REPRESENTATIVE - use one of the 2nx2n box as the value of the downscaled
	* AVERAGE - use an average of the 2nx2n box as the value of the downscaled
	*/
	typedef enum
	{
		XNV_DSM_REPRESENTATIVE,
		XNV_DSM_AVERAGE,
		XNV_DSM_UNKNOWN
	} XnVDownscaleMethod;

	XnVDownscale();
	/**
	* Downscale a depth map.
	*
	* @param	[in]	dmOrigMap	The original depth map
	* @param	[out]	dmNewMap	The result depth map
	* @param	[in]	nFactor		The factor of the downscale. Must be a power of 2.
	* @param	[in]	eDMethod	The downscale method, use the average of an area or a single pixel in it.
	* 
	* @return XN_STATUS_OK on success, XN_STATUS_EE_ILLEGAL_INPUT if factor isn't a power of 2, doesn't divide either resolution or the method is unknown.
	*/
	XnStatus Run(const XnVDepthMap& dmOrigMap, XnVDepthMap& dmNewMap, XnUInt16 nFactor, XnVDownscaleMethod eDMethod);
	/**
	* Downscale an image map.
	*
	* @param	[in]	imOrigMap	The original image map
	* @param	[out]	imNewMap	The result image map
	* @param	[in]	nFactor		The factor of the downscale. Must be a power of 2.
	* @param	[in]	eDMethod	The downscale method, use the average of an area or a single pixel in it.
	* 
	* @return XN_STATUS_OK on success, XN_STATUS_EE_ILLEGAL_INPUT if factor isn't a power of 2, doesn't divide either resolution or the method is unknown.
	*/
	XnStatus Run(const XnVImageMap& imOrigMap, XnVImageMap& imNewMap, XnUInt16 nFactor, XnVDownscaleMethod eDMethod);

protected:
	XnStatus GetShiftSize(XnUInt16 nFactor, XnInt8& nShiftSize) const;

	XnStatus UseOne(const XnVDepthMap& dmOrigMap, XnVDepthMap& dmNewMap, XnUInt8 nShiftSize);
	XnStatus UseAverage(const XnVDepthMap& dmOrigMap, XnVDepthMap& dmNewMap, XnUInt8 nShiftSize, XnUInt16 nFactor);

	XnStatus UseOne(const XnVImageMap& dmOrigMap, XnVImageMap& dmNewMap, XnUInt8 nShiftSize);
	XnStatus UseAverage(const XnVImageMap& dmOrigMap, XnVImageMap& dmNewMap, XnUInt8 nShiftSize, XnUInt16 nFactor);

	XnUInt16 m_DepthDifferenceAverageThreshold;
};

#endif //_XNV_DOWNSCALE_H_

