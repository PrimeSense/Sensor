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
#include "XnIntPropertySynchronizer.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnIntSynchronizerCookie
{
public:
	XnIntSynchronizerCookie(XnIntProperty* pSource, XnIntProperty* pDestination, XnIntPropertyConvertCallback pConvertFunc) :
	  pSource(pSource), pDestination(pDestination), pConvertFunc(pConvertFunc)
	{}

	XnIntProperty* pSource;
	XnIntProperty* pDestination;
	XnIntPropertyConvertCallback pConvertFunc;
	XnCallbackHandle hCallback;
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnIntPropertySynchronizer::XnIntPropertySynchronizer()
{
}

XnIntPropertySynchronizer::~XnIntPropertySynchronizer()
{
	for (XnList::Iterator it = m_Cookies.begin(); it != m_Cookies.end(); ++it)
	{
		XnIntSynchronizerCookie* pSynchData = (XnIntSynchronizerCookie*)*it;
		pSynchData->pSource->OnChangeEvent().Unregister(pSynchData->hCallback);
		XN_DELETE(pSynchData);
	}
}

XnStatus XN_CALLBACK_TYPE IntPropertyValueChangedCallback(const XnProperty* pSender, void* pCookie)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// get the property current value
	XnIntProperty* pIntProp = (XnIntProperty*)pSender;

	XnUInt64 nNewValue;
	nRetVal = pIntProp->GetValue(&nNewValue);
	XN_IS_STATUS_OK(nRetVal);

	XnIntSynchronizerCookie* pSynchData = (XnIntSynchronizerCookie*)pCookie;

	XnUInt64 nDestValue;

	// convert the value if needed
	if (pSynchData->pConvertFunc != NULL)
	{
		nRetVal = pSynchData->pConvertFunc(nNewValue, &nDestValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nDestValue = nNewValue;
	}

	// now set the new value
	nRetVal = pSynchData->pDestination->UnsafeUpdateValue(nDestValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnIntPropertySynchronizer::RegisterSynchronization(XnIntProperty* pSource, XnIntProperty* pDestination, XnIntPropertyConvertCallback pConvertFunc /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnIntSynchronizerCookie* pCookie;
	XN_VALIDATE_NEW(pCookie, XnIntSynchronizerCookie, pSource, pDestination, pConvertFunc);

	nRetVal = m_Cookies.AddFirst(pCookie);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pCookie);
		return (nRetVal);
	}

	nRetVal = pSource->OnChangeEvent().Register(IntPropertyValueChangedCallback, pCookie, &pCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pCookie);
		m_Cookies.Remove(m_Cookies.begin());
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

