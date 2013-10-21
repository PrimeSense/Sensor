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
