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
