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
#include "XnDataProcessorHolder.h"
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDataProcessorHolder::XnDataProcessorHolder() :
	m_pProcessor(NULL),
	m_hLock(NULL)
{
}

XnDataProcessorHolder::~XnDataProcessorHolder()
{
	xnOSCloseCriticalSection(&m_hLock);
	XN_DELETE(m_pProcessor);
}

XnStatus XnDataProcessorHolder::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnOSCreateCriticalSection(&m_hLock);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

void XnDataProcessorHolder::Lock()
{
	xnOSEnterCriticalSection(&m_hLock);
}

void XnDataProcessorHolder::Unlock()
{
	xnOSLeaveCriticalSection(&m_hLock);
}

void XnDataProcessorHolder::Replace(XnDataProcessor* pNew)
{
	// lock first, to make sure processor is not in use right now
	Lock();

	if (m_pProcessor != NULL)
	{
		XN_DELETE(m_pProcessor);
	}

	m_pProcessor = pNew;

	Unlock();
}

void XnDataProcessorHolder::ProcessData(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize)
{
	if (m_pProcessor == NULL)
		return;

	// lock first
	Lock();

	// check again (it could have been replaced while we waited to lock)
	if (m_pProcessor != NULL)
	{
		m_pProcessor->ProcessData(pHeader, pData, nDataOffset, nDataSize);
	}

	Unlock();
}

