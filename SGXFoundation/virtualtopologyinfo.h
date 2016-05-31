///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///
#pragma once
#ifndef SYNGLYPHX_VIRTUALTOPOLOGYINFO_H
#define SYNGLYPHX_VIRTUALTOPOLOGYINFO_H

#include "sgxfoundation.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXFOUNDATION_API VirtualTopologyInfo
	{
	public:
		enum Type {
			Null = 0,		// linear 3D euclidean space
			CubePlane,			//six facet coord system for each side of cube
			SphereNonZeroRadius,			//spherical coords compatible with KML
			Circle,			//default branchLevel = 1 attached to pin
			CylinderSide,
			LinePin,				//default root pin shaped as icecream cone
			LineRod,
			SphereZeroRadius			//zero origin offset with spherical coords
		};

		typedef boost::bimap<Type, std::wstring> TypeStringBimap;

		VirtualTopologyInfo();
		~VirtualTopologyInfo();

		static const TypeStringBimap s_virtualTopologyNames;
	};

	//This translator is so that VirtualTopologyInfo::Type can be automatically used by boost::property_tree
	class SGXFOUNDATION_API VirtualTopologyTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef VirtualTopologyInfo::Type external_type;

		VirtualTopologyTranslator();

		boost::optional<VirtualTopologyInfo::Type> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(VirtualTopologyInfo::Type const& v);

	private:

	};

} //namespace SynGlyphX

namespace boost {

	namespace property_tree {

		template<>
		struct translator_between < std::wstring, SynGlyphX::VirtualTopologyInfo::Type >
		{
			typedef SynGlyphX::VirtualTopologyTranslator type;
		};
	}
}  //namespace boost

#endif //SYNGLYPHX_VIRTUALTOPOLOGYINFO_H