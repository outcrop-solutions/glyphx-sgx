#include "FileDataSource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/filesystem.hpp>
#include "StringConvert.h"
#include <boost/property_tree/ptree.hpp>
#include "DataSourceTable.h"

namespace GlyphEngine {

	const FileDataSource::FileTypeBimap FileDataSource::s_fileTypeStrings = boost::assign::list_of < FileDataSource::FileTypeBimap::relation >
		(FileDataSource::FileType::SQLITE3, L"SQLITE3")
		(FileDataSource::FileType::CSV, L"CSV")
		(FileDataSource::FileType::KML, L"KML/KMZ")
		(FileDataSource::FileType::XML, L"XML")
		(FileDataSource::FileType::JSON, L"JSON");

	const FileDataSource::FileTypeBimap FileDataSource::s_fileTypePrefixes = boost::assign::list_of < FileDataSource::FileTypeBimap::relation >
		(FileDataSource::FileType::SQLITE3, L"sqlite3")
		(FileDataSource::FileType::CSV, L"csv");

	FileDataSource::FileDataSource(FileType type, const std::wstring& host, const std::wstring& username, const std::wstring& password) :
		DataSource(host, username, password),
		m_fileType(type)
	{
		//CSV files are a single table so put in a dummy value so that the table count is 1
		if (!CanDataSourceHaveMultipleTables()) {
		
			DataSource::TableNames tables;
			tables.push_back(SingleTableName);
			AddTables(tables);
		}
	}

	FileDataSource::FileDataSource(const PropertyTree& propertyTree) :
		DataSource(propertyTree),
		m_fileType(s_fileTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {

		if (m_host == L"localhost") {

			m_host = propertyTree.get<std::wstring>(L"Name");
		}

		//CSV files are a single table so put in a dummy value so that the table count is 1
		if (!CanDataSourceHaveMultipleTables()) {

			m_tables.clear();
			DataSource::TableNames tables;
			tables.push_back(SingleTableName);
			AddTables(tables);
		}
	}

	FileDataSource::FileDataSource(const FileDataSource& datasource) :
		DataSource(datasource),
		m_fileType(datasource.m_fileType) {


	}

	FileDataSource::~FileDataSource()
	{
	}

	FileDataSource& FileDataSource::operator=(const FileDataSource& datasource) {

		DataSource::operator=(datasource);
		m_fileType = datasource.m_fileType;

		return *this;
	}

	bool FileDataSource::operator==(const FileDataSource& datasource) const {

		if (DataSource::operator!=(datasource)) {

			return false;
		}

		if (m_fileType != datasource.m_fileType) {

			return false;
		}

		return true;
	}

	bool FileDataSource::operator!=(const FileDataSource& datasource) const {

		return !operator==(datasource);
	}

	DataSource::SourceType FileDataSource::GetSourceType() const {

		return DataSource::SourceType::File;
	}

	FileDataSource::FileType FileDataSource::GetFileType() const {

		return m_fileType;
	}

	const std::wstring& FileDataSource::GetFilename() const {

		return GetHost();
	}

	void FileDataSource::ChangeFilename(const std::wstring& filename) {

		m_host = filename;
	}

	bool FileDataSource::RequiresConversionToDB() const {

		return (m_fileType != FileType::SQLITE3);
	}

	bool FileDataSource::IsOriginalDataSourceADatabase() const {

		return (m_fileType == FileType::SQLITE3);
	}

	bool FileDataSource::CanDataSourceHaveMultipleTables() const {

		return CanFileTypeHaveMultipleTables(m_fileType);
	}

	bool FileDataSource::CanFileTypeHaveMultipleTables(FileType fileType) {

		return (fileType == FileType::SQLITE3);
	}

	bool FileDataSource::IsFile() const {

		return true;
	}

	bool FileDataSource::CanDataSourceBeFound() const {

		boost::filesystem::path datasourcePath(GetFilename());
		return (boost::filesystem::exists(datasourcePath) && boost::filesystem::is_regular_file(datasourcePath));
	}

	FileDataSource::PropertyTree& FileDataSource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = DataSource::ExportToPropertyTree(parentPropertyTree);
		propertyTree.put(L"<xmlattr>.type", s_fileTypeStrings.left.at(m_fileType));
		propertyTree.put(L"Name", GetHost());

		return propertyTree;
	}

	std::wstring FileDataSource::GetFormattedName() const {

		boost::filesystem::path datasourcePath(GetFilename());
		return datasourcePath.stem().wstring();
	}

	FileDataSource::FileType FileDataSource::GetFileTypeForFile(const std::wstring& filename) {

#ifdef _WIN32
		auto path = boost::filesystem::path(filename);
		std::wstring extension = path.extension().native();
#else
        std::wstring extension = StringConvert::ToStdWString(boost::filesystem::path(filename).extension().native());
#endif
        if (extension == L".csv") {

			return FileDataSource::FileType::CSV;
		}
		else if (IsSQLite3DB(filename)) {

			return FileDataSource::FileType::SQLITE3;
		}
		else {

			throw std::invalid_argument("File is not recognized as a supported data source type.");
		}
	}

	bool FileDataSource::IsSQLite3DB(const std::wstring& filename) {

		const char* first16BytesOfSQLite3DB = "SQLite format 3";
		std::ifstream fileStream(StringConvert::ToStdString(filename), std::ifstream::in | std::ifstream::binary);
		if (fileStream.is_open()) {

			char first16bytes[16];
			fileStream.read(first16bytes, 16);

			return (memcmp(first16bytes, first16BytesOfSQLite3DB, 16) == 0);
		}

		return false;
	}

	std::wstring FileDataSource::GetDBName() const {

		return GetHost();
	}

} //namespace SynGlyphX