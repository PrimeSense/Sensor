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




#ifndef __XN_DEVICE_FUNCTIONS_TYPEDEFS_H__
#define __XN_DEVICE_FUNCTIONS_TYPEDEFS_H__

// create a list of typedefs

#define XN_DEVICE_TYPEDEF_NAME(name) XnDevice##name##FuncPtr
#define XN_DEVICE_INTERFACE_FUNCTION(name, sig) typedef XnStatus (*XN_DEVICE_TYPEDEF_NAME(name))sig;
#include <XnDeviceProto.inl>
#undef XN_DEVICE_INTERFACE_FUNCTION

#endif //__XN_DEVICE_FUNCTIONS_TYPEDEFS_H__