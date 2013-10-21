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
#ifndef _XNV_LABELED_OBJECT_H_
#define _XNV_LABELED_OBJECT_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVRealObject.h"
#include "XnVLabelMatrix.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* This is a real object that holds a label matrix with it. In effect, it creates a mask over the depth map
* and allows categorization of it using labels.
* As a Real Object, the LabeledObject holds a label (actually, as a XnVObject) and a pointer to XnVDepthMap.
*/

class XN_EE_FW_API XnVLabeledObject : public XnVRealObject
{
public:
	XnVLabeledObject() :
		XnVRealObject(NULL)
		{}

	const XnVLabelMatrix* GetLabelMatrix() const
		{return m_pLM;}

	void SetLabelMatrix(XnVLabelMatrix* pLM)
		{m_pLM = pLM;}

	virtual XnDepthPixel GetAt(XnUInt32 nPos) const
	{
		if (((XnVObject::XnVLabel*)m_pLM->Data())[nPos] == m_nID)
			return m_pdmDepthMap->Data()[nPos];
		return m_pdmDepthMap->GetNoSampleValue();
	}

protected:
	XnVLabelMatrix* m_pLM;
};

#endif // _XNV_LABELED_OBJECT_H_
