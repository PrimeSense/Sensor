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
#include <XnCore.h>
#include <XnOS.h>
#include <XnCore/XnCoreGlobals.h>
#include <XnLog.h>
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
// Note: See the XnIOGlobals.h file for global variables description
XnBool g_bXnCoreWasInit = FALSE;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

// The following line is needed to be once in *ALL* of the high level shared library modules. DO NOT REMOVE!!! 
XN_API_EXPORT_INIT()

XN_CORE_API XnStatus XnInit()
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Make sure the core subsystem is not already initialized
	XN_VALIDATE_CORE_NOT_INIT();

	// Initialize the OS subsystem
	nRetVal = xnOSInit();
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_OS_ALREADY_INIT)
	{
		return (nRetVal);
	}

	// initialize log
	nRetVal = xnLogInitSystem();
	XN_IS_STATUS_OK(nRetVal);

	// Mark the Xiron core subsystem initialization as successful
	g_bXnCoreWasInit = TRUE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnInitFromINIFile(const XnChar* cpINIFileName)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpINIFileName);

	// Call the real init function
	nRetVal = XnInit();
	XN_IS_STATUS_OK(nRetVal);

	// initialize log
	nRetVal = xnLogInitFromINIFile(cpINIFileName, XN_CORE_INI_SECTION);
	XN_IS_STATUS_OK(nRetVal);

	// initialize profiling
	nRetVal = xnProfilingInitFromINI(cpINIFileName, XN_CORE_INI_SECTION);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnShutdown()
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Check if the core subsystem is initialized
	XN_VALIDATE_CORE_INIT();	

	// Shutdown the network subsystem
	nRetVal = xnOSShutdownNetwork();
	XN_IS_STATUS_OK(nRetVal);

	// Shutdown the OS subsystem
	nRetVal = xnOSShutdown();
	XN_IS_STATUS_OK(nRetVal);

	// Shutdown the Log subsystem
	nRetVal = xnLogClose();
	XN_IS_STATUS_OK(nRetVal);

	// Shutdown the Profiling subsystem
	nRetVal = xnProfilingShutdown();
	XN_IS_STATUS_OK(nRetVal);

	// Mark the Xiron core subsystem as not-initialized
	g_bXnCoreWasInit = FALSE;

	// All is good...
	return (XN_STATUS_OK);
}
