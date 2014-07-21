#ifndef SYNGLYPHX_DATASOURCE
#define SYNGLYPHX_DATASOURCE

#include "sgxdatatransform_global.h"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>
#include <boost/bimap.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT Datasource
	{
	public:
		enum SourceType {
			SQLITE3,
			ODBC,
			CSV,
			KML
		};

		typedef boost::bimap<SourceType, std::wstring> SourceTypeBimap;

		Datasource(const std::wstring& dbName, SourceType type, const std::wstring& host = L"localhost", unsigned int port = 0, const std::wstring& username = L"", const std::wstring& password = L"");
		Datasource(const boost::property_tree::wptree& propertyTree);
        Datasource(const Datasource& datasource);
        ~Datasource();

        Datasource& operator=(const Datasource& datasource);

        const std::wstring& GetDBName() const;
		SourceType GetType() const;
        const std::wstring& GetHost() const;
        unsigned int GetPort() const;
        const std::wstring& GetUsername() const;
        const std::wstring& GetPassword() const;
        const std::vector<std::wstring>& GetTables() const;

		void AddTables(const std::vector<std::wstring>& table);

		bool IsOriginalDatasourceADatabase() const;

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree);

		static const SourceTypeBimap s_sourceTypeStrings;

    private:
        std::wstring m_dbName;
		SourceType m_type;
        std::wstring m_host;
        unsigned int m_port;
        std::wstring m_username;
        std::wstring m_password;
        std::vector<std::wstring> m_tables;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATASOURCE
