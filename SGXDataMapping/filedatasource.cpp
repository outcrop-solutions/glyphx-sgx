#include "filedatasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/filesystem.hpp>
#include "stringconvert.h"

namespace SynGlyphX {

	const FileDatasource::FileTypeBimap FileDatasource::s_fileTypeStrings = boost::assign::list_of < FileDatasource::FileTypeBimap::relation >
		(FileDatasource::FileType::SQLITE3, L"SQLITE3")
		(FileDatasource::FileType::CSV, L"CSV")
		(FileDatasource::FileType::KML, L"KML/KMZ")
		(FileDatasource::FileType::XML, L"XML")
		(FileDatasource::FileType::JSON, L"JSON");

	FileDatasource::FileDatasource(FileType type, const std::wstring& filename, const std::wstring& host, unsigned int port, const std::wstring& username, const std::wstring& password) :
		Datasource(filename, host, port, username, password),
		m_fileType(type)
	{
		//CSV files are a single table so put in a dummy value so that the table count is 1
		if (!CanDatasourceHaveMultipleTables()) {
		
			Datasource::TableNames tables;
			tables.push_back(SingleTableName);
			AddTables(tables);
		}

		boost::filesystem::path datasourcePath(filename);
		m_formattedName = datasourcePath.filename().wstring();
	}

	FileDatasource::FileDatasource(const PropertyTree& propertyTree) :
		Datasource(propertyTree),
		m_fileType(s_fileTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {

		//CSV files are a single table so put in a dummy value so that the table count is 1
		if (!CanDatasourceHaveMultipleTables()) {

			m_tables.clear();
			Datasource::TableNames tables;
			tables.push_back(SingleTableName);
			AddTables(tables);
		}

		boost::filesystem::path datasourcePath(m_dbName);
		m_formattedName = datasourcePath.filename().wstring();
	}

	FileDatasource::FileDatasource(const FileDatasource& datasource) :
		Datasource(datasource),
		m_fileType(datasource.m_fileType),
		m_formattedName(datasource.m_formattedName) {


	}

	FileDatasource::~FileDatasource()
	{
	}

	FileDatasource& FileDatasource::operator=(const FileDatasource& datasource) {

		Datasource::operator=(datasource);
		m_fileType = datasource.m_fileType;
		m_formattedName = datasource.m_formattedName;

		return *this;
	}

	bool FileDatasource::operator==(const FileDatasource& datasource) const {

		if (Datasource::operator!=(datasource)) {

			return false;
		}

		if (m_fileType != datasource.m_fileType) {

			return false;
		}

		return true;
	}

	bool FileDatasource::operator!=(const FileDatasource& datasource) const {

		return !operator==(datasource);
	}

	Datasource::SourceType FileDatasource::GetSourceType() const {

		return Datasource::SourceType::File;
	}

	FileDatasource::FileType FileDatasource::GetFileType() const {

		return m_fileType;
	}

	const std::wstring& FileDatasource::GetFilename() const {

		return m_dbName;
	}

	void FileDatasource::ChangeFilename(const std::wstring& filename) {

		m_dbName = filename;
	}

	bool FileDatasource::RequiresConversionToDB() const {

		return (m_fileType != FileType::SQLITE3);
	}

	bool FileDatasource::IsOriginalDatasourceADatabase() const {

		return ((m_fileType == FileType::SQLITE3) || (m_fileType == FileType::CSV));
	}

	bool FileDatasource::CanDatasourceHaveMultipleTables() const {

		return (m_fileType == FileType::SQLITE3);
	}

	bool FileDatasource::IsFile() const {

		return true;
	}

	bool FileDatasource::CanDatasourceBeFound() const {

		boost::filesystem::path datasourcePath(m_dbName);
		return (boost::filesystem::exists(datasourcePath) && boost::filesystem::is_regular_file(datasourcePath));
	}

	FileDatasource::PropertyTree& FileDatasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = Datasource::ExportToPropertyTree(parentPropertyTree);
		propertyTree.put(L"<xmlattr>.type", s_fileTypeStrings.left.at(m_fileType));

		return propertyTree;
	}

	const std::wstring& FileDatasource::GetFormattedName() const {

		return m_formattedName;
	}

	FileDatasource::FileType FileDatasource::GetFileTypeForFile(const std::wstring& filename) {

		if (filename.substr(filename.length() - 4) == L".csv") {

			return FileDatasource::FileType::CSV;
		}
		else if (IsSQLite3DB(filename)) {

			return FileDatasource::FileType::SQLITE3;
		}
		else {

			throw std::invalid_argument("Data source file is not a supported type.");
		}
	}

	bool FileDatasource::IsSQLite3DB(const std::wstring& filename) {

		const char* first16BytesOfSQLite3DB = "SQLite format 3";
		std::ifstream fileStream(StringConvert::ToStdString(filename), std::ifstream::in | std::ifstream::binary);
		if (fileStream.is_open()) {

			char first16bytes[16];
			fileStream.read(first16bytes, 16);

			return (memcmp(first16bytes, first16BytesOfSQLite3DB, 16) == 0);
		}

		return false;
	}

} //namespace SynGlyphX