#include "filedatasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	const FileDatasource::SourceTypeBimap FileDatasource::s_sourceTypeStrings = boost::assign::list_of < FileDatasource::SourceTypeBimap::relation >
		(FileDatasource::SourceType::SQLITE3, L"SQLITE3")
		(FileDatasource::SourceType::CSV, L"CSV")
		(FileDatasource::SourceType::KML, L"KML/KMZ");

	FileDatasource::FileDatasource(SourceType type, const std::wstring& filename, const std::wstring& host, unsigned int port, const std::wstring& username, const std::wstring& password) :
		Datasource(filename, host, port, username, password),
		m_type(type)
	{
	}

	FileDatasource::FileDatasource(const PropertyTree& propertyTree) :
		Datasource(propertyTree),
		m_type(s_sourceTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {


	}

	FileDatasource::FileDatasource(const FileDatasource& datasource) :
		Datasource(datasource),
		m_type(datasource.m_type) {


	}

	FileDatasource::~FileDatasource()
	{
	}

	FileDatasource& FileDatasource::operator=(const FileDatasource& datasource) {

		Datasource::operator=(datasource);
		m_type = datasource.m_type;

		return *this;
	}

	FileDatasource::SourceType FileDatasource::GetType() const {

		return m_type;
	}

	const std::wstring& FileDatasource::GetFilename() const {

		return m_dbName;
	}

	void FileDatasource::ChangeFilename(const std::wstring& filename) {

		m_dbName = filename;
	}

	bool FileDatasource::RequiresConversionToDB() const {

		return (m_type != SourceType::SQLITE3);
	}

	bool FileDatasource::IsOriginalDatasourceADatabase() const {

		return (m_type == SourceType::SQLITE3);
	}

	bool FileDatasource::IsFile() const {

		return true;
	}

	bool FileDatasource::CanDatasourceBeFound() const {

		boost::filesystem::path datasourcePath(m_dbName);
		return (boost::filesystem::exists(datasourcePath) && boost::filesystem::is_regular_file(datasourcePath));
	}

	FileDatasource::PropertyTree& FileDatasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = Datasource::ExportToPropertyTree(parentPropertyTree, L"File");
		propertyTree.put(L"<xmlattr>.type", s_sourceTypeStrings.left.at(m_type));

		return propertyTree;
	}

} //namespace SynGlyphX