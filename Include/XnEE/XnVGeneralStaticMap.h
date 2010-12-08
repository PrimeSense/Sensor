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






#ifndef _XNV_GENERAL_STATIC_MAP_H_
#define _XNV_GENERAL_STATIC_MAP_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVDepthMap.h"
#include "XnVDevice.h"
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * This is a general StaticMap base class.
 * It implements the Save/Load functionality, as well as the Clean,
 * which applies the internal XnVDepthMap instance to the input XnVDepthMap.
 * It also implements some basic Reset and AddMap capabilities, as a framework 
 * for the specific sub classes, which will implement the specific AddMap and Reset
 * relevant to the specific StaticMap implementation.
 */

class XN_EE_FW_API XnVGeneralStaticMap
{
public:
	XnVGeneralStaticMap(const XnChar* strINIFile = NULL);
	virtual ~XnVGeneralStaticMap();

	/**
	* Load a static map from file. The file name is given in the INI file supplied to the ctor.
	* Static maps are saved in QVGA/VGA resolutions, and therefore can only be loaded in these resolutions.
	*
	* @param	[in]	nResolution	The resolution.
	*/
	virtual XnStatus Load(XnUInt32 nResolution);
	virtual XnStatus Load(XnResolutions nResolution);
	virtual XnStatus Load(const XnChar* strFileName);

	/**
	* Save the current static map to 2 files: 1 for VGA and 1 for QVGA.
	* The file names are given in the INI file supplied to the ctor.
	* Saving in other resolutions will result in failure.
	*/
	virtual XnStatus Save();
	virtual XnStatus Save(const XnChar* strFileName);
	
	/**
	* Discard the current static map.
	*/
	XnStatus Reset();

	/**
	* Each specific type of static map will have different things to do when adding a new map. This is the place for it.
	*
	* @param	[in]	dmMap	A depth map to add to the static.
	*/
	virtual XnStatus AddMap(const XnVDepthMap& dmMap) = 0;
	
	/**
	* Clean the current depth map according to the static map.
	* If resolutions don't match, the function will fail.
	*
	* @param	[in,out]	dmMap	A depth map on which to apply the static map.
	*
	* @return XN_STATUS_OK on success, XNV_STATUS_RESOLUTION_MISMATCH if resolution doesn't match learned static.
	*/
	virtual XnStatus Clean(XnVDepthMap& dmMap);

	/**
	* Get the internal depth map. See the learned values per pixel in the static map.
	*/
	XnVDepthMap* GetDepthMap() const;
protected:
	/**
	* Each specific type of static map will have different things to do when resetting. This is the place for it.
	*/
	virtual XnStatus DoReset() = 0;

	/**
	* Do all general things needed before adding a new map. This includes checking resolutions match (and resetting if not)
	* Specific AddMap functions should call this at the beginning.
	*
	* @param	[in]	dmMap	A depth map to add to the static.
	*/
	XnStatus PreAddMap(const XnVDepthMap& dmMap);

	virtual XnStatus Init(XnUInt32 nNewSize) = 0;

	XnVDepthMap* m_pdmStaticMap;
	XnBool m_bInit;

	XnVDevice m_Device;

	XnChar m_strVGAFile[80];
	XnChar m_strQVGAFile[80];
	XnBool m_bVGAExists, m_bQVGAExists;
};

#endif //_XNV_GENERAL_STATIC_MAP_H_

