#ifndef SYNGLYPHX_TYPEDEFS
#define SYNGLYPHX_TYPEDEFS

#include <array>

namespace SynGlyphX {

    typedef std::array<double, 3> Vector3;
    typedef std::array<unsigned char, 4> Color;

    namespace Geometry {
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

    } //namespace Geometry

    namespace Topology {
        enum Type {
            Null = 0,		// linear 3D euclidean space
            Cube,			//six facet coord system for each side of cube
            Sphere,			//spherical coords compatible with KML
            Torus,			//default branchLevel = 1 attached to pin
            Cylinder,
            Pin,				//default root pin shaped as icecream cone
            Rod,
            Point,			//zero origin offset with spherical coords
        };

    } //namespace Geometry

} //namespace SynGlyphX

#endif //SYNGLYPHX_TYPEDEFS