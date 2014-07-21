#include "minmaxglyph.h"

namespace SynGlyphX {

	MinMaxGlyph::MinMaxGlyph(const GlyphProperties& minGlyph) :
		m_minGlyph(minGlyph),
		m_difference(GlyphMappableProperties::GetPropertiesZero())
	{
	}

	MinMaxGlyph::MinMaxGlyph(const boost::property_tree::wptree& propertyTree) :
		m_difference(GlyphMappableProperties::GetPropertiesZero()) {


	}

	MinMaxGlyph::MinMaxGlyph(const MinMaxGlyph& glyph) :
		m_minGlyph(glyph.m_minGlyph),
		m_difference(glyph.m_difference) {

	}

	MinMaxGlyph::~MinMaxGlyph()
	{
	}

	MinMaxGlyph& MinMaxGlyph::operator=(const MinMaxGlyph& glyph) {

		m_minGlyph = glyph.m_minGlyph;
		m_difference = glyph.m_difference;

		return *this;
	}

	const GlyphProperties& MinMaxGlyph::GetMinGlyph() const {

		return m_minGlyph;
	}

	const GlyphMappableProperties& MinMaxGlyph::GetDifference() const {

		return m_difference;
	}

	void MinMaxGlyph::SetMinGlyphProperties(const GlyphMappableProperties& properties) {

		m_minGlyph.GlyphMappableProperties::operator=(properties);
	}

	void MinMaxGlyph::SetDifference(const GlyphMappableProperties& difference) {

		m_difference = difference;
	}

	MinMaxGlyph::PropertyTree& MinMaxGlyph::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		PropertyTree& rootGlyphPropertyTree = propertyTree.add(L"Glyph", L"");

		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Position", m_minGlyph.GetTranslate(), m_difference.GetTranslate());
		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Rotation", m_minGlyph.GetRotate(), m_difference.GetRotate());
		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Scale", m_minGlyph.GetScale(), m_difference.GetScale());

		rootGlyphPropertyTree.put<std::wstring>(L"Color", m_minGlyph.GetColor().ToHexString(3));

		AddValueToPropertyTree(rootGlyphPropertyTree, L"Transparency", m_minGlyph.GetColor()[3], m_difference.GetColor()[3]);

		if (m_minGlyph.GetShape() == GlyphProperties::Shape::Torus) {
			AddValueToPropertyTree(rootGlyphPropertyTree, L"TorusRatio", m_minGlyph.GetRatio(), m_difference.GetRatio());
		}

		return rootGlyphPropertyTree;
	}

	void MinMaxGlyph::AddVector3ToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, const Vector3& min, const Vector3& difference) const {
		
		boost::property_tree::wptree& vectorPropertyTree = propertyTreeParent.add(name, L"");
		AddValueToPropertyTree(vectorPropertyTree, L"X", min[0], difference[0]);
		AddValueToPropertyTree(vectorPropertyTree, L"Y", min[1], difference[1]);
		AddValueToPropertyTree(vectorPropertyTree, L"Z", min[2], difference[2]);
	}

	void MinMaxGlyph::AddValueToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, double min, double difference) const {

		boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		valuePropertyTree.put<double>(L"Min", min);

		if (std::abs(difference) > 0.01) {

			valuePropertyTree.put<double>(L"Difference", difference);
		}
	}

} //namespace SynGlyphX