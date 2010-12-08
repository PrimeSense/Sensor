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






#ifndef _XNV_HUMAN_STRETCH_H_
#define _XNV_HUMAN_STRETCH_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVRealObject.h"
#include "XnVExtremes.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This algorithm receives a XnVRealObject and a dividing XnVPoint,
 * and generates XnVExtremes object.
 * It uses the XnVRealObject's internal XnVDepthMap, and finds 
 * maximum and minimum values of all axis in each quadrant defined by the dividng point.
 * This means the output XnVExtremes object holds a total of 16 points:
 * For each of the 4 quadrants created by the dividing point, an extreme x value,
 * an extreme y value and 2 extreme z values are found.
 */
class XN_EE_FW_API XnVHumanStretch
{
public:
	XnVHumanStretch();

	/**
	* Assuming the input is a human, find extremes which should represent hands and feet.
	*
	* @param	[in]	roObject	The Real Object to run on
	* @param	[in]	ptDivider	Point by which to divide frame into quadrants.
	* @param	[out]	exExtremes	The extremeties found
	*/
	XnStatus Run(const XnVRealObject& roObject, const XnVPoint& ptDivider, XnVExtremes& exExtremes);

	/**
	* Get separate status for each extremity. This is relevant when Run() returned a value different than XN_STATUS_OK.
	*
	*/
	XnStatus GetTopLeftStatus() const;
	XnStatus GetTopRightStatus() const;
	XnStatus GetBottomLeftStatus() const;
	XnStatus GetBottomRightStatus() const;
protected:
	XnStatus GetCandidates(const XnVDepthMap& dmObject, const XnVPoint& ptDivider,
						XnVPoint& ptXLeft, XnVPoint& ptYLeft, XnVPoint& ptZLeftFront, XnVPoint& ptZLeftBack,
						XnVPoint& ptXRight, XnVPoint& ptYRight, XnVPoint& ptZRightFront, XnVPoint& ptZRightBack,
						XnInt32 nYStart, XnInt32 nYEnd, XnInt32 nMultFactor, XnInt32 nAddFactor);

	bool CheckRelevance(const XnVDepthMap& dmMap, XnUInt16 nX, XnUInt16 nY) const;

	XnUInt16 m_nXFromMiddle;
	XnUInt16 m_nBoxSide;
	XnUInt32 m_nBoxThreshold;

	enum
	{
		XN_HS_TOP_LEFT,
		XN_HS_TOP_RIGHT,
		XN_HS_BOTTOM_LEFT,
		XN_HS_BOTTOM_RIGHT,
	};

	XnStatus m_SpecificStatus[4];

	XnBool m_bTop;
};



#endif //_XNV_HUMAN_STRETCH_H_

