#include "Data.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace GlyphEngine {

	Data::Data(const boost::property_tree::wptree& datasourcePropertyTree)
	{
		m_id = datasourcePropertyTree.get_optional<std::wstring>(L"<xmlattr>.id").get_value_or(L"0");
		m_source = datasourcePropertyTree.get_optional<std::wstring>(L"<xmlattr>.source").get_value_or(L"File") == L"File" ? SourceType::File : SourceType::DatabaseServer;

		if (m_source == SourceType::File) {

			m_host = datasourcePropertyTree.get<std::wstring>(L"Host", L"");
			m_table = L"OnlyTable";
		}
		else if (m_source == SourceType::DatabaseServer) {
			m_host = datasourcePropertyTree.get<std::wstring>(L"Host", L"");
			m_username = datasourcePropertyTree.get<std::wstring>(L"Username", L"");
			m_password = datasourcePropertyTree.get<std::wstring>(L"Password", L"");
		}
	}

	void Data::AddInputField(const boost::property_tree::wptree& inputFieldPropertyTree)
	{
		std::wstring m_name = inputFieldPropertyTree.get<std::wstring>(L"<xmlattr>.name");
		std::wstring m_field = inputFieldPropertyTree.get<std::wstring>(L"<xmlattr>.field");
		std::wstring m_type = inputFieldPropertyTree.get<std::wstring>(L"<xmlattr>.type");

		m_inputFields.insert(std::make_pair(m_name, InputField(m_name, m_field, m_type)));
	}

	int Data::GetInputFieldCount()
	{
		return m_inputFields.size();
	}

	const Data::SourceType Data::GetSourceType() const {

		return m_source;
	}

	const std::wstring& Data::GetId() const {

		return m_id;
	}

    const std::wstring& Data::GetHost() const {

        return m_host;
    }

    const std::wstring& Data::GetUsername() const {

        return m_username;
    }

    const std::wstring& Data::GetPassword() const {

        return m_password;
    }


} //namespace GlyphEngine