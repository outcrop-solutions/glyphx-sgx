#ifndef SYNGLYPHX_GLYPH
#define SYNGLYPHX_GLYPH

#include "ANTzBridge.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include "typedefs.h"
#include "data/nptypes.h"

namespace SynGlyphX {

    class ANTZBRIDGE_API Glyph
    {
    public:
        Glyph();
        Glyph(pNPnode node);
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

        //void AddChild(boost::shared_ptr<Glyph> glyph);
        //boost::shared_ptr<Glyph> GetChild(unsigned int index) const;

        void SetGeometry(Geometry::Shape shape, Geometry::Surface surface);
        Geometry::Shape GetShape() const;
        Geometry::Surface GetSurface() const;

        void SetTopology(Topology::Type topology);
        Topology::Type GetTopology() const;

        static boost::shared_ptr<Glyph> GetRoot();
        static boost::shared_ptr<const Glyph> GetTemplate();

        //void ClearChildren();

        unsigned int GetNumberOfChildren() const;
        void SetNumberOfChildren(unsigned int numberOfChildren);

        void SetTagOffset(double x, double y, double z);
        const Vector3& GetTagOffset() const;

    private:
        static boost::shared_ptr<Glyph> CreateRootPin();

        static boost::shared_ptr<Glyph> s_root;
        static boost::shared_ptr<Glyph> s_template;

        Vector3 m_rotate;
        Vector3 m_scale;
        Vector3 m_translate;
        Vector3 m_tagOffset;

        double m_ratio;

        Color m_color;

        Geometry::Shape m_geometryShape;
        Geometry::Surface m_geometrySurface;

        Topology::Type m_topology;

        unsigned int m_numberOfChildren;
        //std::vector<boost::shared_ptr<Glyph>> m_children;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPH