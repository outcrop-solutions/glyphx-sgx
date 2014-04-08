#ifndef SYNGLYPHX_GLYPH
#define SYNGLYPHX_GLYPH

#include <boost/shared_ptr.hpp>
#include <vector>
#include "typedefs.h"

namespace SynGlyphX {

    class Glyph
    {
    public:
        Glyph();
        ~Glyph();

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

        boost::shared_ptr<Glyph> GetChild(unsigned int index);

        void SetGeometry(Geometry::Shape shape, Geometry::Surface surface);
        Geometry::Shape GetShape() const;
        Geometry::Surface GetSurface() const;

        void SetTopology(Topology::Type topology);
        Topology::Type GetTopology() const;

    private:
        Vector3 m_rotate;
        Vector3 m_scale;
        Vector3 m_translate;
        Vector3 m_tagOffset;

        double m_ratio;

        Color m_color;

        Geometry::Shape m_geometryShape;
        Geometry::Surface m_geometrySurface;

        Topology::Type m_topology;

        std::vector<boost::shared_ptr<Glyph>> m_children;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPH