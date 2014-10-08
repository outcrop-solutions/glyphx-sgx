#ifndef SYNGLYPHX_DATASOURCE
#define SYNGLYPHX_DATASOURCE

#include "sgxdatatransform_global.h"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <unordered_set>
#include <boost/bimap.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT Datasource
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;
		typedef std::unordered_set<std::wstring> TableSet;

		Datasource(const std::wstring& dbName, const std::wstring& host = L"localhost", unsigned int port = 0, const std::wstring& username = L"", const std::wstring& password = L"");
		Datasource(const PropertyTree& propertyTree);
        Datasource(const Datasource& datasource);
        virtual ~Datasource();

        Datasource& operator=(const Datasource& datasource);
		bool operator==(const Datasource& datasource) const;
		bool operator!=(const Datasource& datasource) const;

        const std::wstring& GetDBName() const;
        const std::wstring& GetHost() const;
        unsigned int GetPort() const;
        const std::wstring& GetUsername() const;
        const std::wstring& GetPassword() const;
		const TableSet& GetTables() const;

		void EnableTables(const TableSet& table, bool enable = true);

		virtual bool IsOriginalDatasourceADatabase() const = 0;
		virtual bool CanDatasourceHaveMultipleTables() const = 0;
		virtual bool IsFile() const = 0;
		virtual bool CanDatasourceBeFound() const = 0;
		virtual const std::wstring& GetFormattedName() const = 0;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree, const std::wstring& parentName);

    protected:
        std::wstring m_dbName;
        std::wstring m_host;
        unsigned int m_port;
        std::wstring m_username;
        std::wstring m_password;
		TableSet m_tables;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATASOURCE
