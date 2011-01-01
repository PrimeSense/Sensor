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
#include "XnPropertySetInternal.h"
#include <XnList.h>
#include <XnStringsHash.h>
#include "XnActualIntProperty.h"
#include "XnActualRealProperty.h"
#include "XnActualStringProperty.h"
#include "XnActualGeneralProperty.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnPropertySetModuleEnumerator
{
	XnBool bFirst;
	XnPropertySetData* pModules;
	XnPropertySetData::ConstIterator it;
};

struct XnPropertySetEnumerator
{
	XnBool bFirst;
	XnPropertySetData* pModules;
	XnPropertySetData::ConstIterator itModule;
	XnChar strModule[XN_DEVICE_MAX_STRING_LENGTH];
	XnActualPropertiesHash::ConstIterator itProp;
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_DDK_API XnStatus XnPropertySetCreate(XnPropertySet** ppSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_OUTPUT_PTR(ppSet);

	XnPropertySet* pSet;
	XN_VALIDATE_ALLOC(pSet, XnPropertySet);

	pSet->pData = XN_NEW(XnPropertySetData);
	if (pSet->pData == NULL)
	{
		xnOSFree(pSet);
		return XN_STATUS_ALLOC_FAILED;
	}

	*ppSet = pSet;
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetDestroy(XnPropertySet** ppSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(ppSet);
	XN_VALIDATE_INPUT_PTR(*ppSet);

	XnPropertySet* pSet = (*ppSet);

	if (pSet->pData != NULL)
	{
		XnPropertySetClear(pSet);
		XN_DELETE(pSet->pData);
	}

	xnOSFree(pSet);

	*ppSet = NULL;
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetClear(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSet);

	while (!pSet->pData->IsEmpty())
	{
		nRetVal = XnPropertySetRemoveModule(pSet, pSet->pData->begin().Key());
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetAddModule(XnPropertySet* pSet, const XnChar* strModuleName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModuleName);

	XnActualPropertiesHash* pModule = NULL;

	// make sure this module doesn't already exists
	nRetVal = pSet->pData->Get(strModuleName, pModule);
	if (XN_STATUS_OK == nRetVal)
	{
		return XN_STATUS_DEVICE_MODULE_ALREADY_EXISTS;
	}

	// doesn't exist. create a new one
	XN_VALIDATE_NEW(pModule, XnActualPropertiesHash, strModuleName);

	nRetVal = XnPropertySetDataAttachModule(pSet->pData, strModuleName, pModule);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pModule);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetRemoveModule(XnPropertySet* pSet, const XnChar* strModuleName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModuleName);

	XnActualPropertiesHash* pModule = NULL;
	nRetVal = XnPropertySetDataDetachModule(pSet->pData, strModuleName, &pModule);
	XN_IS_STATUS_OK(nRetVal);

	// now free its memory
	XN_DELETE(pModule);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetAddIntProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModuleName);
	XN_VALIDATE_INPUT_PTR(strProperty);

	// get module
	XnActualPropertiesHash* pModule;
	nRetVal = pSet->pData->Get(strModuleName, pModule);
	XN_IS_STATUS_OK(nRetVal);

	// add property
	nRetVal = pModule->Add(strProperty, nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetAddRealProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModuleName);
	XN_VALIDATE_INPUT_PTR(strProperty);

	// get module
	XnActualPropertiesHash* pModule;
	nRetVal = pSet->pData->Get(strModuleName, pModule);
	XN_IS_STATUS_OK(nRetVal);

	// add property
	nRetVal = pModule->Add(strProperty, dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetAddStringProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModuleName);
	XN_VALIDATE_INPUT_PTR(strProperty);
	XN_VALIDATE_INPUT_PTR(strValue);

	// get module
	XnActualPropertiesHash* pModule;
	nRetVal = pSet->pData->Get(strModuleName, pModule);
	XN_IS_STATUS_OK(nRetVal);

	// add property
	nRetVal = pModule->Add(strProperty, strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetAddGeneralProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, const XnGeneralBuffer* pgbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModuleName);
	XN_VALIDATE_INPUT_PTR(strProperty);
	XN_VALIDATE_INPUT_PTR(pgbValue);

	// get module
	XnActualPropertiesHash* pModule;
	nRetVal = pSet->pData->Get(strModuleName, pModule);
	XN_IS_STATUS_OK(nRetVal);

	// add property
	nRetVal = pModule->Add(strProperty, *pgbValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetRemoveProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModuleName);
	XN_VALIDATE_INPUT_PTR(strProperty);

	// get module
	XnActualPropertiesHash* pModule;
	nRetVal = pSet->pData->Get(strModuleName, pModule);
	XN_IS_STATUS_OK(nRetVal);

	// remove the property
	XnProperty* pProp;
	nRetVal = pModule->Remove(strProperty);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetGetModuleEnumerator(const XnPropertySet* pSet, XnPropertySetModuleEnumerator** ppEnumerator)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_OUTPUT_PTR(ppEnumerator);

	XnPropertySetModuleEnumerator* pEnumer;
	XN_VALIDATE_ALLOC(pEnumer, XnPropertySetModuleEnumerator);

	pEnumer->bFirst = TRUE;
	pEnumer->it = pSet->pData->end();
	pEnumer->pModules = pSet->pData;

	*ppEnumerator = pEnumer;

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetModuleEnumeratorFree(XnPropertySetModuleEnumerator** ppEnumer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(ppEnumer);
	XN_VALIDATE_INPUT_PTR(*ppEnumer);
	
	xnOSFree(*ppEnumer);
	*ppEnumer = NULL;

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetModuleEnumeratorMoveNext(XnPropertySetModuleEnumerator* pEnumerator, XnBool* pbEnd)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pEnumerator);
	XN_VALIDATE_OUTPUT_PTR(pbEnd);

	if (pEnumerator->bFirst)
	{
		pEnumerator->it = pEnumerator->pModules->begin();
		pEnumerator->bFirst = FALSE;
	}
	else if (pEnumerator->it == pEnumerator->pModules->end())
	{
		return XN_STATUS_ILLEGAL_POSITION;
	}
	else
	{
		pEnumerator->it++;
	}

	*pbEnd = (pEnumerator->it == pEnumerator->pModules->end());
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetModuleEnumeratorGetCurrent(const XnPropertySetModuleEnumerator* pEnumer, const XnChar** pstrModuleName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pEnumer);
	XN_VALIDATE_OUTPUT_PTR(pstrModuleName);
	
	if (pEnumer->it == pEnumer->pModules->end())
	{
		return XN_STATUS_ILLEGAL_POSITION;
	}

	*pstrModuleName = pEnumer->it.Key();

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetGetEnumerator(const XnPropertySet* pSet, XnPropertySetEnumerator** ppEnumerator, const XnChar* strModule /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_OUTPUT_PTR(ppEnumerator);

	if (strModule != NULL)
	{
		// make sure module exists
		XnPropertySetData::ConstIterator it = pSet->pData->end();
		nRetVal = pSet->pData->Find(strModule, it);
		XN_IS_STATUS_OK(nRetVal);
	}

	XnPropertySetEnumerator* pEnumer;
	XN_VALIDATE_ALLOC(pEnumer, XnPropertySetEnumerator);

	pEnumer->bFirst = TRUE;
	pEnumer->pModules = pSet->pData;
	if (strModule != NULL)
	{
		strncpy(pEnumer->strModule, strModule, XN_DEVICE_MAX_STRING_LENGTH);
	}
	else
	{
		pEnumer->strModule[0] = '\0';
	}
	
	*ppEnumerator = pEnumer;

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetFindProperty(const XnPropertySet* pSet, const XnChar* strModule, const XnChar* strProp, XnPropertySetEnumerator** ppEnumerator)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSet);
	XN_VALIDATE_INPUT_PTR(strModule);
	XN_VALIDATE_INPUT_PTR(strProp);
	XN_VALIDATE_OUTPUT_PTR(ppEnumerator);

	// find module
	XnPropertySetData::Iterator itModule = pSet->pData->end();
	nRetVal = pSet->pData->Find(strModule, itModule);
	XN_IS_STATUS_OK(nRetVal);

	XnActualPropertiesHash* pModule = itModule.Value();

	// find property
	XnActualPropertiesHash::Iterator itProp = pModule->end();
	nRetVal = pModule->Find(strProp, itProp);
	XN_IS_STATUS_OK(nRetVal);

	// create enumerator
	XnPropertySetEnumerator* pEnumer;
	XN_VALIDATE_ALLOC(pEnumer, XnPropertySetEnumerator);

	pEnumer->itModule = itModule;
	pEnumer->itProp = itProp;
	pEnumer->pModules = pSet->pData;
	pEnumer->strModule[0] = '\0';
	pEnumer->bFirst = FALSE;

	*ppEnumerator = pEnumer;

	return XN_STATUS_OK;
}

XN_DDK_API XnStatus XnPropertySetEnumeratorFree(XnPropertySetEnumerator** ppEnumerator)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(ppEnumerator);
	XN_VALIDATE_INPUT_PTR(*ppEnumerator);

	xnOSFree(*ppEnumerator);
	*ppEnumerator = NULL;
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetEnumeratorMoveNext(XnPropertySetEnumerator* pEnumerator, XnBool* pbEnd)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pEnumerator);
	XN_VALIDATE_OUTPUT_PTR(pbEnd);

	*pbEnd = TRUE;

	if (pEnumerator->strModule[0] != '\0') // single module
	{
		if (pEnumerator->bFirst)
		{
			pEnumerator->bFirst = FALSE;

			// find this module
			nRetVal = pEnumerator->pModules->Find(pEnumerator->strModule, pEnumerator->itModule);
			if (nRetVal == XN_STATUS_NO_MATCH)
			{
				pEnumerator->itModule = pEnumerator->pModules->end();
			}
			XN_IS_STATUS_OK(nRetVal);

			pEnumerator->itProp = pEnumerator->itModule.Value()->begin();
		}
		else if (pEnumerator->itProp == pEnumerator->itModule.Value()->end())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}
		else
		{
			// advance prop iterator
			pEnumerator->itProp++;
		}

		*pbEnd = (pEnumerator->itProp == pEnumerator->itModule.Value()->end());
	}
	else // all modules
	{
		if (pEnumerator->bFirst)
		{
			pEnumerator->bFirst = FALSE;

			// search for the first modules that has properties
			pEnumerator->itModule = pEnumerator->pModules->begin();
			while (pEnumerator->itModule != pEnumerator->pModules->end() && pEnumerator->itModule.Value()->IsEmpty())
			{
				pEnumerator->itModule++;
			}

			// if we found one, take it's first property
			if (pEnumerator->itModule != pEnumerator->pModules->end())
			{
				pEnumerator->itProp = pEnumerator->itModule.Value()->begin();
				*pbEnd = FALSE;
			}
			else
			{
				*pbEnd = TRUE;
			}
		}
		else if (pEnumerator->itModule == pEnumerator->pModules->end())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}
		else
		{
			// move to next one
			pEnumerator->itProp++;

			// check if we reached end of module
			if (pEnumerator->itProp == pEnumerator->itModule.Value()->end())
			{
				// move to next module with properties
				do
				{
					pEnumerator->itModule++;
				}
				while (pEnumerator->itModule != pEnumerator->pModules->end() && pEnumerator->itModule.Value()->IsEmpty());

				// if we found one, take it's first property
				if (pEnumerator->itModule != pEnumerator->pModules->end())
				{
					pEnumerator->itProp = pEnumerator->itModule.Value()->begin();
					*pbEnd = FALSE;
				}
				else
				{
					*pbEnd = TRUE;
				}
			}
			else
			{
				*pbEnd = FALSE;
			}
		}
	}

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetEnumeratorGetCurrentPropertyInfo(const XnPropertySetEnumerator* pEnumerator, XnPropertyType* pnType, const XnChar** pstrModule, const XnChar** pstrProp)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pEnumerator);
	XN_VALIDATE_OUTPUT_PTR(pnType);
	XN_VALIDATE_OUTPUT_PTR(pstrModule);
	XN_VALIDATE_OUTPUT_PTR(pstrProp);
	
	XnProperty* pProp = pEnumerator->itProp.Value();
	*pnType = pProp->GetType();
	*pstrModule = pProp->GetModule();
	*pstrProp = pProp->GetName();
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetEnumeratorGetIntValue(const XnPropertySetEnumerator* pEnumerator, XnUInt64* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pEnumerator);
	XN_VALIDATE_OUTPUT_PTR(pnValue);

	XnProperty* pPropBase = pEnumerator->itProp.Value();
	if (pPropBase->GetType() != XN_PROPERTY_TYPE_INTEGER)
	{
		return XN_STATUS_DEVICE_PROPERTY_BAD_TYPE;
	}

	XnActualIntProperty* pProp = (XnActualIntProperty*)pPropBase;
	*pnValue = pProp->GetValue();

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetEnumeratorGetRealValue(const XnPropertySetEnumerator* pEnumerator, XnDouble* pdValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pEnumerator);
	XN_VALIDATE_OUTPUT_PTR(pdValue);

	XnProperty* pPropBase = pEnumerator->itProp.Value();
	if (pPropBase->GetType() != XN_PROPERTY_TYPE_REAL)
	{
		return XN_STATUS_DEVICE_PROPERTY_BAD_TYPE;
	}

	XnActualRealProperty* pProp = (XnActualRealProperty*)pPropBase;
	*pdValue = pProp->GetValue();

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetEnumeratorGetStringValue(const XnPropertySetEnumerator* pEnumerator, const XnChar** pstrValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pEnumerator);
	XN_VALIDATE_OUTPUT_PTR(pstrValue);

	XnProperty* pPropBase = pEnumerator->itProp.Value();
	if (pPropBase->GetType() != XN_PROPERTY_TYPE_STRING)
	{
		return XN_STATUS_DEVICE_PROPERTY_BAD_TYPE;
	}

	XnActualStringProperty* pProp = (XnActualStringProperty*)pPropBase;
	*pstrValue = pProp->GetValue();

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetEnumeratorGetGeneralValue(const XnPropertySetEnumerator* pEnumerator, XnGeneralBuffer* pgbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pEnumerator);
	XN_VALIDATE_OUTPUT_PTR(pgbValue);

	XnProperty* pPropBase = pEnumerator->itProp.Value();
	if (pPropBase->GetType() != XN_PROPERTY_TYPE_GENERAL)
	{
		return XN_STATUS_DEVICE_PROPERTY_BAD_TYPE;
	}

	XnActualGeneralProperty* pProp = (XnActualGeneralProperty*)pPropBase;
	*pgbValue = pProp->GetValue();

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetDataAttachModule(XnPropertySetData* pSetData, const XnChar* strModuleName, XnActualPropertiesHash* pModule)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSetData);
	XN_VALIDATE_INPUT_PTR(strModuleName);
	XN_VALIDATE_INPUT_PTR(pModule);

	nRetVal = pSetData->Set(strModuleName, pModule);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetDataDetachModule(XnPropertySetData* pSetData, const XnChar* strModuleName, XnActualPropertiesHash** ppModule)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pSetData);
	XN_VALIDATE_INPUT_PTR(strModuleName);
	XN_VALIDATE_OUTPUT_PTR(ppModule);
	
	// remove it
	nRetVal = pSetData->Remove(strModuleName, *ppModule);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_DDK_API XnStatus XnPropertySetCloneModule(const XnPropertySet* pSource, XnPropertySet* pDest, const XnChar* strModule, const XnChar* strNewName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnActualPropertiesHash* pModuleProps;
	nRetVal = pSource->pData->Get(strModule, pModuleProps);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddModule(pDest, strNewName);
	XN_IS_STATUS_OK(nRetVal);

	for (XnActualPropertiesHash::ConstIterator it = pModuleProps->begin(); it != pModuleProps->end(); ++it)
	{
		XnProperty* pProp = it.Value();
		switch (pProp->GetType())
		{
		case XN_PROPERTY_TYPE_INTEGER:
			{
				XnActualIntProperty* pIntProp = (XnActualIntProperty*)pProp;
				nRetVal = XnPropertySetAddIntProperty(pDest, strNewName, pIntProp->GetName(), pIntProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_REAL:
			{
				XnActualRealProperty* pRealProp = (XnActualRealProperty*)pProp;
				nRetVal = XnPropertySetAddRealProperty(pDest, strNewName, pRealProp->GetName(), pRealProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_STRING:
			{
				XnActualStringProperty* pStrProp = (XnActualStringProperty*)pProp;
				nRetVal = XnPropertySetAddStringProperty(pDest, strNewName, pStrProp->GetName(), pStrProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		case XN_PROPERTY_TYPE_GENERAL:
			{
				XnActualGeneralProperty* pGenProp = (XnActualGeneralProperty*)pProp;
				nRetVal = XnPropertySetAddGeneralProperty(pDest, strNewName, pGenProp->GetName(), &pGenProp->GetValue());
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
		default:
			XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DDK, "Unknown property type: %d", pProp->GetType());
		}
	}
	
	return (XN_STATUS_OK);
}
