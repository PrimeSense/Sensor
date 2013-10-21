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
#include <XnModuleCppRegistratration.h>
#include "XnExportedSensorDevice.h"
#include "XnSensorDepthGenerator.h"
#include "XnSensorImageGenerator.h"
#include "XnSensorIRGenerator.h"
#include "XnSensorAudioGenerator.h"
#include "XnSensorClient.h"

//---------------------------------------------------------------------------
// Exporting
//---------------------------------------------------------------------------

XN_EXPORT_MODULE(xn::Module)
XN_EXPORT_DEVICE(XnExportedSensorDevice)
XN_EXPORT_DEPTH(XnExportedSensorDepthGenerator)
XN_EXPORT_IMAGE(XnExportedSensorImageGenerator)
XN_EXPORT_IR(XnExportedSensorIRGenerator)
XN_EXPORT_AUDIO(XnExportedSensorAudioGenerator)

