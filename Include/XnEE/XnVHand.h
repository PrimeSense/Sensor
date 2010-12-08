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






#ifndef _XNV_HAND_H
#define _XNV_HAND_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnEE/XnVBodyPart.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 *  A hand is comprised of its 3 joints: The shoulder, the elbow and the palm.
 */
class XN_EE_FW_API XnVHand
{
public:
	XnVHand()
	{}
	XnVHand(const XnVHand& other)
	{
		this->m_Elbow = other.m_Elbow;
		this->m_Palm = other.m_Palm;
		this->m_Shoulder = other.m_Shoulder;
	}

	const XnVBodyPart& Shoulder() const
		{return m_Shoulder;}
	XnVBodyPart& Shoulder()
		{return m_Shoulder;}

	const XnVBodyPart& Elbow() const
		{return m_Elbow;}
	XnVBodyPart& Elbow()
		{return m_Elbow;}

	const XnVBodyPart& Palm() const
		{return m_Palm;}
	XnVBodyPart& Palm()
		{return m_Palm;}

protected:
	XnVBodyPart m_Shoulder;
	XnVBodyPart	m_Elbow;
	XnVBodyPart	m_Palm;
};

#endif

