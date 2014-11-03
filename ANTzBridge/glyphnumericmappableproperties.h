#ifndef SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H
#define SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H

#include "ANTzBridge.h"
#include "foundationtypes.h"
#include "color.h"

namespace SynGlyphX {

	class ANTZBRIDGE_API GlyphNumericMappableProperties
	{
	public:
		GlyphNumericMappableProperties();
		GlyphNumericMappableProperties(const GlyphNumericMappableProperties& properties);
		virtual ~GlyphNumericMappableProperties();

		GlyphNumericMappableProperties& operator=(const GlyphNumericMappableProperties& properties);
		bool operator==(const GlyphNumericMappableProperties& properties) const;
		bool operator!=(const GlyphNumericMappableProperties& properties) const;
		bool operator<(const GlyphNumericMappableProperties& properties) const;

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

		void SetToZero();
		void AddDifference(const GlyphNumericMappableProperties& properties);
		void Subtract(const GlyphNumericMappableProperties& properties);

		static const GlyphNumericMappableProperties& GetDefaultMinProperties();

	protected:
		Vector3 m_rotation;
		Vector3 m_scale;
		Vector3 m_position;

		double m_ratio;

		Color m_color;

	private:
		static GlyphNumericMappableProperties CreateDefaultMinProperties();
		static GlyphNumericMappableProperties s_defaultMinProperties;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H