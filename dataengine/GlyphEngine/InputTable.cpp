#include "InputTable.h"
#include "DataSource.h"
#include "hashid.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace GlyphEngine {

	InputTable::InputTable()
	{
	}

	InputTable::InputTable(const boost::uuids::uuid& datasourceID, const std::wstring& table) :
		m_datasourceID(datasourceID),
		m_table(table) {

		//Since some datasources are single table make sure table has a non-empty value
		if (m_table.empty()) {

			m_table = DataSource::SingleTableName;
		}
	}

	InputTable::InputTable(const boost::property_tree::wptree& propertyTree) {

		m_datasourceID = propertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id");
		m_table = propertyTree.get<std::wstring>(L"<xmlattr>.table");

		//Since some datasources are single table make sure table has a non-empty value
		if (m_table.empty()) {

			m_table = DataSource::SingleTableName;
		}
	}
	
	InputTable::InputTable(const InputTable& inputTable) :
		m_datasourceID(inputTable.m_datasourceID),
		m_table(inputTable.m_table) {


	}

	InputTable::~InputTable()
	{
	}

	InputTable& InputTable::operator=(const InputTable& inputTable) {

		m_datasourceID = inputTable.m_datasourceID;
		m_table = inputTable.m_table;

		return *this;
	}

	bool InputTable::operator==(const InputTable& inputTable) const {

		if (m_datasourceID != inputTable.m_datasourceID) {

			return false;
		}

		if (m_table != inputTable.m_table) {

			return false;
		}

		return true;
	}

	bool InputTable::operator!=(const InputTable& inputField) const {

		return !operator==(inputField);
	}

	const boost::uuids::uuid& InputTable::GetDatasourceID() const {

		return m_datasourceID;
	}

	const std::wstring& InputTable::GetTable() const {

		return m_table;
	}

	bool InputTable::IsValid() const {

		return !(m_datasourceID.is_nil() || m_table.empty());
	}

	HashID InputTable::GetHashID() const {

        HashID seed = 0;

		if (IsValid()) {

			CombineHashID(seed, m_datasourceID);
			CombineHashID(seed, m_table);
		}

		return seed;
	}

} 