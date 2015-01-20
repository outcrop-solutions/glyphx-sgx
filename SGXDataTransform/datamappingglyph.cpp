#include "datamappingglyph.h"
#include <utility>

namespace SynGlyphX {

	DataMappingGlyph::DataMappingGlyph() : 
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>() {


	}

	DataMappingGlyph::DataMappingGlyph(const DataMappingGlyph& glyph) :
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>(glyph) {


	}

	DataMappingGlyph::DataMappingGlyph(const Glyph& glyph) :
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>(glyph.GetStructure()) {

		m_position[0] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetPosition()[0]));
		m_position[1] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetPosition()[1]));
		m_position[2] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetPosition()[2]));
		m_rotation[0] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetRotation()[0]));
		m_rotation[1] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetRotation()[1]));
		m_rotation[2] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetRotation()[2]));
		m_scale[0] = NumericMappingProperty(std::pair<double, double>(0.1, glyph.GetScale()[0] - 0.1));
		m_scale[1] = NumericMappingProperty(std::pair<double, double>(0.1, glyph.GetScale()[1] - 0.1));
		m_scale[2] = NumericMappingProperty(std::pair<double, double>(0.1, glyph.GetScale()[2] - 0.1));

		m_color = ColorMappingProperty(std::pair<GlyphColor, GlyphColor>(GlyphColor(), glyph.GetColor()));
		m_transparency = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetTransparency()));

		m_tag = TextMappingProperty(glyph.GetTag());
		m_description = TextMappingProperty(glyph.GetDescription());

		m_rotationRate[0] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetRotationRate()[0]));
		m_rotationRate[1] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetRotationRate()[1]));
		m_rotationRate[2] = NumericMappingProperty(std::pair<double, double>(0.0, glyph.GetRotationRate()[2]));
	}

	DataMappingGlyph::DataMappingGlyph(const boost::property_tree::wptree& propertyTree) :
		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>(GlyphStructuralProperties(propertyTree)) {

		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Position"), m_position);
		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Rotation"), m_rotation);
		GetXYZNumericPropertiesFromPropertyTree(propertyTree.get_child(L"Scale"), m_scale);

		const boost::property_tree::wptree& colorPropertyTree = propertyTree.get_child(L"Color");
		m_color = ColorMappingProperty(colorPropertyTree.get_child(L"RGB"));
		m_transparency = NumericMappingProperty(colorPropertyTree.get_child(L"Transparency"));

		m_tag = TextMappingProperty(propertyTree.get_child(L"Tag"));
		boost::optional<const boost::property_tree::wptree&> descriptionTree = propertyTree.get_child_optional(L"Description");
		if (descriptionTree.is_initialized()) {

			m_description = TextMappingProperty(descriptionTree.get());
		}

		boost::optional<const boost::property_tree::wptree&> rotationRateTree = propertyTree.get_child_optional(L"RotationRate");
		if (descriptionTree.is_initialized()) {

			GetXYZNumericPropertiesFromPropertyTree(rotationRateTree.get(), m_rotationRate);
		}
	}

	DataMappingGlyph::~DataMappingGlyph() {

	}

	boost::property_tree::wptree& DataMappingGlyph::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& rootGlyphPropertyTree = propertyTree.add(L"Glyph", L"");

		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Position", L""), m_position);
		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Rotation", L""), m_rotation);
		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"Scale", L""), m_scale);

		boost::property_tree::wptree& colorPropertyTree = rootGlyphPropertyTree.add(L"Color", L"");
		m_color.ExportToPropertyTree(colorPropertyTree.add(L"RGB", L""));
		m_transparency.ExportToPropertyTree(colorPropertyTree.add(L"Transparency", L""));

		m_tag.ExportToPropertyTree(rootGlyphPropertyTree.add(L"Tag", L""));
		m_description.ExportToPropertyTree(rootGlyphPropertyTree.add(L"Description", L""));

		AddXYZNumericPropertiesToPropertyTree(rootGlyphPropertyTree.add(L"RotationRate", L""), m_rotationRate);

		m_structure.ExportToPropertyTree(rootGlyphPropertyTree);

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

		glyph.GetPosition()[0] = m_position[0].GetValue().first;
		glyph.GetPosition()[1] = m_position[1].GetValue().first;
		glyph.GetPosition()[2] = m_position[2].GetValue().first;

		glyph.GetRotation()[0] = m_rotation[0].GetValue().first;
		glyph.GetRotation()[1] = m_rotation[1].GetValue().first;
		glyph.GetRotation()[2] = m_rotation[2].GetValue().first;

		glyph.GetScale()[0] = m_scale[0].GetValue().first;
		glyph.GetScale()[1] = m_scale[1].GetValue().first;
		glyph.GetScale()[2] = m_scale[2].GetValue().first;

		glyph.GetColor() = m_color.GetValue().first;
		glyph.GetTransparency() = m_transparency.GetValue().first;

		glyph.GetTag() = m_tag.GetValue();
		glyph.GetDescription() = m_description.GetValue();

		glyph.GetRotationRate()[0] = m_rotationRate[0].GetValue().first;
		glyph.GetRotationRate()[1] = m_rotationRate[1].GetValue().first;
		glyph.GetRotationRate()[2] = m_rotationRate[2].GetValue().first;

		glyph.GetStructure() = m_structure;

		return glyph;
	}

	SynGlyphX::Glyph DataMappingGlyph::GetDifference() const {

		SynGlyphX::Glyph glyph;

		glyph.GetPosition()[0] = m_position[0].GetValue().second;
		glyph.GetPosition()[1] = m_position[1].GetValue().second;
		glyph.GetPosition()[2] = m_position[2].GetValue().second;

		glyph.GetRotation()[0] = m_rotation[0].GetValue().second;
		glyph.GetRotation()[1] = m_rotation[1].GetValue().second;
		glyph.GetRotation()[2] = m_rotation[2].GetValue().second;

		glyph.GetScale()[0] = m_scale[0].GetValue().second;
		glyph.GetScale()[1] = m_scale[1].GetValue().second;
		glyph.GetScale()[2] = m_scale[2].GetValue().second;

		glyph.GetColor() = m_color.GetValue().second;
		glyph.GetTransparency() = m_transparency.GetValue().second;

		glyph.GetTag() = m_tag.GetValue();
		glyph.GetDescription() = m_description.GetValue();

		glyph.GetRotationRate()[0] = m_rotationRate[0].GetValue().second;
		glyph.GetRotationRate()[1] = m_rotationRate[1].GetValue().second;
		glyph.GetRotationRate()[2] = m_rotationRate[2].GetValue().second;

		glyph.GetStructure() = m_structure;

		return glyph;
	}

	SynGlyphX::Glyph DataMappingGlyph::GetMaxGlyph() const {

		SynGlyphX::Glyph glyph;

		glyph.GetPosition()[0] = m_position[0].GetValue().first + m_position[0].GetValue().second;
		glyph.GetPosition()[1] = m_position[1].GetValue().first + m_position[1].GetValue().second;
		glyph.GetPosition()[2] = m_position[2].GetValue().first + m_position[2].GetValue().second;

		glyph.GetRotation()[0] = m_rotation[0].GetValue().first + m_rotation[0].GetValue().second;
		glyph.GetRotation()[1] = m_rotation[1].GetValue().first + m_rotation[1].GetValue().second;
		glyph.GetRotation()[2] = m_rotation[2].GetValue().first + m_rotation[2].GetValue().second;

		glyph.GetScale()[0] = m_scale[0].GetValue().first + m_scale[0].GetValue().second;
		glyph.GetScale()[1] = m_scale[1].GetValue().first + m_scale[1].GetValue().second;
		glyph.GetScale()[2] = m_scale[2].GetValue().first + m_scale[2].GetValue().second;

		glyph.GetColor().Set(0, m_color.GetValue().first[0] + m_color.GetValue().second[0]);
		glyph.GetColor().Set(1, m_color.GetValue().first[0] + m_color.GetValue().second[1]);
		glyph.GetColor().Set(2, m_color.GetValue().first[0] + m_color.GetValue().second[2]);
		glyph.GetTransparency() = m_transparency.GetValue().first + m_transparency.GetValue().second;

		glyph.GetTag() = m_tag.GetValue();
		glyph.GetDescription() = m_description.GetValue();

		glyph.GetRotationRate()[0] = m_rotationRate[0].GetValue().first + m_rotationRate[0].GetValue().second;
		glyph.GetRotationRate()[1] = m_rotationRate[1].GetValue().first + m_rotationRate[1].GetValue().second;
		glyph.GetRotationRate()[2] = m_rotationRate[2].GetValue().first + m_rotationRate[2].GetValue().second;

		glyph.GetStructure() = m_structure;

		return glyph;
	}

	bool DataMappingGlyph::IsPositionXYBoundToInputFields() const {

		return (m_position[0].GetBinding().IsBoundToInputField() && m_position[1].GetBinding().IsBoundToInputField());
	}

	
	DataMappingGlyph& DataMappingGlyph::operator=(const DataMappingGlyph& glyph) {

		GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>::operator=(glyph);

		return *this;
	}

	bool DataMappingGlyph::operator==(const DataMappingGlyph& glyph) const {

		return GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty>::operator==(glyph);
	}

	bool DataMappingGlyph::operator!=(const DataMappingGlyph& glyph) const {

		return !operator==(glyph);
	}

	void DataMappingGlyph::SetInputBinding(MappableField field, const InputBinding& binding) {

		if (field < MappableField::MappableFieldSize) {

			InputBinding& selectedBinding = const_cast<InputBinding&>(GetInputBinding(field));
			selectedBinding = binding;
		}
		else {
			throw new std::out_of_range("Out of range of number of input fields");
		}
	}

	void DataMappingGlyph::ClearInputBinding(MappableField field) {

		if (field < MappableField::MappableFieldSize) {
			
			InputBinding& selectedBinding = const_cast<InputBinding&>(GetInputBinding(field));
			selectedBinding.Clear();
		}
		else {
			throw new std::out_of_range("Out of range of number of input fields");
		}
	}

	const InputBinding& DataMappingGlyph::GetInputBinding(MappableField field) const {

		if (field == MappableField::PositionX) {

			return m_position[0].GetBinding();
		}
		else if (field == MappableField::PositionY) {

			return m_position[1].GetBinding();
		}
		else if (field == MappableField::PositionZ) {

			return m_position[2].GetBinding();
		}
		else if (field == MappableField::RotationX) {

			return m_rotation[0].GetBinding();
		}
		else if (field == MappableField::RotationY) {

			return m_rotation[1].GetBinding();
		}
		else if (field == MappableField::RotationZ) {

			return m_rotation[2].GetBinding();
		}
		else if (field == MappableField::ScaleX) {

			return m_scale[0].GetBinding();
		}
		else if (field == MappableField::ScaleY) {

			return m_scale[1].GetBinding();
		}
		else if (field == MappableField::ScaleZ) {

			return m_scale[2].GetBinding();
		}
		else if (field == MappableField::Color) {

			return m_color.GetBinding();
		}
		else if (field == MappableField::Transparency) {

			return m_transparency.GetBinding();
		}
		else if (field == MappableField::Tag) {

			return m_tag.GetBinding();
		}
		else if (field == MappableField::Description) {

			return m_description.GetBinding();
		}
		else if (field == MappableField::RotationRateX) {

			return m_rotationRate[0].GetBinding();
		}
		else if (field == MappableField::RotationRateY) {

			return m_rotationRate[1].GetBinding();
		}
		else if (field == MappableField::RotationRateZ) {

			return m_rotationRate[2].GetBinding();
		}
	}

} //namespace SynGlyphX