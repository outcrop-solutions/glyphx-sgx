#ifndef SYNGLYPHX_GLYPHSTRUCTURALPROPERTIES_H
#define SYNGLYPHX_GLYPHSTRUCTURALPROPERTIES_H

#include "sgxfoundation.h"
#include <boost/bimap.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXFOUNDATION_API GlyphStructuralProperties
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
			Cylinder,			//radius 1, height 2
		};

		enum Surface {
			Wireframe = 0,
			Solid
		};

		enum VirtualTopology {
			Null = 0,		// linear 3D euclidean space
			CubePlane,			//six facet coord system for each side of cube
			SphereNonZeroRadius,			//spherical coords compatible with KML
			Circle,			//default branchLevel = 1 attached to pin
			CylinderSide,
			LinePin,				//default root pin shaped as icecream cone
			LineRod,
			SphereZeroRadius,			//zero origin offset with spherical coords
		};

		typedef boost::bimap<Shape, std::wstring> ShapeBimap;
		typedef boost::bimap<Surface, std::wstring> SurfaceBimap;
		typedef boost::bimap<VirtualTopology, std::wstring> VirtualTopologyBimap;

		GlyphStructuralProperties(Shape shape = Shape::Torus, Surface surface = Surface::Solid, VirtualTopology virtualTopology = VirtualTopology::Circle);
		GlyphStructuralProperties(const boost::property_tree::wptree& propertyTree);
		GlyphStructuralProperties(const GlyphStructuralProperties& properties);
		~GlyphStructuralProperties();

		GlyphStructuralProperties& operator=(const GlyphStructuralProperties& properties);
		bool operator==(const GlyphStructuralProperties& properties) const;
		bool operator!=(const GlyphStructuralProperties& properties) const;

		void SetGeometryShape(Shape shape);
		Shape GetGeometryShape() const;

		void SetGeometrySurface(Surface surface);
		Surface GetGeometrySurface() const;

		void SetVirtualTopology(VirtualTopology virtualTopology);
		VirtualTopology GetVirtualTopology() const;

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		static const ShapeBimap s_shapeNames;
		static const SurfaceBimap s_surfaceNames;
		static const VirtualTopologyBimap s_virtualTopologyNames;

	private:
		Shape m_geometryShape;
		Surface m_geometrySurface;
		VirtualTopology m_virtualTopology;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHSTRUCTURALPROPERTIES_H