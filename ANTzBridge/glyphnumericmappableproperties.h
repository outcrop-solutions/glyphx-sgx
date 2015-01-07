#ifndef SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H
#define SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H

#include "ANTzBridge.h"
#include "foundationtypes.h"
#include "color.h"

namespace SynGlyphXANTz {

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

		void SetRotation(const SynGlyphX::Vector3& rotation);
		const SynGlyphX::Vector3& GetRotation() const;
		void SetPosition(const SynGlyphX::Vector3& position);
		const SynGlyphX::Vector3& GetPosition() const;
		void SetScale(const SynGlyphX::Vector3& scale);
		const SynGlyphX::Vector3& GetScale() const;

		void SetColor(const SynGlyphX::Color& color);
		const SynGlyphX::Color& GetColor() const;

		void SetRatio(double ratio);
		double GetRatio() const;

		void SetToZero();
		void AddDifference(const GlyphNumericMappableProperties& properties);
		void Subtract(const GlyphNumericMappableProperties& properties);

		static const GlyphNumericMappableProperties& GetDefaultMinProperties();

	protected:
		SynGlyphX::Vector3 m_rotation;
		SynGlyphX::Vector3 m_scale;
		SynGlyphX::Vector3 m_position;

		double m_ratio;

		SynGlyphX::Color m_color;

	private:
		static GlyphNumericMappableProperties CreateDefaultMinProperties();
		static GlyphNumericMappableProperties s_defaultMinProperties;
	};

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H