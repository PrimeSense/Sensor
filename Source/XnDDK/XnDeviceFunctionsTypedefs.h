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
#ifndef __XN_DEVICE_FUNCTIONS_TYPEDEFS_H__
#define __XN_DEVICE_FUNCTIONS_TYPEDEFS_H__

// create a list of typedefs

#define XN_DEVICE_TYPEDEF_NAME(name) XnDevice##name##FuncPtr
#define XN_DEVICE_INTERFACE_FUNCTION(name, sig) typedef XnStatus (*XN_DEVICE_TYPEDEF_NAME(name))sig;
#include <XnDeviceProto.inl>
#undef XN_DEVICE_INTERFACE_FUNCTION

#endif //__XN_DEVICE_FUNCTIONS_TYPEDEFS_H__