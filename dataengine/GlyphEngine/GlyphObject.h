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
#ifndef GLYPHOBJECT_H
#define GLYPHOBJECT_H

#include "GlyphEngine_Exports.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/uuid/uuid_io.hpp>


namespace GlyphEngine {

	struct Placement {
		std::wstring min, diff, bind, type;
		void create(std::wstring m, std::wstring d, std::wstring b, std::wstring t)
		{
			min = m;
			diff = d;
			bind = b;
			type = t;
		}
	};

	struct Position {
		Placement x, y, z;
	};

	struct Color {
		Placement rgb, tp;
	};

	struct Geometry {
		std::wstring value, type, surface, ratio;
		Geometry(std::wstring v, std::wstring t, std::wstring s, std::wstring r) :
			value(v), type(t), surface(s), ratio(r)
		{}
	};

	class GLYPHENGINE GlyphObject
	{
	public:

		GlyphObject(const boost::property_tree::wptree& propertyTree);
		int GetLabel();

	private:
		void ImportPosition(const boost::property_tree::wptree& positionPropertyTree);
		void ImportScale(const boost::property_tree::wptree& scalePropertyTree);
		void ImportColor(const boost::property_tree::wptree& colorPropertyTree);
		void ImportTag(const boost::property_tree::wptree& tagPropertyTree);
		void ImportGeometry(const boost::property_tree::wptree& geometryPropertyTree);

		int m_label;
		std::wstring m_tag;
		Position m_position;
		Position m_scale;
		Color m_color;
		Geometry *m_geometry;
	};
}

#endif //GLYPHOBJECT_H