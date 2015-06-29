#pragma once
class GlyphGeometryInfo
{
public:
	
};

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

#ifndef SYNGLYPHX_GLYPHGEOMETRYINFO_H
#define SYNGLYPHX_GLYPHGEOMETRYINFO_H

#include "sgxfoundation.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXFOUNDATION_API GlyphGeometryInfo
	{
	public:
		enum Shape {
			Cube = 0,				//length 2, currently is something else, what? //zz debug
			Sphere,			//radius 1
			Cone,				//radius 1, height 2
			Torus,			//ratio 0.1, radius 1.5 should we change to 1.0 ?
			Dodecahedron,		//size ?
			Octahedron,		//size ?
			Tetrahedron,		//should have edge length of 2, actual size ?
			Icosahedron,		//should have edge length of 2, actual size ?
			Pin,				//height 5.5, 5 from tip to center of sphere
			Cylinder			//radius 1, height 2
		};

		enum Surface {
			Wireframe = 0,
			Solid
		};

		typedef boost::bimap<Shape, std::wstring> ShapeBimap;
		typedef boost::bimap<Surface, std::wstring> SurfaceBimap;

		GlyphGeometryInfo();
		~GlyphGeometryInfo();

		static const ShapeBimap s_shapeNames;
		static const SurfaceBimap s_surfaceNames;
	};

	//This translator is so that GlyphGeometryInfo::Shape can be automatically used by boost::property_tree
	class SGXFOUNDATION_API GeometryShapeTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef GlyphGeometryInfo::Shape external_type;

		GeometryShapeTranslator();

		boost::optional<GlyphGeometryInfo::Shape> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(GlyphGeometryInfo::Shape const& v);

	private:
		
	};

	//This translator is so that GlyphGeometryInfo::Shape can be automatically used by boost::property_tree
	class SGXFOUNDATION_API GeometrySurfaceTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef GlyphGeometryInfo::Surface external_type;

		GeometrySurfaceTranslator();

		boost::optional<GlyphGeometryInfo::Surface> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(GlyphGeometryInfo::Surface const& v);

	private:

	};

} //namespace SynGlyphX

namespace boost {

	namespace property_tree {

		template<>
		struct translator_between<std::wstring, SynGlyphX::GlyphGeometryInfo::Shape>
		{
			typedef SynGlyphX::GeometryShapeTranslator type;
		};

		template<>
		struct translator_between<std::wstring, SynGlyphX::GlyphGeometryInfo::Surface>
		{
			typedef SynGlyphX::GeometrySurfaceTranslator type;
		};
	}
} //namespace boost

#endif //SYNGLYPHX_GLYPHGEOMETRYINFO_H