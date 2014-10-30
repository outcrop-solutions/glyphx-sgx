#include "datamappingdefaults.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<DataMappingDefaults::TagFieldPropertyDefault, std::wstring> DataMappingDefaults::s_tagFieldStrings = boost::assign::list_of < boost::bimap<DataMappingDefaults::TagFieldPropertyDefault, std::wstring>::relation >
		(DataMappingDefaults::TagFieldPropertyDefault::None, L"None")
		(DataMappingDefaults::TagFieldPropertyDefault::PositionX, L"Position X")
		(DataMappingDefaults::TagFieldPropertyDefault::PositionY, L"Position Y")
		(DataMappingDefaults::TagFieldPropertyDefault::PositionZ, L"Position Z")
		(DataMappingDefaults::TagFieldPropertyDefault::RotationX, L"Rotation X")
		(DataMappingDefaults::TagFieldPropertyDefault::RotationY, L"Rotation Y")
		(DataMappingDefaults::TagFieldPropertyDefault::RotationZ, L"Rotation Z")
		(DataMappingDefaults::TagFieldPropertyDefault::ScaleX, L"Scale X")
		(DataMappingDefaults::TagFieldPropertyDefault::ScaleY, L"Scale Y")
		(DataMappingDefaults::TagFieldPropertyDefault::ScaleZ, L"Scale Z")
		(DataMappingDefaults::TagFieldPropertyDefault::Color, L"Color")
		(DataMappingDefaults::TagFieldPropertyDefault::Transparency, L"Transparency");

	DataMappingDefaults::DataMappingDefaults() :
		m_tagField(TagFieldPropertyDefault::None),
		m_defaultTagValue(L"No Tag")
	{
	}

	DataMappingDefaults::DataMappingDefaults(const PropertyTree& propertyTree) :
		m_tagField(s_tagFieldStrings.right.at(propertyTree.get<std::wstring>(L"TagFieldDefault"))),
		m_defaultTagValue(propertyTree.get<std::wstring>(L"TagValueDefault")) {


	}

	DataMappingDefaults::DataMappingDefaults(const DataMappingDefaults& defaults) :
		m_tagField(defaults.m_tagField),
		m_defaultTagValue(defaults.m_defaultTagValue) {


	}

	DataMappingDefaults::~DataMappingDefaults()
	{
	}

	DataMappingDefaults& DataMappingDefaults::operator=(const DataMappingDefaults& defaults) {

		m_tagField = defaults.m_tagField;
		m_defaultTagValue = defaults.m_defaultTagValue;

		return *this;
	}

	bool DataMappingDefaults::operator==(const DataMappingDefaults& defaults) const {

		if (m_tagField != defaults.m_tagField) {

			return false;
		}

		if (m_defaultTagValue != defaults.m_defaultTagValue) {

			return false;
		}

		return true;
	}

	bool DataMappingDefaults::operator!=(const DataMappingDefaults& defaults) const {

		return !operator==(defaults);
	}

	void DataMappingDefaults::Clear() {

		m_tagField = TagFieldPropertyDefault::None;
		m_defaultTagValue = L"No Tag";
	}

	void DataMappingDefaults::ExportToPropertyTree(PropertyTree& parentPropertyTree) const {

		PropertyTree& propertyTree = parentPropertyTree.add(L"Defaults", L"");
		propertyTree.put(L"TagFieldDefault", s_tagFieldStrings.left.at(m_tagField));
		propertyTree.put(L"TagValueDefault", m_defaultTagValue);
	}

	void DataMappingDefaults::SetTagField(TagFieldPropertyDefault tagField) {

		m_tagField = tagField;
	}

	DataMappingDefaults::TagFieldPropertyDefault DataMappingDefaults::GetTagField() const {

		return m_tagField;
	}

	void DataMappingDefaults::SetDefaultTagValue(const std::wstring& value) {

		m_defaultTagValue = value;
	}

	const std::wstring& DataMappingDefaults::GetDefaultTagValue() const {

		return m_defaultTagValue;
	}

} //namespace SynGlyphX