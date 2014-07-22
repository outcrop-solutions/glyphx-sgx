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

        void SetRotation(const Vector3& rotation);
        const Vector3& GetRotation() const;
		void SetPosition(const Vector3& position);
        const Vector3& GetPosition() const;
        void SetScale(const Vector3& scale);
        const Vector3& GetScale() const;

        void SetColor(const Color& color);
        const Color& GetColor() const;

        void SetRatio(double ratio);
        double GetRatio() const;

		static const GlyphMappableProperties& GetPropertiesZero();

    protected:
		Vector3 m_rotation;
        Vector3 m_scale;
        Vector3 m_position;

        double m_ratio;

        Color m_color;

	private:
		static GlyphMappableProperties CreatePropertiesZero();
		static GlyphMappableProperties s_zero;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHMAPPABLEPROPERTIES
