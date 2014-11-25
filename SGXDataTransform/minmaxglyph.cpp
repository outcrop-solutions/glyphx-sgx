#include "minmaxglyph.h"

namespace SynGlyphX {

	MinMaxGlyph::MinMaxGlyph(const GlyphProperties& maxGlyph) :
		m_minGlyph(maxGlyph),
		m_difference(maxGlyph)
	{
		//Treat the maxGlyph parameter as the max glyph, but info like the tag needs to be minGlyph so first set m_minGlyph to copy maxGlyph followed by
		//setting the m_minGlyph numeric properties to the default minimum
		m_minGlyph.GlyphNumericMappableProperties::operator=(GlyphNumericMappableProperties::GetDefaultMinProperties());
		m_difference.Subtract(m_minGlyph);
	}

	MinMaxGlyph::MinMaxGlyph(const boost::property_tree::wptree& propertyTree) {

		m_difference.SetToZero();

		Vector3 minVec3;
		Vector3 differenceVec3;
		double min, difference;

		GetVector3FromPropertyTree(propertyTree.get_child(L"Position"), minVec3, differenceVec3, m_inputBindings);
		m_minGlyph.SetPosition(minVec3);
		m_difference.SetPosition(differenceVec3);

		GetVector3FromPropertyTree(propertyTree.get_child(L"Rotation"), minVec3, differenceVec3, &m_inputBindings[3]);
		m_minGlyph.SetRotation(minVec3);
		m_difference.SetRotation(differenceVec3);

		GetVector3FromPropertyTree(propertyTree.get_child(L"Scale"), minVec3, differenceVec3, &m_inputBindings[6]);
		m_minGlyph.SetScale(minVec3);
		m_difference.SetScale(differenceVec3);

		Color minColor;
		Color differenceColor;
		GetColorFromPropertyTree(propertyTree, minColor, differenceColor, &m_inputBindings[9]);
		m_minGlyph.SetColor(minColor);
		m_difference.SetColor(differenceColor);

		boost::optional<const boost::property_tree::wptree&> torusRatioTree = propertyTree.get_child_optional(L"TorusRatio");

		if (torusRatioTree.is_initialized()) {
			GetValueFromPropertyTree(torusRatioTree.get(), min, difference, m_inputBindings[11]);
			m_minGlyph.SetRatio(min);
			m_difference.SetRatio(difference);
		}

		GetStringFromPropertyTree(propertyTree.get_child(L"Tag"), m_inputBindings[12]);

		m_minGlyph.SetGeometry(GlyphProperties::s_shapeNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Shape")), 
							   GlyphProperties::s_surfaceNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Surface")));
		m_minGlyph.SetTopology(GlyphProperties::s_topologyNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Topology")));
	}

	MinMaxGlyph::MinMaxGlyph(const MinMaxGlyph& glyph) :
		m_minGlyph(glyph.m_minGlyph),
		m_difference(glyph.m_difference) {

		for (int i = 0; i < NumInputBindings; ++i) {
			m_inputBindings[i] = glyph.m_inputBindings[i];
		}
	}

	MinMaxGlyph::~MinMaxGlyph()
	{
	}

	MinMaxGlyph& MinMaxGlyph::operator=(const MinMaxGlyph& glyph) {

		m_minGlyph = glyph.m_minGlyph;
		m_difference = glyph.m_difference;

		for (int i = 0; i < NumInputBindings; ++i) {
			m_inputBindings[i] = glyph.m_inputBindings[i];
		}

		return *this;
	}

	bool MinMaxGlyph::operator==(const MinMaxGlyph& glyph) const {

		if (m_minGlyph != glyph.m_minGlyph) {

			return false;
		}

		if (m_difference != glyph.m_difference) {

			return false;
		}

		for (int i = 0; i < NumInputBindings; ++i) {
			
			if (m_inputBindings[i] != glyph.m_inputBindings[i]) {

				return false;
			}
		}

		return true;
	}

	bool MinMaxGlyph::operator!=(const MinMaxGlyph& glyph) const {

		return !operator==(glyph);
	}

	const GlyphProperties& MinMaxGlyph::GetMinGlyph() const {

		return m_minGlyph;
	}

	const GlyphNumericMappableProperties& MinMaxGlyph::GetDifference() const {

		return m_difference;
	}

	GlyphProperties MinMaxGlyph::GetMaxGlyph() const {

		GlyphProperties maxProperties = m_minGlyph;
		maxProperties.AddDifference(m_difference);

		return maxProperties;
	}

	void MinMaxGlyph::SetMinGlyph(const GlyphProperties& properties) {

		m_minGlyph.GlyphProperties::operator=(properties);
	}

	void MinMaxGlyph::SetDifference(const GlyphNumericMappableProperties& difference) {

		m_difference = difference;
	}

	MinMaxGlyph::PropertyTree& MinMaxGlyph::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		PropertyTree& rootGlyphPropertyTree = propertyTree.add(L"Glyph", L"");

		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Position", m_minGlyph.GetPosition(), m_difference.GetPosition(), m_inputBindings);
		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Rotation", m_minGlyph.GetRotation(), m_difference.GetRotation(), &m_inputBindings[3]);
		AddVector3ToPropertyTree(rootGlyphPropertyTree, L"Scale", m_minGlyph.GetScale(), m_difference.GetScale(), &m_inputBindings[6]);

		AddColorToPropertyTree(rootGlyphPropertyTree, m_minGlyph.GetColor(), m_difference.GetColor(), &m_inputBindings[9]);

		if (m_minGlyph.GetShape() == GlyphProperties::Shape::Torus) {
			AddValueToPropertyTree(rootGlyphPropertyTree, L"TorusRatio", m_minGlyph.GetRatio(), m_difference.GetRatio(), m_inputBindings[11]);
		}

		AddStringToPropertyTree(rootGlyphPropertyTree, L"Tag", m_inputBindings[12]);

		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Shape", GlyphProperties::s_shapeNames.left.at(m_minGlyph.GetShape()));
		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Surface", GlyphProperties::s_surfaceNames.left.at(m_minGlyph.GetSurface()));
		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Topology", GlyphProperties::s_topologyNames.left.at(m_minGlyph.GetTopology()));

		return rootGlyphPropertyTree;
	}

	void MinMaxGlyph::AddVector3ToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, const Vector3& min, const Vector3& difference, const InputBinding inputBindings[3]) const {
		
		boost::property_tree::wptree& vectorPropertyTree = propertyTreeParent.add(name, L"");
		AddValueToPropertyTree(vectorPropertyTree, L"X", min[0], difference[0], inputBindings[0]);
		AddValueToPropertyTree(vectorPropertyTree, L"Y", min[1], difference[1], inputBindings[1]);
		AddValueToPropertyTree(vectorPropertyTree, L"Z", min[2], difference[2], inputBindings[2]);
	}

	void MinMaxGlyph::AddValueToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, double min, double difference, const InputBinding& inputBinding) const {

		boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		valuePropertyTree.put<double>(L"Min", min);

		if (std::abs(difference) > 0.01) {

			valuePropertyTree.put<double>(L"Difference", difference);
		}

		if (inputBinding.IsBoundToInputField()) {
			inputBinding.ExportToPropertyTree(valuePropertyTree);
		}
	}

	void MinMaxGlyph::AddColorToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const Color& min, const Color& difference, const InputBinding inputBindings[2]) const {

		boost::property_tree::wptree& colorPropertyTree = propertyTreeParent.add(L"Color", L"");
		boost::property_tree::wptree& rgbPropertyTree = colorPropertyTree.add(L"RGB", L"");
		boost::property_tree::wptree& rgbMinPropertyTree = rgbPropertyTree.add(L"Min", L"");
		rgbMinPropertyTree.put<short>(L"R", min[0]);
		rgbMinPropertyTree.put<short>(L"G", min[1]);
		rgbMinPropertyTree.put<short>(L"B", min[2]);

		if ((std::abs(difference[0]) > 0) || (std::abs(difference[1]) > 0) || (std::abs(difference[2]) > 0)) {

			boost::property_tree::wptree& rgbDiffPropertyTree = rgbPropertyTree.add(L"Difference", L"");
			rgbDiffPropertyTree.put<short>(L"R", difference[0]);
			rgbDiffPropertyTree.put<short>(L"G", difference[1]);
			rgbDiffPropertyTree.put<short>(L"B", difference[2]);
		}

		if (inputBindings[0].IsBoundToInputField()) {
			inputBindings[0].ExportToPropertyTree(rgbPropertyTree);
		}

		AddValueToPropertyTree(colorPropertyTree, L"Transparency", min[3], difference[3], inputBindings[1]);
	}

	void MinMaxGlyph::AddStringToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, const InputBinding& inputfield) const {

		boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");

		if (inputfield.IsBoundToInputField()) {
			inputfield.ExportToPropertyTree(valuePropertyTree);
		}
	}

	void MinMaxGlyph::GetVector3FromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, Vector3& min, Vector3& difference, InputBinding inputfield[3]) const {

		GetValueFromPropertyTree(propertyTreeParent.get_child(L"X"), min[0], difference[0], inputfield[0]);
		GetValueFromPropertyTree(propertyTreeParent.get_child(L"Y"), min[1], difference[1], inputfield[1]);
		GetValueFromPropertyTree(propertyTreeParent.get_child(L"Z"), min[2], difference[2], inputfield[2]);
	}

	void MinMaxGlyph::GetValueFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, double& min, double& difference, InputBinding& inputfield) const {

		min = propertyTreeParent.get<double>(L"Min");
		difference = propertyTreeParent.get_optional<double>(L"Difference").get_value_or(0.0);

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTreeParent.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {
			inputfield = InputBinding(inputFieldTree.get());
		}
	}

	void MinMaxGlyph::GetColorFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, Color& min, Color& difference, InputBinding inputfield[2]) const {

		const boost::property_tree::wptree& colorPropertyTree = propertyTreeParent.get_child(L"Color");
		const boost::property_tree::wptree& rgbPropertyTree = colorPropertyTree.get_child(L"RGB");
		const boost::property_tree::wptree& rgbMinPropertyTree = rgbPropertyTree.get_child(L"Min");
		min.Set(0, rgbMinPropertyTree.get<short>(L"R"));
		min.Set(1, rgbMinPropertyTree.get<short>(L"G"));
		min.Set(2, rgbMinPropertyTree.get<short>(L"B"));

		boost::optional<const boost::property_tree::wptree&> rgbDiffPropertyTree = rgbPropertyTree.get_child_optional(L"Difference");
		if (rgbDiffPropertyTree.is_initialized()) {

			difference.Set(0, rgbDiffPropertyTree.get().get<short>(L"R"));
			difference.Set(1, rgbDiffPropertyTree.get().get<short>(L"G"));
			difference.Set(2, rgbDiffPropertyTree.get().get<short>(L"B"));
		}
		else {
			difference.FromHexString(L"000000");
		}

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = rgbPropertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {
			inputfield[0] = InputBinding(inputFieldTree.get());
		}

		const boost::property_tree::wptree& alphaPropertyTree = colorPropertyTree.get_child(L"Transparency");
		min.Set(3, alphaPropertyTree.get<unsigned char>(L"Min"));
		difference.Set(3, alphaPropertyTree.get_optional<unsigned char>(L"Difference").get_value_or(0));

		boost::optional<const boost::property_tree::wptree&> inputFieldTree2 = alphaPropertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree2.is_initialized()) {
			inputfield[1] = InputBinding(inputFieldTree2.get());
		}
	}

	void MinMaxGlyph::GetStringFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, InputBinding& inputBinding) const {

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTreeParent.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {
			inputBinding = InputBinding(inputFieldTree.get());
		}
	}

	const InputBinding& MinMaxGlyph::GetInputBinding(unsigned int index) const {

		if (index < NumInputBindings) {
			return m_inputBindings[index];
		}
		else {
			throw new std::out_of_range("Out of range of number of input bindings");
		}
	}

	void MinMaxGlyph::SetInputBinding(unsigned int index, const InputBinding& binding) {

		if (index < NumInputBindings) {
			m_inputBindings[index] = binding;
		}
		else {
			throw new std::out_of_range("Out of range of number of input fields");
		}
	}

	void MinMaxGlyph::ClearInputBinding(unsigned int index) {

		if (index < NumInputBindings) {
			m_inputBindings[index].Clear();
		}
		else {
			throw new std::out_of_range("Out of range of number of input fields");
		}
	}

	bool MinMaxGlyph::IsPositionXYBoundToInputFields() const {

		return (m_inputBindings[0].IsBoundToInputField() && m_inputBindings[1].IsBoundToInputField());
	}

} //namespace SynGlyphX