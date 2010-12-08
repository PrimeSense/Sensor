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

