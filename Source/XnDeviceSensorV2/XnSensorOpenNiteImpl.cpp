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

