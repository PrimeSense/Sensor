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






#ifndef XNV_TORSO_H
#define XNV_TORSO_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnEE/XnVBodyPart.h>
#include <XnEE/XnV3DVector.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * A torso is a body part that has a front vector and an up vector.
 */

class XN_EE_FW_API XnVTorso : public XnVBodyPart
{
public:
	XnVTorso()
	{}
	XnVTorso(const XnVTorso& other)
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

#endif // XNV_TORSO_H
