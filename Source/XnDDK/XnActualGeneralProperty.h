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





#ifndef __XN_ACTUAL_GENERAL_PROPERTY_H__
#define __XN_ACTUAL_GENERAL_PROPERTY_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDDK/XnGeneralProperty.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type general which actually holds a value.
*/
class XN_DDK_CPP_API XnActualGeneralProperty : public XnGeneralProperty
{
public:
	XnActualGeneralProperty(const XnChar* strName, void* pData, XnUInt32 nDataSize, ReadValueFromFileFuncPtr pReadFromFileFunc = NULL, const XnChar* strModule = "");
	XnActualGeneralProperty(const XnChar* strName, const XnGeneralBuffer& gbValue, ReadValueFromFileFuncPtr pReadFromFileFunc = NULL, const XnChar* strModule = "");
	~XnActualGeneralProperty();

	void SetAsBufferOwner(XnBool bOwner);

	inline const XnGeneralBuffer& GetValue() const { return m_gbValue; }

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		XnGeneralProperty::UpdateSetCallback((XnGeneralProperty::SetFuncPtr)pFunc, pCookie);
	}

	inline void UpdateSetCallbackToDefault()
	{
		UpdateSetCallback(SetCallback, this);
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		XnGeneralProperty::UpdateGetCallback((XnGeneralProperty::GetFuncPtr)pFunc, pCookie);
	}

	inline void ReplaceBuffer(void* pData, XnUInt32 nDataSize)
	{
		m_gbValue.pData = pData;
		m_gbValue.nDataSize = nDataSize;
	}

	XnStatus AddToPropertySet(XnPropertySet* pSet);

private:
	static XnStatus XN_CALLBACK_TYPE SetCallback(XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCallback(const XnActualGeneralProperty* pSender, const XnGeneralBuffer& gbValue, void* pCookie);

	XnGeneralBuffer m_gbValue;
	XnBool m_bOwner;
};

#endif //__XN_ACTUAL_GENERAL_PROPERTY_H__
