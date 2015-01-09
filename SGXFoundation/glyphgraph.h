#ifndef SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H
#define SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H

#include "sgxfoundation.h"
#include "foundationtypes.h"
#include "color.h"
#include <boost/graph/undirected_graph.hpp>
#include <boost/bimap.hpp>
#include <memory>

namespace SynGlyphX {

	class SGXFOUNDATION_API Glyph {

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

		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
		Color color;
		std::wstring tag;
		std::wstring description;
		Shape geometryShape;
		Surface geometrySurface;
		VirtualTopology virtualTopology;

		Glyph();
		Glyph(const Glyph& glyph);
		~Glyph();
	};

	class SGXFOUNDATION_API GlyphEdge {

	public:
		enum Type {

			Topological,
			Link
		};

		Type m_type;

		GlyphEdge();
		GlyphEdge(const GlyphEdge& edge);
		~GlyphEdge();
	};

	class SGXFOUNDATION_API GlyphGraph : public boost::undirected_graph < Glyph, GlyphEdge > {

	public:
		typedef boost::bimap<Glyph::Shape, std::wstring> ShapeBimap;
		typedef boost::bimap<Glyph::Surface, std::wstring> SurfaceBimap;
		typedef boost::bimap<Glyph::VirtualTopology, std::wstring> VirtualTopologyBimap;

		typedef std::shared_ptr<GlyphGraph> SharedPtr;
		typedef std::shared_ptr<const GlyphGraph> SharedPtr;

		GlyphGraph();
		GlyphGraph(const GlyphGraph& graph);
		~GlyphGraph();

		static const ShapeBimap s_shapeNames;
		static const SurfaceBimap s_surfaceNames;
		static const VirtualTopologyBimap s_virtualTopologyNames;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_BUNDLEDPROPERTYGRAPH_H