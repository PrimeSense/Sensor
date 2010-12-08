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






#ifndef XNV_SKELETON_H
#define XNV_SKELETON_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnEE/XnVLabeledObject.h>
#include <XnEE/XnVHead.h>
#include <XnEE/XnVTorso.h>
#include <XnEE/XnVHand.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * A skeleton is comprised of a head, a torso and 2 hands.
 * It also holds a labeled object that describes it.
 */

class XN_EE_FW_API XnVSkeleton
{
public:
	XnVSkeleton()
	{}
	XnVSkeleton(const XnVSkeleton& other)
	{
		m_Head = other.m_Head;
		m_Torso = other.m_Torso;
		m_RightHand = other.m_RightHand;
		m_LeftHand = other.m_LeftHand;
	}

	const XnVHead& Head() const
		{return m_Head;}
	XnVHead& Head()
		{return m_Head;}

	const XnVTorso& Torso() const
		{return m_Torso;}
	XnVTorso& Torso()
		{return m_Torso;}

	const XnVHand& RightHand() const
		{return m_RightHand;}
	XnVHand& RightHand()
		{return m_RightHand;}

	const XnVHand& LeftHand() const
		{return m_LeftHand;}
	XnVHand& LeftHand()
		{return m_LeftHand;}

	const XnVLabeledObject& LabeledObject() const
		{return m_LabeledObject;}
	XnVLabeledObject& LabeledObject()
		{return m_LabeledObject;}

private:
	XnVHead m_Head;
	XnVTorso m_Torso;
	XnVHand m_RightHand, m_LeftHand;
	XnVLabeledObject m_LabeledObject;
};

#endif // XNV_SKELETON_H
