/*****************************************************************************
*                                                                            *
*  PrimeSense Common                                                         *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PrimeSense Common.                                   *
*                                                                            *
*  PrimeSense Common is free software: you can redistribute it and/or modify *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  PrimeSense Common is distributed in the hope that it will be useful,      *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with PrimeSense Common. If not, see <http://www.gnu.org/licenses/>. *
*                                                                            *
*****************************************************************************/


#ifndef __XN_COMMON_H__
#define __XN_COMMON_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStatus.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_VENDOR_PRIMESENSE "PrimeSense"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef enum XnPrimeSenseErrorModules
{
	XN_ERROR_GROUP_SECURITY = 0,
	XN_ERROR_GROUP_FORMATS = 1000,
	XN_ERROR_GROUP_DDK = 2000,
	XN_ERROR_GROUP_DEVICE = 3000,
	XN_ERROR_GROUP_IO = 4000,
	XN_ERROR_GROUP_EE_CORE = 5000,
	XN_ERROR_GROUP_EE_FRAMEWORK = 6000,
	XN_ERROR_GROUP_EE_NITE = 7000,
} XnPrimeSenseErrorModules;

#define XN_PS_STATUS_MESSAGE_MAP_START(module)								\
	XN_STATUS_MESSAGE_MAP_START_FROM(XN_ERROR_GROUP_PRIMESENSE, module)

#define XN_PS_STATUS_MESSAGE_MAP_END(module)								\
	XN_STATUS_MESSAGE_MAP_END_FROM(XN_ERROR_GROUP_PRIMESENSE, module)

#endif // __XN_COMMON_H__
