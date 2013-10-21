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
