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






#ifndef _XNV_STATUS_H_
#define _XNV_STATUS_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStatus.h>
#include <XnCommon.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_EE_CORE "EECore"

#ifdef XN_EE_CORE_EXPORTS
	#define XN_EE_CORE_API XN_API_EXPORT	
#else
	#define XN_EE_CORE_API XN_API_IMPORT
#endif

#ifdef XN_EE_FW_EXPORTS
	#define XN_EE_FW_API XN_API_EXPORT	
#else
	#define XN_EE_FW_API XN_API_IMPORT
#endif

#ifdef XN_EE_MODULES_EXPORTS
	#define XN_EE_MODULES_API XN_API_EXPORT
#else
	#define XN_EE_MODULES_API XN_API_IMPORT
#endif

#ifdef XN_USE_DEVICE_3_6 
	#define XN_3_6_API 
	#define XN_3_6_PUBLIC_MEMBER public:
#else
	#define XN_3_6_API XN_API_DEPRECATED("This is old 3.6 API. Please don't use it anymore")
	#define XN_3_6_PUBLIC_MEMBER private:
#endif

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

XN_PS_STATUS_MESSAGE_MAP_START(XN_ERROR_GROUP_EE_CORE)
XN_STATUS_MESSAGE(XN_STATUS_EE_NOT_INITIALIZED, "Device was not initialized!")
XN_STATUS_MESSAGE(XN_STATUS_EE_RESOLUTION_ILLEGAL, "Invalid resolution!")
XN_STATUS_MESSAGE(XN_STATUS_EE_RESOLUTION_MISMATCH, "Resolutions do not match!")
XN_STATUS_MESSAGE(XN_STATUS_EE_NOT_ENOUGH_INFORMATION, "Not enough information!")
XN_STATUS_MESSAGE(XN_STATUS_EE_ILLEGAL_INPUT, "Illegal input!")
XN_STATUS_MESSAGE(XN_STATUS_EE_NOT_ENOUGH_MEMORY, "Not enough memory!")
XN_STATUS_MESSAGE(XN_STATUS_EE_WRONG_DEVICE, "Wrong device!")
XN_STATUS_MESSAGE(XN_STATUS_EE_PARTIAL_SUCCESS, "Operation succeeded partially!")
XN_STATUS_MESSAGE(XN_STATUS_EE_UNSUPPORTED_FUNCTIONALITY, "This functionality is not supported!")
XN_STATUS_MESSAGE(XN_STATUS_EE_BAD_PARAMETER, "Bad parameter!")
XN_STATUS_MESSAGE(XN_STATUS_EE_ERROR, "Error!")
XN_STATUS_MESSAGE(XN_STATUS_EE_MATRIX_NOT_SYMMETRIC, "Matrix must be symmetric for this operation")
XN_PS_STATUS_MESSAGE_MAP_END(XN_ERROR_GROUP_EE_CORE)

//---------------------------------------------------------------------------
// Backwards compatibility
//---------------------------------------------------------------------------
#ifndef XN_MESSAGE_MAP_REGISTER

typedef XnStatus XN_API_DEPRECATED("Please use XnStatus instead.") XnVStatus;

#define DECLARE_DEPRECATED_XNV_STATUS(xnv, xn)		\
	const XnStatus XN_API_DEPRECATED("Please use " #xn " instead.") xnv = xn;

DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_OK, XN_STATUS_OK);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_NOT_INITIALIZED, XN_STATUS_EE_NOT_INITIALIZED);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_RESOLUTION_ILLEGAL, XN_STATUS_EE_RESOLUTION_ILLEGAL);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_RESOLUTION_MISMATCH, XN_STATUS_EE_RESOLUTION_MISMATCH);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_NOT_ENOUGH_INFORMATION, XN_STATUS_EE_NOT_ENOUGH_INFORMATION);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_ILLEGAL_INPUT, XN_STATUS_EE_ILLEGAL_INPUT);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_NOT_ENOUGH_MEMORY, XN_STATUS_EE_NOT_ENOUGH_MEMORY);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_WRONG_DEVICE, XN_STATUS_EE_WRONG_DEVICE);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_PATRIAL_SUCCESS, XN_STATUS_EE_PARTIAL_SUCCESS);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_UNSUPPORTED_FUNCITONALITY, XN_STATUS_EE_UNSUPPORTED_FUNCTIONALITY);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_BAD_PARAMETER, XN_STATUS_EE_BAD_PARAMETER);
DECLARE_DEPRECATED_XNV_STATUS(XNV_STATUS_ERROR, XN_STATUS_EE_ERROR);
DECLARE_DEPRECATED_XNV_STATUS(XN_STATUS_EE_UNSUPPORTED_FUNCITONALITY, XN_STATUS_EE_UNSUPPORTED_FUNCTIONALITY);

#endif

#endif // _XNV_STATUS_H_

