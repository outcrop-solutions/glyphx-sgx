#ifndef SYNGLYPHX_DATASOURCE
#define SYNGLYPHX_DATASOURCE

#include "sgxdatatransform_global.h"
#include <string>
#include <vector>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT Datasource
	{
	public:
        Datasource(const std::wstring& dbName, const std::wstring& type, const std::wstring& host = L"localhost", unsigned int port = 0, const std::wstring& username = L"", const std::wstring& password = L"");
        Datasource(const Datasource& datasource);
        ~Datasource();

        Datasource& operator=(const Datasource& datasource);

        const std::wstring& GetDBName() const;
        const std::wstring& GetType() const;
        const std::wstring& GetHost() const;
        unsigned int GetPort() const;
        const std::wstring& GetUsername() const;
        const std::wstring& GetPassword() const;
        const std::vector<std::wstring>& GetTables() const;

    private:
        std::wstring m_dbName;
        std::wstring m_type;
        std::wstring m_host;
        unsigned int m_port;
        std::wstring m_username;
        std::wstring m_password;
        std::vector<std::wstring> m_tables;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATASOURCE
