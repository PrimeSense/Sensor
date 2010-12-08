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





#ifndef __XN_INT_PROPERTY_SYNCHRONIZER_H__
#define __XN_INT_PROPERTY_SYNCHRONIZER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnList.h>
#include <XnDDK/XnIntProperty.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef XnStatus (XN_CALLBACK_TYPE* XnIntPropertyConvertCallback)(XnUInt64 nSourceValue, XnUInt64* pnDestValue);

class XN_DDK_CPP_API XnIntPropertySynchronizer
{
public:
	XnIntPropertySynchronizer();
	~XnIntPropertySynchronizer();

	XnStatus RegisterSynchronization(XnIntProperty* pSource, XnIntProperty* pDestination, XnIntPropertyConvertCallback pConvertFunc = NULL);

private:
	XnList m_Cookies;
};

#endif //__XN_INT_PROPERTY_SYNCHRONIZER_H__
