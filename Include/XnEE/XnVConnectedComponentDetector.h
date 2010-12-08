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






#ifndef _XNV_CONNECTED_COMPONENT_DETECTOR_H_
#define _XNV_CONNECTED_COMPONENT_DETECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDepthMap.h"
#include "XnVObject.h"
#include "XnVLabelMatrix.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * The Connected Component Detection algorithm goes over a XnVDepthMap and fills
 * a XnVLabelMatrix object.
 * A Connected Component is defined in the following way:
 * Two neighboring pixels are in the same Connected Component iff
 * their absolute depth difference is smaller than a certain threshold.
 * This algorithm goes over the input XnVDepthMap, and finds all the
 * Connected Components, giving each of them a unique XnVObject::XnVLabel,
 * and updating the XnVLabelMatrix with the label in the position matching the pixel.
 * A pixel that doesn't belong to any Connected Component, either because it has
 * no depth, or because its Connected Component was smaller than the size threshold
 * and was therefore disregarded, gets label 0.
 * Any other Connected Component gets a label from 1 and onwards.
 * There may be lapses in the label numbering, since the numbering is done before
 * the Connected Components that are too small are removed.
 * This algorithm has no memory: running it on different XnVDepthMap
 * (even if it's the next frame) won't necessarily result in the
 * "same" Connected Component getting the same label.
 */
class XN_EE_FW_API XnVConnectedComponentDetector
{
public:
	XnVConnectedComponentDetector();
	~XnVConnectedComponentDetector();

	/**
	* Label the depth map with connected components labels.
	* All members of a connected component have the same label.
	*
	* @param	[in]	dmMap		The depth map on which to run
	* @param	[out]	vmLabels	A matrix of labels, where the label at (x,y) represents the label of pixel (x,y)
	*
	* @return XN_STATUS_OK on success
	*/
	XnStatus Run(const XnVDepthMap& dmMap, XnVLabelMatrix& vmLabels);

	/**
	* Change the minimal size of ConnectedComponent allowed. This is measured in ratio to the resolution. 
	* For example, if the ratio is 320, and resolution is 640*480, then the minimal CC will be of size (640*480)/320=960 pixels.
	* The default value is 320.
	*
	* @param	[in]	nRatio	The new ratio
	*/
	void SetMinCCSizeRatio(XnUInt32 nRatio);
	XnUInt32 GetMinCCSizeRatio() const;

	/**
	* Change the maximal depth distance (in mm) between neighbors that are in the same Connected Component.
	* The default value is 100 (10cm)
	*
	* @param	[in]	nDistance	The new Distance.
	*/
	void SetMaxNeighborDistance(XnInt32 nDistance);
	XnInt32 GetMaxNeighborDistance() const;

protected:

	class XN_EE_FW_API XnVDisjointSet
	{
	public:
		XnVDisjointSet();
		~XnVDisjointSet();
		inline void Resize(XnUInt32 nSize);
		inline XnUInt32 Unite(XnUInt32 i, XnUInt32 j);
		inline void Clear();
		inline XnUInt16& operator[](XnInt32 nIndex);
		inline const XnUInt16& operator[](XnInt32 nIndex) const;
	protected:
		void SetRoot(XnUInt32 i, XnUInt32 nRoot);
		XnUInt32 FindRoot(XnUInt32 i);

		XnUInt32 m_nSize;
		XnUInt16* m_vSet;
	};

	XnUInt32 m_nSetSize;
	XnUInt32 m_nCounters;

	XnUInt32* m_vCounters;
	XnVDisjointSet m_vDisjointSet;

	XnUInt32 m_nMinCCSizeRatio; // Minimal size of CC to exist is (xres*yres)/ratio
	XnInt32 m_nMaxNeighborDistance; // Maximal distance for neighbor to be in the same Connected Component
};

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------


#endif //_XNV_CONNECTED_COMPONENT_DETECTOR_H_

