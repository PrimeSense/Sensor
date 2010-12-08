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






#ifndef _XNV_LABEL_PRUNER_H_
#define _XNV_LABEL_PRUNER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVLabeledObject.h"
#include "XnVDepthMap.h"
#include "XnVLabelMatrix.h"

//---------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------

/**
 * This algorithm removes from the input XnVDepthMap the values where the matching XnVLabelMatrix has 
 * a value different from the input XnVObject::XnVLabel.
 */

class XN_EE_FW_API XnVLabelPruner
{
public:
	/**
	* Set NoSample value in all the places in the depth map that correspond with a label other than
	* the desired one.
	*
	* @param	[in,out]	dmMap	The depth map to be pruned
	* @param	[in]	lmLabels	Label per pixel
	* @param	[in]	nLabel		The label we want to keep
	*/
	XnStatus Run(XnVDepthMap& dmMap, const XnVLabelMatrix& lmLabels, const XnVObject::XnVLabel nLabel);
	XnStatus Run(XnVLabeledObject& lo);
};

#endif // _XNV_LABEL_PRUNER_H_
