#ifndef SYNGLYPHX_GLYPHPROPERTIES
#define SYNGLYPHX_GLYPHPROPERTIES

#include "ANTzBridge.h"
#include <boost/shared_ptr.hpp>
#include "foundationtypes.h"
#include "data/nptypes.h"

namespace SynGlyphX {

    class ANTZBRIDGE_API GlyphProperties
    {
    public:
        GlyphProperties();
        GlyphProperties(pNPnode node);
        GlyphProperties(const GlyphProperties& properties);
        ~GlyphProperties();

        void SetRotate(double x, double y, double z);
        const Vector3& GetRotate() const;
        void SetTranslate(double x, double y, double z);
        const Vector3& GetTranslate() const;
        void SetScale(double x, double y, double z);
        const Vector3& GetScale() const;

        void SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);
        const Color& GetColor() const;

        void SetRatio(double ratio);
        double GetRatio() const;

        void SetGeometry(Geometry::Shape shape, Geometry::Surface surface);
        Geometry::Shape GetShape() const;
        Geometry::Surface GetSurface() const;

        void SetTopology(Topology::Type topology);
        Topology::Type GetTopology() const;

        static boost::shared_ptr<GlyphProperties> GetRoot();
        static boost::shared_ptr<const GlyphProperties> GetTemplate();

        virtual unsigned int GetNumberOfChildren() const;

        void SetTagOffset(double x, double y, double z);
        const Vector3& GetTagOffset() const;

    private:
        static boost::shared_ptr<GlyphProperties> CreateRootPin();

        static boost::shared_ptr<GlyphProperties> s_root;
        static boost::shared_ptr<GlyphProperties> s_template;

        Vector3 m_rotate;
        Vector3 m_scale;
        Vector3 m_translate;
        Vector3 m_tagOffset;

        double m_ratio;

        Color m_color;

        Geometry::Shape m_geometryShape;
        Geometry::Surface m_geometrySurface;

        Topology::Type m_topology;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHPROPERTIES