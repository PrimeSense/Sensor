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
