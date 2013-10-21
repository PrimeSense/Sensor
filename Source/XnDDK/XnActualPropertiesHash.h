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
#ifndef __XN_ACTUAL_PROPERTIES_HASH_H__
#define __XN_ACTUAL_PROPERTIES_HASH_H__

#include "XnProperty.h"
#include "XnActualIntProperty.h"
#include "XnActualRealProperty.h"
#include "XnActualStringProperty.h"
#include "XnActualGeneralProperty.h"

/** 
* A hash table of actual properties. The user can safely assume that every property in this
* hash is actual.
*/
class XN_DDK_CPP_API XnActualPropertiesHash
{
public:
	XnActualPropertiesHash(const XnChar* strName);
	~XnActualPropertiesHash();

	typedef XnPropertiesHash::Iterator Iterator;
	typedef XnPropertiesHash::ConstIterator ConstIterator;

	XnStatus Add(const XnChar* strName, XnUInt64 nValue);
	XnStatus Add(const XnChar* strName, XnDouble dValue);
	XnStatus Add(const XnChar* strName, const XnChar* strValue);
	XnStatus Add(const XnChar* strName, const XnGeneralBuffer& gbValue);

	XnStatus Remove(const XnChar* strName);
	XnStatus Remove(ConstIterator where);
	inline XnBool IsEmpty() const { return m_Hash.IsEmpty(); }
	XnStatus Clear();

	inline XnStatus Find(const XnChar* strName, Iterator& iter) { return m_Hash.Find(strName, iter); }
	inline XnStatus Find(const XnChar* strName, ConstIterator& iter) const { return m_Hash.Find(strName, iter); }
	inline XnStatus Get(const XnChar* strName, XnProperty*& pProp) const { return m_Hash.Get(strName, pProp); }

	inline Iterator Begin() { return m_Hash.Begin(); }
	inline ConstIterator Begin() const { return m_Hash.Begin(); }
	inline Iterator End() { return m_Hash.End(); }
	inline ConstIterator End() const { return m_Hash.End(); }

	XnStatus CopyFrom(const XnActualPropertiesHash& other);

protected:
	XnPropertiesHash m_Hash;
	XnChar m_strName[XN_DEVICE_MAX_STRING_LENGTH];
};

#endif //__XN_ACTUAL_PROPERTIES_HASH_H__
