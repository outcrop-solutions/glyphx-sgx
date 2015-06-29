#include "glyphgeometryinfo.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const GlyphGeometryInfo::ShapeBimap GlyphGeometryInfo::s_shapeNames = boost::assign::list_of < GlyphGeometryInfo::ShapeBimap::relation >
		(GlyphGeometryInfo::Shape::Cube, L"Cube")
		(GlyphGeometryInfo::Shape::Sphere, L"Sphere")
		(GlyphGeometryInfo::Shape::Cone, L"Cone")
		(GlyphGeometryInfo::Shape::Torus, L"Torus")
		(GlyphGeometryInfo::Shape::Dodecahedron, L"Dodecahedron")
		(GlyphGeometryInfo::Shape::Octahedron, L"Octahedron")
		(GlyphGeometryInfo::Shape::Tetrahedron, L"Tetrahedron")
		(GlyphGeometryInfo::Shape::Icosahedron, L"Icosahedron")
		(GlyphGeometryInfo::Shape::Pin, L"Pin")
		(GlyphGeometryInfo::Shape::Cylinder, L"Cylinder");

	const GlyphGeometryInfo::SurfaceBimap GlyphGeometryInfo::s_surfaceNames = boost::assign::list_of < GlyphGeometryInfo::SurfaceBimap::relation >
		(GlyphGeometryInfo::Surface::Wireframe, L"Wireframe")
		(GlyphGeometryInfo::Surface::Solid, L"Solid");

	GlyphGeometryInfo::GlyphGeometryInfo()
	{
	}


	GlyphGeometryInfo::~GlyphGeometryInfo()
	{
	}

	GeometryShapeTranslator::GeometryShapeTranslator() {


	}

	boost::optional<GlyphGeometryInfo::Shape> GeometryShapeTranslator::get_value(std::wstring const &v) {

		GlyphGeometryInfo::ShapeBimap::right_map::const_iterator iT = GlyphGeometryInfo::s_shapeNames.right.find(v);
		
		if (iT == GlyphGeometryInfo::s_shapeNames.right.end()) {

			return boost::none;
		}
		else {

			return iT->second;
		}
	}

	boost::optional<std::wstring> GeometryShapeTranslator::put_value(GlyphGeometryInfo::Shape const& v) {

		return GlyphGeometryInfo::s_shapeNames.left.at(v);
	}

	GeometrySurfaceTranslator::GeometrySurfaceTranslator() {


	}

	boost::optional<GlyphGeometryInfo::Surface> GeometrySurfaceTranslator::get_value(std::wstring const &v) {

		GlyphGeometryInfo::SurfaceBimap::right_map::const_iterator iT = GlyphGeometryInfo::s_surfaceNames.right.find(v);

		if (iT == GlyphGeometryInfo::s_surfaceNames.right.end()) {

			return boost::none;
		}
		else {

			return iT->second;
		}
	}

	boost::optional<std::wstring> GeometrySurfaceTranslator::put_value(GlyphGeometryInfo::Surface const& v) {

		return GlyphGeometryInfo::s_surfaceNames.left.at(v);
	}

} //namespace SynGlyphX