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






//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFormats.h"
#include <XnCore.h>
#include <XnOS.h>

// The following line is needed to be once in *ALL* of the high level shared library modules. DO NOT REMOVE!!! 
XN_API_EXPORT_INIT()

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
static XnBool g_XnFormatsWasInit = FALSE;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_FORMATS_API XnStatus XnFormatsInit()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Was the Formats subsystem already initialized?
	if (g_XnFormatsWasInit == FALSE)
	{
		// Init the core subsystem
		nRetVal = XnInit();
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_ALREADY_INIT)
			return nRetVal;

		g_XnFormatsWasInit = TRUE;
	}
	else
	{
		// Trying to init twice...
		return (XN_STATUS_ALREADY_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_FORMATS_API XnStatus XnFormatsInitFromINIFile(const XnChar* cpINIFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpINIFileName);

	// Was the Formats subsystem already initialized?
	if (g_XnFormatsWasInit == FALSE)
	{
		// Init the core subsystem
		nRetVal = XnInitFromINIFile(cpINIFileName);
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_ALREADY_INIT)
			return nRetVal;

		g_XnFormatsWasInit = TRUE;
	}
	else
	{
		// Trying to init twice...
		return (XN_STATUS_ALREADY_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_FORMATS_API XnStatus XnFormatsShutdown()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Was the Formats subsystem initialized?
	if (g_XnFormatsWasInit == TRUE)
	{
		// Shutdown the core subsystem
		nRetVal = XnShutdown();
		if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_NOT_INIT)
			return nRetVal;

		g_XnFormatsWasInit = FALSE;
	}
	else
	{
		// Trying to shutdown without doing init...
		return (XN_STATUS_FORMATS_NOT_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}
