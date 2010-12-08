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
