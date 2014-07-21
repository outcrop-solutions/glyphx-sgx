#ifndef SYNGLYPHX_GLYPHMAPPABLEPROPERTIES
#define SYNGLYPHX_GLYPHMAPPABLEPROPERTIES

#include "ANTzBridge.h"
#include "foundationtypes.h"
#include "color.h"

namespace SynGlyphX {

    class ANTZBRIDGE_API GlyphMappableProperties
    {
    public:
        GlyphMappableProperties();
        GlyphMappableProperties(const GlyphMappableProperties& properties);
        ~GlyphMappableProperties();

		GlyphMappableProperties& operator=(const GlyphMappableProperties& properties);
		bool operator==(const GlyphMappableProperties& properties) const;
		bool operator<(const GlyphMappableProperties& properties) const;

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

		static const GlyphMappableProperties& GetPropertiesZero();

    protected:
        Vector3 m_rotate;
        Vector3 m_scale;
        Vector3 m_translate;

        double m_ratio;

        Color m_color;

	private:
		static GlyphMappableProperties CreatePropertiesZero();
		static GlyphMappableProperties s_zero;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHMAPPABLEPROPERTIES
