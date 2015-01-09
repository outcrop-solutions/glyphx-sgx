#include "minmaxglyph.h"
#include <utility>

namespace SynGlyphX {

	template<typename PropertyType>
	TemplateGlyph::TemplateProperty<PropertyType>::TemplateProperty() :
		value(PropertyType()),
		binding(InputBinding()) {

	}

	template<typename PropertyType>
	TemplateGlyph::TemplateProperty<PropertyType>::TemplateProperty(const PropertyType& initialValue) :
		value(initalValue),
		binding(InputBinding()) {

	}

	template<typename PropertyType>
	TemplateGlyph::TemplateProperty<PropertyType>::TemplateProperty(const boost::property_tree::wptree& propertyTree) :
		value(propertyTree.get_optional<PropertyType>(L"Value").get_value_or(PropertyType())),
		binding(InputBinding()) {

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {
			
			binding = InputBinding(inputFieldTree.get());
		}
	}

	template<>
	TemplateGlyph::TemplateProperty<std::pair<double, double>>::TemplateProperty(const boost::property_tree::wptree& propertyTree)  {

		value.first = propertyTree.get<double>(L"Min");
		value.second = propertyTree.get_optional<double>(L"Difference").get_value_or(0.0);

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {
			
			binding = InputBinding(inputFieldTree.get());
		}
		else {

			binding.Clear();
		}
	}

	template<>
	TemplateGlyph::TemplateProperty<std::pair<Color, Color>>::TemplateProperty(const boost::property_tree::wptree& propertyTree) {

		const boost::property_tree::wptree& colorPropertyTree = propertyTree.get_child(L"Color");
		const boost::property_tree::wptree& rgbPropertyTree = colorPropertyTree.get_child(L"RGB");
		const boost::property_tree::wptree& rgbMinPropertyTree = rgbPropertyTree.get_child(L"Min");
		value.first.Set(0, rgbMinPropertyTree.get<short>(L"R"));
		value.first.Set(1, rgbMinPropertyTree.get<short>(L"G"));
		value.first.Set(2, rgbMinPropertyTree.get<short>(L"B"));

		boost::optional<const boost::property_tree::wptree&> rgbDiffPropertyTree = rgbPropertyTree.get_child_optional(L"Difference");
		if (rgbDiffPropertyTree.is_initialized()) {

			value.second.Set(0, rgbDiffPropertyTree.get().get<short>(L"R"));
			value.second.Set(1, rgbDiffPropertyTree.get().get<short>(L"G"));
			value.second.Set(2, rgbDiffPropertyTree.get().get<short>(L"B"));
		}
		else {
			value.second.FromHexString(L"000000");
		}

		boost::optional<const boost::property_tree::wptree&> inputFieldTree = propertyTree.get_child_optional(InputBinding::PropertyTreeName);
		if (inputFieldTree.is_initialized()) {

			binding = InputBinding(inputFieldTree.get());
		}
		else {

			binding.Clear();
		}
	}

	template<typename PropertyType>
	TemplateGlyph::TemplateProperty<PropertyType>::TemplateProperty(const MinMaxProperty& prop) :
		value(prop.value),
		binding(prop.binding) {

	}

	template<typename PropertyType>
	TemplateGlyph::TemplateProperty<PropertyType>::~TemplateProperty() {

	}

	template <typename PropertyType>
	boost::property_tree::wptree& TemplateGlyph::TemplateProperty<PropertyType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<PropertyType>(L"Value", value);

		if (binding.IsBoundToInputField()) {
			binding.ExportToPropertyTree(propertyTree);
		}
	}

	template <>
	boost::property_tree::wptree& TemplateGlyph::TemplateProperty<std::pair<double, double>>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<double>(L"Min", value.first);

		if (std::abs(value.second) > 0.01) {

			propertyTree.put<double>(L"Difference", value.second);
		}

		if (binding.IsBoundToInputField()) {
			binding.ExportToPropertyTree(propertyTree);
		}
	}

	template <>
	boost::property_tree::wptree& TemplateGlyph::TemplateProperty<std::pair<Color, Color>>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		boost::property_tree::wptree& rgbMinPropertyTree = propertyTree.add(L"Min", L"");
		rgbMinPropertyTree.put<short>(L"R", value.first[0]);
		rgbMinPropertyTree.put<short>(L"G", value.first[1]);
		rgbMinPropertyTree.put<short>(L"B", value.first[2]);

		if ((std::abs(value.second[0]) > 0) || (std::abs(value.second[1]) > 0) || (std::abs(value.second[2]) > 0)) {

			boost::property_tree::wptree& rgbDiffPropertyTree = propertyTree.add(L"Difference", L"");
			rgbDiffPropertyTree.put<short>(L"R", value.second[0]);
			rgbDiffPropertyTree.put<short>(L"G", value.second[1]);
			rgbDiffPropertyTree.put<short>(L"B", value.second[2]);
		}

		if (binding.IsBoundToInputField()) {
			binding.ExportToPropertyTree(propertyTree);
		}
	}

	TemplateGlyph::TemplateGlyph() : 
		position(),
		rotation(),
		scale(),
		color(),
		transparency(),
		tag(),
		description(),
		geometryShape(Glyph::Shape::Torus),
		geometrySurface(Glyph::Surface::Solid),
		virtualTopolgy(Glyph::VirtualTopology::Circle) {


	}

	TemplateGlyph::TemplateGlyph(const Glyph& glyph) :
		position(),
		rotation(),
		scale(),
		color(std::pair<Color, Color>(Color(), glyph.color)),
		transparency(std::pair<double, double>(0.0, glyph.color[3])),
		tag(glyph.tag),
		description(glyph.description), 
		geometryShape(glyph.geometryShape),
		geometrySurface(glyph.geometrySurface),
		virtualTopolgy(glyph.virtualTopology) {

		position[0] = NumericProperty(std::pair<double, double>(0.0, glyph.position[0]));
		position[1] = NumericProperty(std::pair<double, double>(0.0, glyph.position[1]));
		position[2] = NumericProperty(std::pair<double, double>(0.0, glyph.position[2]));
		rotation[0] = NumericProperty(std::pair<double, double>(0.0, glyph.rotation[0]));
		rotation[1] = NumericProperty(std::pair<double, double>(0.0, glyph.rotation[1]));
		rotation[2] = NumericProperty(std::pair<double, double>(0.0, glyph.rotation[2]));
		scale[0] = NumericProperty(std::pair<double, double>(0.0, glyph.scale[0]));
		scale[1] = NumericProperty(std::pair<double, double>(0.0, glyph.scale[1]));
		scale[2] = NumericProperty(std::pair<double, double>(0.0, glyph.scale[2]));
	}

	TemplateGlyph::TemplateGlyph(const boost::property_tree::wptree& propertyTree) :
		position(),
		rotation(),
		scale(),
		color(),
		transparency(),
		tag(),
		description(),
		geometryShape(GlyphGraph::s_shapeNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Shape"))),
		geometrySurface(GlyphGraph::s_surfaceNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Surface"))),
		virtualTopolgy(GlyphGraph::s_virtualTopologyNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Topology"))) {

		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Position"), position);
		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Rotation"), rotation);
		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Scale"), scale);

		const boost::property_tree::wptree& colorPropertyTree = propertyTree.get_child(L"Color");
		color = ColorProperty(colorPropertyTree.get_child(L"RGB"));
		transparency = NumericProperty(colorPropertyTree.get_child(L"Transparency"));

		tag = TextProperty(propertyTree.get_child(L"Tag"));
		boost::optional<const boost::property_tree::wptree&> descriptionTree = propertyTree.get_child_optional(L"Description");
		if (descriptionTree.is_initialized()) {

			description = TextProperty(descriptionTree.get());
		}
	}

	TemplateGlyph::~TemplateGlyph() {

	}

	boost::property_tree::wptree& TemplateGlyph::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& rootGlyphPropertyTree = propertyTree.add(L"Glyph", L"");

		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Position", L""), position);
		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Rotation", L""), rotation);
		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Scale", L""), scale);

		boost::property_tree::wptree& colorPropertyTree = propertyTree.add(L"Color", L"");
		color.ExportToPropertyTree(colorPropertyTree.add(L"RGB", L""));
		transparency.ExportToPropertyTree(colorPropertyTree.add(L"Transparency", L""));

		tag.ExportToPropertyTree(rootGlyphPropertyTree.add(L"Tag", L""));
		description.ExportToPropertyTree(rootGlyphPropertyTree.add(L"Description", L""));

		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Shape", GlyphGraph::s_shapeNames.left.at(geometryShape));
		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Surface", GlyphGraph::s_surfaceNames.left.at(geometrySurface));
		rootGlyphPropertyTree.put<std::wstring>(L"<xmlattr>.Topology", GlyphGraph::s_virtualTopologyNames.left.at(virtualTopolgy));

		return rootGlyphPropertyTree;
	}

	void TemplateGlyph::GetXYZNumericPropertiesFromPropertyTree(const boost::property_tree::wptree& propertyTree, NumericPropertyXYZ& prop) {

		prop[0] = NumericProperty(propertyTree.get_child(L"X"));
		prop[1] = NumericProperty(propertyTree.get_child(L"Y"));
		prop[2] = NumericProperty(propertyTree.get_child(L"Z"));
	}

	void TemplateGlyph::AddXYZNumericPropertiesToPropertyTree(boost::property_tree::wptree& propertyTree, const NumericPropertyXYZ& prop) const {

		prop[0].ExportToPropertyTree(propertyTree.add(L"X", L""));
		prop[1].ExportToPropertyTree(propertyTree.add(L"Y", L""));
		prop[2].ExportToPropertyTree(propertyTree.add(L"Z", L""));
	}

	SynGlyphX::Glyph TemplateGlyph::GetMinGlyph() const {

		SynGlyphX::Glyph glyph;

		glyph.position[0] = position[0].value.first;
		glyph.position[1] = position[1].value.first;
		glyph.position[2] = position[2].value.first;

		glyph.rotation[0] = rotation[0].value.first;
		glyph.rotation[1] = rotation[1].value.first;
		glyph.rotation[2] = rotation[2].value.first;

		glyph.scale[0] = scale[0].value.first;
		glyph.scale[1] = scale[1].value.first;
		glyph.scale[2] = scale[2].value.first;

		glyph.color = color.value.first;
		glyph.color.Set(3, transparency.value.first);

		glyph.tag = tag.value;
		glyph.description = description.value;

		glyph.geometryShape = geometryShape;
		glyph.geometrySurface = geometrySurface;
		glyph.virtualTopology = virtualTopolgy;

		return glyph;
	}

	SynGlyphX::Glyph TemplateGlyph::GetDifference() const {

		SynGlyphX::Glyph glyph;

		glyph.position[0] = position[0].value.second;
		glyph.position[1] = position[1].value.second;
		glyph.position[2] = position[2].value.second;

		glyph.rotation[0] = rotation[0].value.second;
		glyph.rotation[1] = rotation[1].value.second;
		glyph.rotation[2] = rotation[2].value.second;

		glyph.scale[0] = scale[0].value.second;
		glyph.scale[1] = scale[1].value.second;
		glyph.scale[2] = scale[2].value.second;

		glyph.color = color.value.second;
		glyph.color.Set(3, transparency.value.second);

		glyph.tag = tag.value;
		glyph.description = description.value;

		glyph.geometryShape = geometryShape;
		glyph.geometrySurface = geometrySurface;
		glyph.virtualTopology = virtualTopolgy;

		return glyph;
	}

	SynGlyphX::Glyph TemplateGlyph::GetMaxGlyph() const {

		SynGlyphX::Glyph glyph;

		glyph.position[0] = position[0].value.first + position[0].value.second;
		glyph.position[1] = position[1].value.first + position[1].value.second;
		glyph.position[2] = position[2].value.first + position[2].value.second;

		glyph.rotation[0] = rotation[0].value.first + rotation[0].value.second;
		glyph.rotation[1] = rotation[1].value.first + rotation[1].value.second;
		glyph.rotation[2] = rotation[2].value.first + rotation[2].value.second;

		glyph.scale[0] = scale[0].value.first + scale[0].value.second;
		glyph.scale[1] = scale[1].value.first + scale[1].value.second;
		glyph.scale[2] = scale[2].value.first + scale[2].value.second;

		glyph.color.Set(0, color.value.first[0] + color.value.second[0]);
		glyph.color.Set(1, color.value.first[0] + color.value.second[1]);
		glyph.color.Set(2, color.value.first[0] + color.value.second[2]);
		glyph.color.Set(3, transparency.value.first + transparency.value.second);

		glyph.tag = tag.value;
		glyph.description = description.value;

		glyph.geometryShape = geometryShape;
		glyph.geometrySurface = geometrySurface;
		glyph.virtualTopology = virtualTopolgy;

		return glyph;
	}

	bool TemplateGlyph::IsPositionXYBoundToInputFields() const {

		return (position[0].binding.IsBoundToInputField() && position[1].binding.IsBoundToInputField());
	}

	/*
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
	}*/

} //namespace SynGlyphX