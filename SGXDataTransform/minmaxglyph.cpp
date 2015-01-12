#include "minmaxglyph.h"
#include <utility>

namespace SynGlyphX {

	DataMappingGlyph::DataMappingGlyph() : 
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>() {


	}

	DataMappingGlyph::DataMappingGlyph(const DataMappingGlyph& glyph) :
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>(glyph) {


	}

	DataMappingGlyph::DataMappingGlyph(const Glyph& glyph) :
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>() {

		structure = glyph.structure;

		position[0] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.position[0]));
		position[1] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.position[1]));
		position[2] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.position[2]));
		rotation[0] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.rotation[0]));
		rotation[1] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.rotation[1]));
		rotation[2] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.rotation[2]));
		scale[0] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.scale[0]));
		scale[1] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.scale[1]));
		scale[2] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.scale[2]));
		color = ColorMappingProperty(std::pair<Color, Color>(Color(), glyph.color));
		transparency = NumericMappingProperty(std::pair<double, double>(0.0, glyph.transparency));
		tag = TextMappingProperty(glyph.tag);
		description = TextMappingProperty(glyph.description);
	}

	DataMappingGlyph::DataMappingGlyph(const boost::property_tree::wptree& propertyTree) :
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>() {

		structure = GlyphStructuralProperties(propertyTree);

		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Position"), position);
		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Rotation"), rotation);
		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Scale"), scale);

		const boost::property_tree::wptree& colorPropertyTree = propertyTree.get_child(L"Color");
		color = ColorMappingProperty(colorPropertyTree.get_child(L"RGB"));
		transparency = NumericMappingProperty(colorPropertyTree.get_child(L"Transparency"));

		tag = TextMappingProperty(propertyTree.get_child(L"Tag"));
		boost::optional<const boost::property_tree::wptree&> descriptionTree = propertyTree.get_child_optional(L"Description");
		if (descriptionTree.is_initialized()) {

			description = TextMappingProperty(descriptionTree.get());
		}
	}

	DataMappingGlyph::~DataMappingGlyph() {

	}

	boost::property_tree::wptree& DataMappingGlyph::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& rootGlyphPropertyTree = propertyTree.add(L"Glyph", L"");

		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Position", L""), position);
		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Rotation", L""), rotation);
		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Scale", L""), scale);

		boost::property_tree::wptree& colorPropertyTree = propertyTree.add(L"Color", L"");
		color.ExportToPropertyTree(colorPropertyTree.add(L"RGB", L""));
		transparency.ExportToPropertyTree(colorPropertyTree.add(L"Transparency", L""));

		tag.ExportToPropertyTree(rootGlyphPropertyTree.add(L"Tag", L""));
		description.ExportToPropertyTree(rootGlyphPropertyTree.add(L"Description", L""));

		structure.ExportToPropertyTree(rootGlyphPropertyTree);

		return rootGlyphPropertyTree;
	}

	void DataMappingGlyph::GetXYZNumericPropertiesFromPropertyTree(const boost::property_tree::wptree& propertyTree, NumericMappingPropertyXYZ& prop) {

		prop[0] = NumericMappingProperty(propertyTree.get_child(L"X"));
		prop[1] = NumericMappingProperty(propertyTree.get_child(L"Y"));
		prop[2] = NumericMappingProperty(propertyTree.get_child(L"Z"));
	}

	void DataMappingGlyph::AddXYZNumericPropertiesToPropertyTree(boost::property_tree::wptree& propertyTree, const NumericMappingPropertyXYZ& prop) const {

		prop[0].ExportToPropertyTree(propertyTree.add(L"X", L""));
		prop[1].ExportToPropertyTree(propertyTree.add(L"Y", L""));
		prop[2].ExportToPropertyTree(propertyTree.add(L"Z", L""));
	}

	SynGlyphX::Glyph DataMappingGlyph::GetMinGlyph() const {

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

		glyph.structure = structure;

		return glyph;
	}

	SynGlyphX::Glyph DataMappingGlyph::GetDifference() const {

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

		glyph.structure = structure;

		return glyph;
	}

	SynGlyphX::Glyph DataMappingGlyph::GetMaxGlyph() const {

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

		glyph.structure = structure;

		return glyph;
	}

	bool DataMappingGlyph::IsPositionXYBoundToInputFields() const {

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