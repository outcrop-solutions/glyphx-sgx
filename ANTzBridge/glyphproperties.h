#ifndef SYNGLYPHX_GLYPHPROPERTIES
#define SYNGLYPHX_GLYPHPROPERTIES

#include "ANTzBridge.h"
#include <boost/shared_ptr.hpp>
#include "glyphmappableproperties.h"

struct NPnode;
typedef NPnode* pNPnode;

namespace SynGlyphX {

    class ANTZBRIDGE_API GlyphProperties : public GlyphMappableProperties
    {
    public:
		typedef boost::shared_ptr<GlyphProperties> SharedPtr;
		typedef boost::shared_ptr<const GlyphProperties> ConstSharedPtr;

        GlyphProperties();
        GlyphProperties(pNPnode node);
        GlyphProperties(const GlyphProperties& properties);
        ~GlyphProperties();

		GlyphProperties& operator=(const GlyphProperties& properties);
		bool operator==(const GlyphProperties& properties) const;

        void SetGeometry(Geometry::Shape shape, Geometry::Surface surface);
        Geometry::Shape GetShape() const;
        Geometry::Surface GetSurface() const;

        void SetTopology(Topology::Type topology);
        Topology::Type GetTopology() const;

        static boost::shared_ptr<GlyphProperties> GetRoot();
        static boost::shared_ptr<const GlyphProperties> GetTemplate();

        void SetTagOffset(double x, double y, double z);
        const Vector3& GetTagOffset() const;

		static std::wstring ShapeToString(Geometry::Shape shape);
		static std::wstring SurfaceToString(Geometry::Surface surface);
		static std::wstring TopologyTypeToString(Topology::Type topo);

    protected:
        static boost::shared_ptr<GlyphProperties> CreateRootPin();

        static boost::shared_ptr<GlyphProperties> s_root;
        static boost::shared_ptr<GlyphProperties> s_template;

        Vector3 m_tagOffset;

        Geometry::Shape m_geometryShape;
        Geometry::Surface m_geometrySurface;

        Topology::Type m_topology;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHPROPERTIES