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






#ifndef _XN_COREGLOBALS_H_
#define _XN_COREGLOBALS_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCore.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/** The Xiron I/O INI section name. */ 
#define XN_CORE_INI_SECTION "Core"

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
/** Was the Xiron Core subsystem successfully initialized? */ 
extern XnBool g_bXnCoreWasInit;

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
/** Make sure the core subsystem was initialized. */ 
#define XN_VALIDATE_CORE_INIT()		\
	if (g_bXnCoreWasInit != TRUE)	\
	{									\
		return (XN_STATUS_NOT_INIT);	\
	}

/** Make sure the core subsystem was not initialized already. */ 
#define XN_VALIDATE_CORE_NOT_INIT()		\
	if (g_bXnCoreWasInit == TRUE)		\
	{										\
		return (XN_STATUS_ALREADY_INIT);	\
	}

#endif //_XN_COREGLOBALS_H_