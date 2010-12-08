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

	inline Iterator begin() { return m_Hash.begin(); }
	inline ConstIterator begin() const { return m_Hash.begin(); }
	inline Iterator end() { return m_Hash.end(); }
	inline ConstIterator end() const { return m_Hash.end(); }

	XnStatus CopyFrom(const XnActualPropertiesHash& other);

protected:
	XnPropertiesHash m_Hash;
	XnChar m_strName[XN_DEVICE_MAX_STRING_LENGTH];
};

#endif //__XN_ACTUAL_PROPERTIES_HASH_H__
