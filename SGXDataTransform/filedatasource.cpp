#include "filedatasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/filesystem.hpp>
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
		//CSV files are a single table so put in a dummy value so that the table count is 1
		if (!CanDatasourceHaveMultipleTables()) {
		
			m_tables.insert(SingleTableName);
		}

		boost::filesystem::path datasourcePath(filename);
		m_formattedName = datasourcePath.filename().wstring();
	}

	FileDatasource::FileDatasource(const PropertyTree& propertyTree) :
		Datasource(propertyTree),
		m_type(s_sourceTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {

		boost::filesystem::path datasourcePath(m_dbName);
		m_formattedName = datasourcePath.filename().wstring();
	}

	FileDatasource::FileDatasource(const FileDatasource& datasource) :
		Datasource(datasource),
		m_type(datasource.m_type),
		m_formattedName(datasource.m_formattedName) {


	}

	FileDatasource::~FileDatasource()
	{
	}

	FileDatasource& FileDatasource::operator=(const FileDatasource& datasource) {

		Datasource::operator=(datasource);
		m_type = datasource.m_type;
		m_formattedName = datasource.m_formattedName;

		return *this;
	}

	bool FileDatasource::operator==(const FileDatasource& datasource) const {

		if (Datasource::operator!=(datasource)) {

			return false;
		}

		if (m_type != datasource.m_type) {

			return false;
		}

		return true;
	}

	bool FileDatasource::operator!=(const FileDatasource& datasource) const {

		return !operator==(datasource);
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

		return ((m_type == SourceType::SQLITE3) || (m_type == SourceType::CSV));
	}

	bool FileDatasource::CanDatasourceHaveMultipleTables() const {

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

	const std::wstring& FileDatasource::GetFormattedName() const {

		return m_formattedName;
	}

} //namespace SynGlyphX