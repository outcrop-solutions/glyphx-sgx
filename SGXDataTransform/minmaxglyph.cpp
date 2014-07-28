#include "minmaxglyph.h"

namespace SynGlyphX {

	MinMaxGlyph::MinMaxGlyph(const GlyphProperties& minGlyph) :
		m_minGlyph(minGlyph),
		m_difference(GlyphMappableProperties::GetPropertiesZero())
	{
	}

	MinMaxGlyph::MinMaxGlyph(const boost::property_tree::wptree& propertyTree) :
		m_difference(GlyphMappableProperties::GetPropertiesZero()) {

		Vector3 minVec3;
		Vector3 differenceVec3;
		double min, difference;

		GetVector3FromPropertyTree(propertyTree.get_child(L"Position"), minVec3, differenceVec3, m_inputfields);
		m_minGlyph.SetPosition(minVec3);
		m_difference.SetPosition(differenceVec3);

		GetVector3FromPropertyTree(propertyTree.get_child(L"Rotation"), minVec3, differenceVec3, &m_inputfields[3]);
		m_minGlyph.SetRotation(minVec3);
		m_difference.SetRotation(differenceVec3);

		GetVector3FromPropertyTree(propertyTree.get_child(L"Scale"), minVec3, differenceVec3, &m_inputfields[6]);
		m_minGlyph.SetScale(minVec3);
		m_difference.SetScale(differenceVec3);

		Color minColor;
		Color differenceColor;
		GetColorFromPropertyTree(propertyTree, minColor, differenceColor, &m_inputfields[9]);
		m_minGlyph.SetColor(minColor);
		m_difference.SetColor(differenceColor);

		boost::optional<const boost::property_tree::wptree&> torusRatioTree = propertyTree.get_child_optional(L"TorusRatio");

		if (torusRatioTree.is_initialized()) {
			GetValueFromPropertyTree(torusRatioTree.get(), min, difference, m_inputfields[11]);
			m_minGlyph.SetRatio(min);
			m_difference.SetRatio(difference);
		}

		m_minGlyph.SetGeometry(GlyphProperties::s_shapeNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Shape")), 
							   GlyphProperties::s_surfaceNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Surface")));
		m_minGlyph.SetTopology(GlyphProperties::s_topologyNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Topology")));
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

		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Position", m_minGlyph.GetPosition(), m_difference.GetPosition(), m_inputfields);
		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Rotation", m_minGlyph.GetRotation(), m_difference.GetRotation(), &m_inputfields[3]);
		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Scale", m_minGlyph.GetScale(), m_difference.GetScale(), &m_inputfields[6]);

		AddColorToPropertyTree(rootGlyphPropertyTree, m_minGlyph.GetColor(), m_difference.GetColor(), &m_inputfields[9]);

		if (m_minGlyph.GetShape() == GlyphProperties::Shape::Torus) {
			AddValueToPropertyTree(rootGlyphPropertyTree, L"TorusRatio", m_minGlyph.GetRatio(), m_difference.GetRatio(), m_inputfields[11]);
		}

		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Shape", GlyphProperties::s_shapeNames.left.at(m_minGlyph.GetShape()));
		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Surface", GlyphProperties::s_surfaceNames.left.at(m_minGlyph.GetSurface()));
		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Topology", GlyphProperties::s_topologyNames.left.at(m_minGlyph.GetTopology()));

		return rootGlyphPropertyTree;
	}

	void MinMaxGlyph::AddVector3ToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, const Vector3& min, const Vector3& difference, const InputField inputfield[3]) const {
		
		boost::property_tree::wptree& vectorPropertyTree = propertyTreeParent.add(name, L"");
		AddValueToPropertyTree(vectorPropertyTree, L"X", min[0], difference[0], inputfield[0]);
		AddValueToPropertyTree(vectorPropertyTree, L"Y", min[1], difference[1], inputfield[1]);
		AddValueToPropertyTree(vectorPropertyTree, L"Z", min[2], difference[2], inputfield[2]);
	}

	void MinMaxGlyph::AddValueToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, double min, double difference, const InputField& inputfield) const {

		boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		valuePropertyTree.put<double>(L"Min", min);

		if (std::abs(difference) > 0.01) {

			valuePropertyTree.put<double>(L"Difference", difference);
		}

		if (inputfield.IsValid()) {
			inputfield.ExportToPropertyTree(valuePropertyTree);
		}
	}

	void MinMaxGlyph::AddColorToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const Color& min, const Color& difference, const InputField inputfield[2]) const {

		boost::property_tree::wptree& colorPropertyTree = propertyTreeParent.add(L"Color", L"");
		boost::property_tree::wptree& rgbPropertyTree = colorPropertyTree.add(L"RGB", L"");
		rgbPropertyTree.put<std::wstring>(L"Min", min.ToHexString(3));

		if ((difference[0] > 0) || (difference[1] > 0) || (difference[2] > 0)) {

			rgbPropertyTree.put<std::wstring>(L"Difference", difference.ToHexString(3));
		}

		if (inputfield[0].IsValid()) {
			inputfield[0].ExportToPropertyTree(rgbPropertyTree);
		}

		AddValueToPropertyTree(colorPropertyTree, L"Transparency", min[3], difference[3], inputfield[1]);
	}

	void MinMaxGlyph::GetVector3FromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, Vector3& min, Vector3& difference, InputField inputfield[3]) const {

		GetValueFromPropertyTree(propertyTreeParent.get_child(L"X"), min[0], difference[0], inputfield[0]);
		GetValueFromPropertyTree(propertyTreeParent.get_child(L"Y"), min[1], difference[1], inputfield[1]);
		GetValueFromPropertyTree(propertyTreeParent.get_child(L"Z"), min[2], difference[2], inputfield[2]);
	}

	void MinMaxGlyph::GetValueFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, double& min, double& difference, InputField& inputfield) const {

		min = propertyTreeParent.get<double>(L"Min");
		difference = propertyTreeParent.get_optional<double>(L"Difference").get_value_or(0.0);

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTreeParent.get_child_optional(L"InputField");
		if (inputFieldTree.is_initialized()) {
			inputfield = InputField(inputFieldTree.get());
		}
	}

	void MinMaxGlyph::GetColorFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, Color& min, Color& difference, InputField inputfield[2]) const {

		const boost::property_tree::wptree& colorPropertyTree = propertyTreeParent.get_child(L"Color");
		const boost::property_tree::wptree& rgbPropertyTree = colorPropertyTree.get_child(L"RGB");
		min.FromHexString(rgbPropertyTree.get<std::wstring>(L"Min"));
		difference.FromHexString(rgbPropertyTree.get_optional<std::wstring>(L"Difference").get_value_or(L"000000"));

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = rgbPropertyTree.get_child_optional(L"InputField");
		if (inputFieldTree.is_initialized()) {
			inputfield[0] = InputField(inputFieldTree.get());
		}

		const boost::property_tree::wptree& alphaPropertyTree = colorPropertyTree.get_child(L"Transparency");
		min[3] = alphaPropertyTree.get<unsigned char>(L"Min");
		difference[3] = alphaPropertyTree.get_optional<unsigned char>(L"Difference").get_value_or(0);

		boost::optional<const boost::property_tree::wptree&> inputFieldTree2 = alphaPropertyTree.get_child_optional(L"InputField");
		if (inputFieldTree2.is_initialized()) {
			inputfield[1] = InputField(inputFieldTree2.get());
		}
	}

} //namespace SynGlyphX