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
