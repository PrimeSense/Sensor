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
#ifndef _XNV_HEAD_H
#define _XNV_HEAD_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnEE/XnVBodyPart.h>
#include <XnEE/XnV3DVector.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* A head is a body part that has a front vector and an up vector.
*/

class XN_EE_FW_API XnVHead : public XnVBodyPart
{
public:
	XnVHead()
	{}
	XnVHead(const XnVHead& other)
	{
		m_FrontVector = other.m_FrontVector;
		m_UpVector = other.m_UpVector;
	}

	const XnV3DVector& FrontVector() const
		{return m_FrontVector;}
	XnV3DVector& FrontVector()
		{return m_FrontVector;}

	const XnV3DVector& UpVector() const
		{return m_UpVector;}
	XnV3DVector& UpVector()
		{return m_UpVector;}
protected:
	XnV3DVector m_FrontVector, m_UpVector;
};

#endif // _XNV_HEAD_H
