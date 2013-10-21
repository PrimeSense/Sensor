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
