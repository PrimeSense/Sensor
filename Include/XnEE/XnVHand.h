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

