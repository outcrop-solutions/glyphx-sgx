#include "userdefinedbaseimageproperties.h"

namespace SynGlyphX {

	UserDefinedBaseImageProperties::UserDefinedBaseImageProperties(const std::wstring& filename) :
		BaseImageProperties(),
		m_filename(filename)
	{
	}

	UserDefinedBaseImageProperties::UserDefinedBaseImageProperties(const boost::property_tree::wptree& propertyTree) :
		BaseImageProperties(propertyTree),
		m_filename(propertyTree.get<std::wstring>(L"<xmlattr>.filename")) {


	}

	UserDefinedBaseImageProperties::UserDefinedBaseImageProperties(const UserDefinedBaseImageProperties& properties) :
		BaseImageProperties(properties),
		m_filename(properties.m_filename) {


	}

	UserDefinedBaseImageProperties::~UserDefinedBaseImageProperties()
	{
	}

	const std::wstring& UserDefinedBaseImageProperties::GetFilename() const {

		return m_filename;
	}

	bool UserDefinedBaseImageProperties::IsGeographic() const {

		return false;
	}

	void UserDefinedBaseImageProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		BaseImageProperties::ExportToPropertyTree(propertyTree);
		propertyTree.put(L"<xmlattr>.filename", m_filename);
	}

} //namespace SynGlyphX