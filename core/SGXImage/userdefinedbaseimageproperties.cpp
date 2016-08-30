#include "userdefinedbaseimageproperties.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

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

	bool UserDefinedBaseImageProperties::operator==(const UserDefinedBaseImageProperties& properties) const {

		return m_filename == properties.m_filename;
	}

	bool UserDefinedBaseImageProperties::operator!=(const UserDefinedBaseImageProperties& properties) const {

		return !operator==(properties);
	}

	const std::wstring& UserDefinedBaseImageProperties::GetFilename() const {

		return m_filename;
	}

	void UserDefinedBaseImageProperties::SetFilename(const std::wstring& filename) {

		m_filename = filename;
	}

	bool UserDefinedBaseImageProperties::IsGeographic() const {

		return false;
	}

	void UserDefinedBaseImageProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put(L"<xmlattr>.filename", m_filename);
	}

	bool UserDefinedBaseImageProperties::CanFileBeFound() const {

		boost::filesystem::path filePath(m_filename);
		return (boost::filesystem::exists(filePath) && boost::filesystem::is_regular_file(filePath));
	}

} //namespace SynGlyphX