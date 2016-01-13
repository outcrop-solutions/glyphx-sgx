#include "datamappingdefaults.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<DataMappingGlyph::MappableField, std::wstring> DataMappingDefaults::s_tagFieldStrings = boost::assign::list_of < boost::bimap<DataMappingGlyph::MappableField, std::wstring>::relation >
		(DataMappingGlyph::MappableField::Tag, L"None")
		(DataMappingGlyph::MappableField::PositionX, L"Position X")
		(DataMappingGlyph::MappableField::PositionY, L"Position Y")
		(DataMappingGlyph::MappableField::PositionZ, L"Position Z")
		(DataMappingGlyph::MappableField::RotationX, L"Rotation X")
		(DataMappingGlyph::MappableField::RotationY, L"Rotation Y")
		(DataMappingGlyph::MappableField::RotationZ, L"Rotation Z")
		(DataMappingGlyph::MappableField::ScaleX, L"Scale X")
		(DataMappingGlyph::MappableField::ScaleY, L"Scale Y")
		(DataMappingGlyph::MappableField::ScaleZ, L"Scale Z")
		(DataMappingGlyph::MappableField::Color, L"Color")
		(DataMappingGlyph::MappableField::Transparency, L"Transparency")
		(DataMappingGlyph::MappableField::Description, L"Description")
		(DataMappingGlyph::MappableField::RotationRateX, L"Rotation Rate X")
		(DataMappingGlyph::MappableField::RotationRateY, L"Rotation Rate Y")
		(DataMappingGlyph::MappableField::RotationRateZ, L"Rotation Rate Z");

	const std::wstring DataMappingDefaults::s_propertyTreeName = L"Defaults";

	DataMappingDefaults::DataMappingDefaults() :
		m_tagField(DataMappingGlyph::MappableField::Tag),
		m_defaultTagValue(L"No Tag"),
		m_removeWhenScaleIsZero(true)
	{
	}

	DataMappingDefaults::DataMappingDefaults(const PropertyTree& propertyTree) :
		m_tagField(s_tagFieldStrings.right.at(propertyTree.get<std::wstring>(L"TagFieldDefault"))),
		m_defaultTagValue(propertyTree.get<std::wstring>(L"TagValueDefault")),
		m_removeWhenScaleIsZero(true) {

		boost::optional<const boost::property_tree::wptree&> scaleDefaultPropertyTree = propertyTree.get_child_optional(L"ScaleZeroDefault");
		if (scaleDefaultPropertyTree.is_initialized()) {

			m_removeWhenScaleIsZero = scaleDefaultPropertyTree.get().get_optional<bool>(L"<xmlattr>.remove").get_value_or(true);
		}
	}

	DataMappingDefaults::DataMappingDefaults(const DataMappingDefaults& defaults) :
		m_tagField(defaults.m_tagField),
		m_defaultTagValue(defaults.m_defaultTagValue),
		m_removeWhenScaleIsZero(defaults.m_removeWhenScaleIsZero) {


	}

	DataMappingDefaults::~DataMappingDefaults()
	{
	}

	DataMappingDefaults& DataMappingDefaults::operator=(const DataMappingDefaults& defaults) {

		m_tagField = defaults.m_tagField;
		m_defaultTagValue = defaults.m_defaultTagValue;
		m_removeWhenScaleIsZero = defaults.m_removeWhenScaleIsZero;

		return *this;
	}

	bool DataMappingDefaults::operator==(const DataMappingDefaults& defaults) const {

		if (m_tagField != defaults.m_tagField) {

			return false;
		}

		if (m_defaultTagValue != defaults.m_defaultTagValue) {

			return false;
		}

		if (m_removeWhenScaleIsZero != defaults.m_removeWhenScaleIsZero) {

			return false;
		}

		return true;
	}

	bool DataMappingDefaults::operator!=(const DataMappingDefaults& defaults) const {

		return !operator==(defaults);
	}

	void DataMappingDefaults::Clear() {

		m_tagField = DataMappingGlyph::MappableField::Tag;
		m_defaultTagValue = L"No Tag";
		m_removeWhenScaleIsZero = true;
	}

	void DataMappingDefaults::ExportToPropertyTree(PropertyTree& parentPropertyTree) const {

		PropertyTree& propertyTree = parentPropertyTree.add(s_propertyTreeName, L"");
		propertyTree.put(L"TagFieldDefault", s_tagFieldStrings.left.at(m_tagField));
		propertyTree.put(L"TagValueDefault", m_defaultTagValue);

		PropertyTree& scaleDefaultPropertyTree = propertyTree.add(L"ScaleZeroDefault", L"");
		scaleDefaultPropertyTree.put(L"<xmlattr>.remove", m_removeWhenScaleIsZero);
	}

	void DataMappingDefaults::SetTagField(DataMappingGlyph::MappableField tagField) {

		m_tagField = tagField;
	}

	DataMappingGlyph::MappableField DataMappingDefaults::GetTagField() const {

		return m_tagField;
	}

	bool DataMappingDefaults::IsDefaultTagFieldSet() const {

		return (m_tagField != DataMappingGlyph::MappableField::Tag);
	}

	void DataMappingDefaults::SetDefaultTagValue(const std::wstring& value) {

		m_defaultTagValue = value;
	}

	const std::wstring& DataMappingDefaults::GetDefaultTagValue() const {

		return m_defaultTagValue;
	}

	void DataMappingDefaults::SetRemoveWhenScaleIsZero(bool removeWhenScaleIsZero) {

		m_removeWhenScaleIsZero = removeWhenScaleIsZero;
	}

	bool DataMappingDefaults::GetRemoveWhenScaleIsZero() const {

		return m_removeWhenScaleIsZero;
	}

} //namespace SynGlyphX