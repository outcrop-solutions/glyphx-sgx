#ifndef SYNGLYPHX_GLYPHTEMPLATE_H
#define SYNGLYPHX_GLYPHTEMPLATE_H

#include "sgxfoundation.h"
#include <array>

namespace SynGlyphX {

	template <typename NumericType, typename ColorType, typename TextType>
	class SGXFOUNDATION_API GlyphTemplate {

	public:
		typedef std::array<NumericType, 3> NumericTypeXYZ;

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

		NumericTypeXYZ position;
		NumericTypeXYZ rotation;
		NumericTypeXYZ scale;
		ColorType color;
		TextType tag;
		TextType description;
		Shape geometryShape;
		Surface geometrySurface;
		VirtualTopology virtualTopology;

		GlyphTemplate();
		GlyphTemplate(const GlyphTemplate& glyph);
		~GlyphTemplate();
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHTEMPLATE_H