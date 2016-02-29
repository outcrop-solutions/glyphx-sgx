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

	const FileDatasource::FileTypeBimap FileDatasource::s_fileTypePrefixes = boost::assign::list_of < FileDatasource::FileTypeBimap::relation >
		(FileDatasource::FileType::SQLITE3, L"sqlite3")
		(FileDatasource::FileType::CSV, L"csv");

	FileDatasource::FileDatasource(FileType type, const std::wstring& filename, const std::wstring& host, const std::wstring& username, const std::wstring& password) :
		Datasource(host, username, password),
		m_fileType(type),
		m_filename(filename)
	{
		//CSV files are a single table so put in a dummy value so that the table count is 1
		if (!CanDatasourceHaveMultipleTables()) {
		
			Datasource::TableNames tables;
			tables.push_back(SingleTableName);
			AddTables(tables);
		}
	}

	FileDatasource::FileDatasource(const PropertyTree& propertyTree) :
		Datasource(propertyTree),
		m_filename(propertyTree.get<std::wstring>(L"Name")),
		m_fileType(s_fileTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {

		//CSV files are a single table so put in a dummy value so that the table count is 1
		if (!CanDatasourceHaveMultipleTables()) {

			m_tables.clear();
			Datasource::TableNames tables;
			tables.push_back(SingleTableName);
			AddTables(tables);
		}
	}

	FileDatasource::FileDatasource(const FileDatasource& datasource) :
		Datasource(datasource),
		m_fileType(datasource.m_fileType),
		m_filename(datasource.m_filename) {


	}

	FileDatasource::~FileDatasource()
	{
	}

	FileDatasource& FileDatasource::operator=(const FileDatasource& datasource) {

		Datasource::operator=(datasource);
		m_fileType = datasource.m_fileType;
		m_filename = datasource.m_filename;

		return *this;
	}

	bool FileDatasource::operator==(const FileDatasource& datasource) const {

		if (Datasource::operator!=(datasource)) {

			return false;
		}

		if (m_fileType != datasource.m_fileType) {

			return false;
		}

		if (m_filename != datasource.m_filename) {

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

		return m_filename;
	}

	void FileDatasource::ChangeFilename(const std::wstring& filename) {

		m_filename = filename;
	}

	bool FileDatasource::RequiresConversionToDB() const {

		return (m_fileType != FileType::SQLITE3);
	}

	bool FileDatasource::IsOriginalDatasourceADatabase() const {

		return ((m_fileType == FileType::SQLITE3) || (m_fileType == FileType::CSV));
	}

	bool FileDatasource::CanDatasourceHaveMultipleTables() const {

		return CanFileTypeHaveMultipleTables(m_fileType);
	}

	bool FileDatasource::CanFileTypeHaveMultipleTables(FileType fileType) {

		return (fileType == FileType::SQLITE3);
	}

	bool FileDatasource::IsFile() const {

		return true;
	}

	bool FileDatasource::CanDatasourceBeFound() const {

		boost::filesystem::path datasourcePath(m_filename);
		return (boost::filesystem::exists(datasourcePath) && boost::filesystem::is_regular_file(datasourcePath));
	}

	FileDatasource::PropertyTree& FileDatasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = Datasource::ExportToPropertyTree(parentPropertyTree);
		propertyTree.put(L"<xmlattr>.type", s_fileTypeStrings.left.at(m_fileType));
		propertyTree.put(L"Name", m_filename);

		return propertyTree;
	}

	std::wstring FileDatasource::GetFormattedName() const {

		boost::filesystem::path datasourcePath(m_filename);
		return datasourcePath.filename().wstring();
	}

	FileDatasource::FileType FileDatasource::GetFileTypeForFile(const std::wstring& filename) {

		std::wstring extension = boost::filesystem::path(filename).extension().native();
		if (extension == L".csv") {

			return FileDatasource::FileType::CSV;
		}
		else if (IsSQLite3DB(filename)) {

			return FileDatasource::FileType::SQLITE3;
		}
		else {

			throw std::invalid_argument("File is not recognized as a supported data source type.");
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

	std::wstring FileDatasource::GetDBName() const {

		return m_filename;
	}

} //namespace SynGlyphX