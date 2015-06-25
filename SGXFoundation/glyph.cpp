#include "glyph.h"

namespace SynGlyphX {

	const Glyph Glyph::s_defaultGlyph = Glyph::CreateDefaultGlyph(GlyphGeometryInfo::Shape::Torus, VirtualTopologyInfo::Type::Circle);
	const Glyph Glyph::s_defaultRootGlyph = Glyph::CreateDefaultGlyph(GlyphGeometryInfo::Shape::Pin, VirtualTopologyInfo::Type::LinePin);

	Glyph::Glyph() :
		GlyphTemplate < double, GlyphColor, std::wstring, GlyphGeometry, VirtualTopology >(),
		m_tagOffset({ {0.0, 1.5, 0.0} })
	{
	}

	Glyph::Glyph(const Glyph& glyph) :
		GlyphTemplate < double, GlyphColor, std::wstring, GlyphGeometry, VirtualTopology >(glyph),
		m_tagOffset(glyph.m_tagOffset) {


	}

	Glyph::~Glyph()
	{
	}

	Vector3& Glyph::GetTagOffset() {

		return m_tagOffset;
	}

	const Vector3& Glyph::GetTagOffset() const {

		return m_tagOffset;
	}

	Glyph& Glyph::operator=(const Glyph& glyph) {

		GlyphTemplate::operator=(glyph);

		m_tagOffset = glyph.m_tagOffset;

		return *this;
	}

	bool Glyph::operator==(const Glyph& glyph) const {

		if (!GlyphTemplate::operator==(glyph)) {

			return false;
		}

		if (m_tagOffset != glyph.m_tagOffset) {

			return false;
		}

		return true;
	}

	bool Glyph::operator!=(const Glyph& glyph) const {

		return (!operator==(glyph));
	}

	Glyph Glyph::CreateDefaultGlyph(GlyphGeometryInfo::Shape geometryShape, VirtualTopologyInfo::Type virtualTopologyType) {

		Glyph glyph;

		glyph.GetStructure().SetGeometryShape(geometryShape);
		glyph.GetStructure().SetGeometrySurface(GlyphGeometryInfo::Surface::Solid);

		glyph.GetVirtualTopology().SetType(virtualTopologyType);

		glyph.GetPosition()[0] = 0.0;
		glyph.GetPosition()[1] = 0.0;
		glyph.GetPosition()[2] = 0.0;
		glyph.GetRotation()[0] = 0.0;
		glyph.GetRotation()[1] = 0.0;
		glyph.GetRotation()[2] = 0.0;
		glyph.GetScale()[0] = 1.0;
		glyph.GetScale()[1] = 1.0;
		glyph.GetScale()[2] = 1.0;
		glyph.GetColor() = GlyphColor({ { 50, 101, 101 } });
		glyph.GetTransparency() = 255;

		glyph.GetTag() = L"";
		glyph.GetDescription() = L"";

		glyph.GetRotationRate()[0] = 0.0;
		glyph.GetRotationRate()[1] = 0.0;
		glyph.GetRotationRate()[2] = 0.0;

		return glyph;
	}

} //namespace SynGlyphX 