#ifndef SYNGLYPHX_GLYPHPROPERTIES
#define SYNGLYPHX_GLYPHPROPERTIES

#include "ANTzBridge.h"
#include <boost/shared_ptr.hpp>
#include <boost/bimap.hpp>
#include "glyphmappableproperties.h"

struct NPnode;
typedef NPnode* pNPnode;

namespace SynGlyphX {

    class ANTZBRIDGE_API GlyphProperties : public GlyphMappableProperties
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

		enum Topology {
			Null = 0,		// linear 3D euclidean space
			CubePlane,			//six facet coord system for each side of cube
			SphereNonZeroRadius,			//spherical coords compatible with KML
			Circle,			//default branchLevel = 1 attached to pin
			CylinderSide,
			LinePin,				//default root pin shaped as icecream cone
			LineRod,
			SphereZeroRadius,			//zero origin offset with spherical coords
		};

		typedef boost::shared_ptr<GlyphProperties> SharedPtr;
		typedef boost::shared_ptr<const GlyphProperties> ConstSharedPtr;

		typedef boost::bimap<Shape, std::wstring> ShapeBimap;
		typedef boost::bimap<Surface, std::wstring> SurfaceBimap;
		typedef boost::bimap<Topology, std::wstring> TopologyBimap;

        GlyphProperties();
        GlyphProperties(pNPnode node);
        GlyphProperties(const GlyphProperties& properties);
        ~GlyphProperties();

		GlyphProperties& operator=(const GlyphProperties& properties);
		bool operator==(const GlyphProperties& properties) const;
		bool operator!=(const GlyphProperties& properties) const;

        void SetGeometry(Shape shape, Surface surface);
        Shape GetShape() const;
        Surface GetSurface() const;

        void SetTopology(Topology topology);
        Topology GetTopology() const;

		static ConstSharedPtr GetRoot();
		static ConstSharedPtr GetTemplate();

        void SetTagOffset(double x, double y, double z);
        const Vector3& GetTagOffset() const;

		static const ShapeBimap s_shapeNames;
		static const SurfaceBimap s_surfaceNames;
		static const TopologyBimap s_topologyNames;

    protected:
        static SharedPtr CreateRootPin();

        static SharedPtr s_root;
        static SharedPtr s_template;

        Vector3 m_tagOffset;

        Shape m_geometryShape;
        Surface m_geometrySurface;

        Topology m_topology;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHPROPERTIES