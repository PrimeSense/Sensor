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






#ifndef _XNV_USER_EXTRACTOR_H_
#define _XNV_USER_EXTRACTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDepthMap.h"
#include "XnVRealObject.h"
#include "XnVLabelMatrix.h"

#include "XnVBoundingBox.h"

#include "XnVPointContainer.h"
#include "XnVConnectedComponentDetector.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define MAX_CCS 2000
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This algorithm uses as input a XnVDepthMap instance and an XnVLabelMatrix,
 * which may be the output of XnVConnectedComponentDetector.
 * The XnVLabelMatrix can be either supplied by the user
 * or XnVConnectedComponentDetector is performed internally.
 * The algorithm takes the Connected Component which is closest and large enough
 * (threshold exists to determine what "large enough" means"), determines this is
 * the basis of the User, and adds all other Connected Components which are
 * closer than it, or have some overlap with it (another threshold).
 */

class XN_EE_FW_API XnVUserExtractor
{
public:
	XnVUserExtractor();
	~XnVUserExtractor();

	/**
	* Extracts a user from the depth map, putting the result in a XnVRealObject object supplied.
	* A Label Matrix is supplied, created externally.
	*
	* @param	[in]	dmMap		the depth map to work on.
	* @param	[in]	lmLabels	the label matrix, the output of the XnVConnectedComponentDetector
	* @param	[in,out]	roUser		The user in which to put the output. 
	* @param	[in]	bKeep		Should keep the user extracted. False means to have all the scene except for it.
	*/
	XnStatus Run(XnVDepthMap& dmMap, const XnVLabelMatrix& lmLabels, XnVRealObject& roUser, XnBool bKeep = true)
	{
		return Run(dmMap, lmLabels, roUser.GetDepthMap(), bKeep);
	}

	/**
	* Extracts a user from the depth map, leaving only it in the original depth map.
	* A Label Matrix is supplied, created externally.
	*
	* @param	[in,out]	dmMap		the depth map to start from. On success, it will contain only the user.
	* @param	[in]		lmLabels	The label matrix, the output of the XnVConenctedComponentDetector
	*/
	XnStatus Run(XnVDepthMap& dmMap, const XnVLabelMatrix& lmLabels)
	{
		return Run(dmMap, lmLabels, NULL, true);
	}

	/**
	* Extracts a user from the depth map, putting the result in a XnVRealObject object supplied.
	*
	* @param	[in]	dmMap		the depth map to work on.
	* @param	[in,out]	roUser		The user in which to put the output. 
	* @param	[in]	bKeep		Should keep the user extracted. False means to have all the scene except for it.
	*/
	XnStatus Run(XnVDepthMap& dmMap, XnVRealObject& roUser, XnBool bKeep = true)
	{
		XnStatus rc;

		rc = CreateLabels(dmMap);
		if (rc != XN_STATUS_OK)
			return rc;

		return Run(dmMap, *m_pLabels, roUser, bKeep);
	}
	/**
	* Extracts a user from the depth map, leaving only it in the original depth map.
	*
	* @param	[in,out]	dmMap		the depth map to start from. On success, it will contain only the user.
	*/
	XnStatus Run(XnVDepthMap& dmMap)
	{
		XnStatus rc;

		rc = CreateLabels(dmMap);
		if (rc != XN_STATUS_OK)
			return rc;

		return Run(dmMap, *m_pLabels);
	}

	/**
	* MinSizeRatioToChoose determines the minimal size of chosen connected component.
	* The chosen connected component is the closest one that is at least the proper size.
	* The size used is the total amount of pixels divided by this parameter.
	* For instance, if the full resolution is 640x480, and the threshold is set to 32,
	* then the minimal size a connected component must be to be considered large enough
	* is (640*480)/32 = 9600 pixels.
	* The default value is 32.
	*
	* @param	[in]	nMinSizeRatioToChoose
	*/
	void SetMinSizeRatioToChoose(XnUInt32 nMinSizeRatioToChoose)
	{
		m_nMinSizeRatioToChoose = nMinSizeRatioToChoose;
	}
	XnUInt32 GetMinSizeRatioToChoose() const
	{
		return m_nMinSizeRatioToChoose;
	}

	/**
	* DepthOverlap determines the minimal overlap in depth that a connected component has to have
	* with the chosen connected component, in order to be a candidate to join it as the user.
	* For instance, a depth overlap threshold of 1 means any connected component with at least
	* one pixel closer by 1 than the maximal depth of the chosen connected component is added to the user.
	* Smaller values will cause further connected components to be added.
	* The default value is 0.
	*
	* @param	[in]	nDepthOverlap
	*/
	void SetDepthOverlap(XnInt32 nDepthOverlap)
	{
		m_nDepthOverlap = nDepthOverlap;
	}
	XnInt32 GetDepthOverlap() const
	{
		return m_nDepthOverlap;
	}

	/**
	* UseUserBoundingBox is used after a connected component is chosen.
	* If it is true, then all other connected components that don't have any pixel
	* in the chosen one's bounding box will not be considered to be part of the user.
	* The default value is false.
	*
	* @param	[in]	bUseUserBoundingBox
	*/
	void SetUseUserBoundingBox(XnBool bUseUserBoundingBox)
	{
		m_bUseUserBoundingBox = bUseUserBoundingBox;
	}
	XnBool GetUseUserBoundingBox() const
	{
		return m_bUseUserBoundingBox;
	}

protected:
	XnStatus Run(XnVDepthMap& dmMap, const XnVLabelMatrix& lmLabels, XnVDepthMap* dmOutput, XnBool bKeep);
	XnStatus GetCCs(const XnVDepthMap& dmMap, const XnVLabelMatrix& lmLabels1,
				   XnUInt16 nMinX, XnUInt16 nMaxX, XnUInt16 nMinY, XnUInt16 nMaxY);

	XnStatus InitializeLabels();
	XnStatus CreateLabels(XnVDepthMap& dmMap);

	XnVBoundingBox m_CCs[MAX_CCS];

	XnVDepthMap m_dmWorkingMap;
	XnUInt32 m_nMinSizeRatioToChoose;
	XnInt32 m_nDepthOverlap;

	XnBool m_bUseUserBoundingBox;

	XnVConnectedComponentDetector* m_pagLabeler;
	XnVLabelMatrix* m_pLabels;
};


#endif //_XNV_USER_EXTRACTOR_H_

