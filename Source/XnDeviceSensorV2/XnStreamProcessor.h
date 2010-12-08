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





#ifndef __XN_STREAM_PROCESSOR_H__
#define __XN_STREAM_PROCESSOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDataProcessor.h"
#include "XnSensorStreamHelper.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/**
* Base class for streams data processors
*/
class XnStreamProcessor : public XnDataProcessor
{
public:
	XnStreamProcessor(XnDeviceStream* pStream, XnSensorStreamHelper* pHelper);

//---------------------------------------------------------------------------
// Utility Functions
//---------------------------------------------------------------------------
protected:
	XnDeviceStream* GetStream() { return m_pStream; }
	XnSensorStreamHelper* GetStreamHelper() { return m_pHelper; }

//---------------------------------------------------------------------------
// Members
//---------------------------------------------------------------------------
private:
	XnDeviceStream* m_pStream;
	XnSensorStreamHelper* m_pHelper;
};

#endif //__XN_STREAM_PROCESSOR_H__
