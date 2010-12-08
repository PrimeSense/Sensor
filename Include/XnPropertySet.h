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





#ifndef __XN_PROPERTY_SET_H__
#define __XN_PROPERTY_SET_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnGeneralBuffer.h>
#include <XnPlatform.h>
#include <XnDDK.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/** The type of the property. */
typedef enum XnPropertyType
{
	XN_PROPERTY_TYPE_INTEGER,
	XN_PROPERTY_TYPE_REAL,
	XN_PROPERTY_TYPE_STRING,
	XN_PROPERTY_TYPE_GENERAL,
} XnPropertyType;

struct XnPropertySet; // Forward Declaration
typedef struct XnPropertySet XnPropertySet;

struct XnPropertySetModuleEnumerator; // Forward Declaration
typedef struct XnPropertySetModuleEnumerator XnPropertySetModuleEnumerator;

struct XnPropertySetEnumerator; // Forward Declaration
typedef struct XnPropertySetEnumerator XnPropertySetEnumerator;

//---------------------------------------------------------------------------
// Exported functions
//---------------------------------------------------------------------------

/**
* Creates a new property set.
* 
* @param	ppSet			[out]		A pointer to the new set.
*/
XN_DDK_API XnStatus XnPropertySetCreate(XnPropertySet** ppSet);

/**
* Destroys a previously created property set.
* 
* @param	ppSet			[in/out]	A pointer to the set.
*/
XN_DDK_API XnStatus XnPropertySetDestroy(XnPropertySet** ppSet);

/**
* Clears a property set from all the properties.
* 
* @param	pSet			[in]		The property set.
*/
XN_DDK_API XnStatus XnPropertySetClear(XnPropertySet* pSet);

/**
* Adds a module to the property set.
* 
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module to add.
*/
XN_DDK_API XnStatus XnPropertySetAddModule(XnPropertySet* pSet, const XnChar* strModuleName);

/**
* Removes a module from the property set.
* 
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module to remove.
*/
XN_DDK_API XnStatus XnPropertySetRemoveModule(XnPropertySet* pSet, const XnChar* strModuleName);

/**
* Adds an integer property to the property set.
* 
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	nValue			[in]		Value for that property.
*/
XN_DDK_API XnStatus XnPropertySetAddIntProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, XnUInt64 nValue);

/**
* Adds an real property to the property set.
* 
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	dValue			[in]		Value for that property.
*/
XN_DDK_API XnStatus XnPropertySetAddRealProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, XnDouble dValue);

/**
* Adds an string property to the property set.
* 
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	strValue			[in]		Value for that property.
*/
XN_DDK_API XnStatus XnPropertySetAddStringProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, const XnChar* strValue);

/**
* Adds an general property to the property set.
* 
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	pgbValue			[in]		Value for that property.
*/
XN_DDK_API XnStatus XnPropertySetAddGeneralProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty, const XnGeneralBuffer* pgbValue);

/**
* Removes a property from the property set.
* 
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to remove.
*/
XN_DDK_API XnStatus XnPropertySetRemoveProperty(XnPropertySet* pSet, const XnChar* strModuleName, const XnChar* strProperty);

/**
* Gets a modules enumerator. This enumerator should be freed using XnPropertySetModuleEnumeratorFree.
* 
* @param	pSet			[in]		The property set.
* @param	ppEnumerator	[out]		The created enumerator.
*/
XN_DDK_API XnStatus XnPropertySetGetModuleEnumerator(const XnPropertySet* pSet, XnPropertySetModuleEnumerator** ppEnumerator);

/**
* Frees a previously created module enumerator.
* 
* @param	ppEnumerator	[in/out]	The enumerator.
*/
XN_DDK_API XnStatus XnPropertySetModuleEnumeratorFree(XnPropertySetModuleEnumerator** ppEnumer);

/**
* Moves the enumerator to the next module. This function must be called *before* getting current.
* 
* @param	pEnumerator		[in]		The enumerator.
* @param	pbEnd			[out]		TRUE if the enumerator has reached the end of the collection.
*/
XN_DDK_API XnStatus XnPropertySetModuleEnumeratorMoveNext(XnPropertySetModuleEnumerator* pEnumerator, XnBool* pbEnd);

/**
* Gets the current module name from the enumerator.
* 
* @param	pEnumerator		[in]		The enumerator.
* @param	pstrModuleName	[out]		The name of the current module.
*/
XN_DDK_API XnStatus XnPropertySetModuleEnumeratorGetCurrent(const XnPropertySetModuleEnumerator* pEnumer, const XnChar** pstrModuleName);

/**
* Gets a property enumerator. This enumerator must be freed using XnPropertySetEnumeratorFree.
* 
* @param	pSet			[in]			The property set.
* @param	ppEnumerator	[in/out]		The enumerator.
* @param	strModule		[in]			Optional. When provided, only properties of this module will be enumerated.
*/
XN_DDK_API XnStatus XnPropertySetGetEnumerator(const XnPropertySet* pSet, XnPropertySetEnumerator** ppEnumerator, const XnChar* strModule = NULL);

/**
* Finds a property according to its name and module, and returns an enumerator to it.
* This enumerator must be freed using XnPropertySetEnumeratorFree.
*
* @param	pSet			[in]			The property set.
* @param	strModule		[in]			The module name.
* @param	strProp			[in]			The property name.
* @param	ppEnumerator	[in/out]		The enumerator.
*/
XN_DDK_API XnStatus XnPropertySetFindProperty(const XnPropertySet* pSet, const XnChar* strModule, const XnChar* strProp, XnPropertySetEnumerator** ppEnumerator);

/**
* Frees a previously created properties enumerator.
* 
* @param	ppEnumerator	[in/out]	The enumerator.
*/
XN_DDK_API XnStatus XnPropertySetEnumeratorFree(XnPropertySetEnumerator** ppEnumerator);

/**
* Moves the enumerator to the next property. This function must be called *before* getting current.
* 
* @param	pEnumerator		[in]		The enumerator.
* @param	pbEnd			[out]		TRUE if the enumerator has reached the end of the collection.
*/
XN_DDK_API XnStatus XnPropertySetEnumeratorMoveNext(XnPropertySetEnumerator* pEnumerator, XnBool* pbEnd);

/**
* Gets information regarding the current property.
* 
* @param	pEnumerator		[in]		The enumerator.
* @param	pnType			[out]		The type of the current property.
* @param	pstrModule		[out]		The module of the current property.
* @param	pstrProp		[out]		The name of the current property.
*/
XN_DDK_API XnStatus XnPropertySetEnumeratorGetCurrentPropertyInfo(const XnPropertySetEnumerator* pEnumerator, XnPropertyType* pnType, const XnChar** pstrModule, const XnChar** pstrProp);

/**
* Gets the current integer value.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pnValue				[out]		The value of the property.
*/
XN_DDK_API XnStatus XnPropertySetEnumeratorGetIntValue(const XnPropertySetEnumerator* pEnumerator, XnUInt64* pnValue);

/**
* Gets the current real property.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pdValue				[out]		The value of the property.
*/
XN_DDK_API XnStatus XnPropertySetEnumeratorGetRealValue(const XnPropertySetEnumerator* pEnumerator, XnDouble* pdValue);

/**
* Gets the current string property.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pstrValue			[out]		The value of the property.
*/
XN_DDK_API XnStatus XnPropertySetEnumeratorGetStringValue(const XnPropertySetEnumerator* pEnumerator, const XnChar** pstrValue);

/**
* Gets the current general property.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pgbValue			[out]		The value of the property.
*/
XN_DDK_API XnStatus XnPropertySetEnumeratorGetGeneralValue(const XnPropertySetEnumerator* pEnumerator, XnGeneralBuffer* pgbValue);

#endif //__XN_PROPERTY_SET_H__
