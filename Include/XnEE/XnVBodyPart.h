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






#ifndef _XNV_BODY_PART_H
#define _XNV_BODY_PART_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnEE/XnVPoint.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This object holds a single body part. A basic body part is defined by a point and confidence.
 */
class XN_EE_FW_API XnVBodyPart
{
public:
	XnVBodyPart()
	{}
	XnVBodyPart(const XnVBodyPart& other)
	{
		m_Confidence = other.Confidence();
		m_Position = other.m_Position;
	}

	const XnFloat& Confidence() const
		{return m_Confidence;}
	XnFloat& Confidence()
		{return m_Confidence;}

	const XnVPoint& Position() const
		{return m_Position;}
	XnVPoint& Position()
		{return m_Position;}
protected:
	XnFloat m_Confidence;
	XnVPoint m_Position;

};

#endif
