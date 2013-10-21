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
#include "XnActualPropertiesHash.h"

#include "XnActualIntProperty.h"
#include "XnActualRealProperty.h"
#include "XnActualStringProperty.h"
#include "XnActualGeneralProperty.h"

XnActualPropertiesHash::XnActualPropertiesHash(const XnChar* strName)
{
	strncpy(m_strName, strName, XN_DEVICE_MAX_STRING_LENGTH);
}

XnActualPropertiesHash::~XnActualPropertiesHash()
{
	// free all properties
	for (Iterator it = Begin(); it != End(); ++it)
	{
		XN_DELETE(it->Value());
	}
}

XnStatus XnActualPropertiesHash::Add(const XnChar* strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	Iterator it = End();
	if (XN_STATUS_OK == Find(strName, it))
	{
		return XN_STATUS_DEVICE_PROPERTY_ALREADY_EXISTS;
	}

	// create property
	XnActualIntProperty* pProp;
	XN_VALIDATE_NEW(pProp, XnActualIntProperty, strName, nValue, m_strName);

	// and add it to the hash
	nRetVal = m_Hash.Set(strName, pProp);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pProp);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnActualPropertiesHash::Add(const XnChar* strName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	Iterator it = End();
	if (XN_STATUS_OK == Find(strName, it))
	{
		return XN_STATUS_DEVICE_PROPERTY_ALREADY_EXISTS;
	}

	// create property
	XnActualRealProperty* pProp;
	XN_VALIDATE_NEW(pProp, XnActualRealProperty, strName, dValue, m_strName);

	// and add it to the hash
	nRetVal = m_Hash.Set(strName, pProp);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pProp);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnActualPropertiesHash::Add(const XnChar* strName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	Iterator it = End();
	if (XN_STATUS_OK == Find(strName, it))
	{
		return XN_STATUS_DEVICE_PROPERTY_ALREADY_EXISTS;
	}

	// create property
	XnActualStringProperty* pProp;
	XN_VALIDATE_NEW(pProp, XnActualStringProperty, strName, strValue, m_strName);

	// and add it to the hash
	nRetVal = m_Hash.Set(strName, pProp);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pProp);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnActualPropertiesHash::Add(const XnChar* strName, const XnGeneralBuffer& gbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	Iterator it = End();
	if (XN_STATUS_OK == Find(strName, it))
	{
		return XN_STATUS_DEVICE_PROPERTY_ALREADY_EXISTS;
	}

	// create buffer
	XnGeneralBuffer gbNew;
	nRetVal = XnGeneralBufferAlloc(&gbNew, gbValue.nDataSize);
	XN_IS_STATUS_OK(nRetVal);

	// copy content
	xnOSMemCopy(gbNew.pData, gbValue.pData, gbValue.nDataSize);

	// create property
	XnActualGeneralProperty* pProp;
	XN_VALIDATE_NEW(pProp, XnActualGeneralProperty, strName, gbNew, NULL, m_strName);

	pProp->SetAsBufferOwner(TRUE);

	// and add it to the hash
	nRetVal = m_Hash.Set(strName, pProp);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pProp);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnActualPropertiesHash::Remove(const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	ConstIterator it;
	nRetVal = Find(strName, it);
	XN_IS_STATUS_OK(nRetVal);

	return Remove(it);
}

XnStatus XnActualPropertiesHash::Remove(ConstIterator where)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnProperty* pProp = where->Value();

	nRetVal = m_Hash.Remove(where);
	XN_IS_STATUS_OK(nRetVal);

	XN_DELETE(pProp);

	return (XN_STATUS_OK);
}

XnStatus XnActualPropertiesHash::Clear()
{
	while (!IsEmpty())
	{
		Remove(Begin());
	}

	return XN_STATUS_OK;
}

XnStatus XnActualPropertiesHash::CopyFrom(const XnActualPropertiesHash& other)
{
	XnStatus nRetVal = XN_STATUS_OK;

	Clear();
	strncpy(m_strName, other.m_strName, XN_DEVICE_MAX_STRING_LENGTH);

	for (ConstIterator it = other.Begin(); it != other.End(); ++it)
	{
		switch (it->Value()->GetType())
		{
		case XN_PROPERTY_TYPE_INTEGER:
			{
				XnActualIntProperty* pProp = (XnActualIntProperty*)it->Value();
				nRetVal = Add(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_REAL:
			{
				XnActualRealProperty* pProp = (XnActualRealProperty*)it->Value();
				nRetVal = Add(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_STRING:
			{
				XnActualStringProperty* pProp = (XnActualStringProperty*)it->Value();
				nRetVal = Add(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_GENERAL:
			{
				XnActualGeneralProperty* pProp = (XnActualGeneralProperty*)it->Value();
				nRetVal = Add(pProp->GetName(), pProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Unknown property type: %d\n", it->Value()->GetType());
		}
	}

	return (XN_STATUS_OK);
}
