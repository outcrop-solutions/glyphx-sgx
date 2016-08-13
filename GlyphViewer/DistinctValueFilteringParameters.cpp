#include "DistinctValueFilteringParameters.h"
#include "datasource.h"
#include "sourcedatacache.h"
#include <boost/uuid/uuid_io.hpp>

DistinctValueFilteringParameters::DistinctValueFilteringParameters()
{
}

DistinctValueFilteringParameters::DistinctValueFilteringParameters(const DistinctValueFilteringParameters& filters) :
	m_distinctValuesFilters(filters.m_distinctValuesFilters) {


}

DistinctValueFilteringParameters::~DistinctValueFilteringParameters()
{
}

DistinctValueFilteringParameters& DistinctValueFilteringParameters::operator=(const DistinctValueFilteringParameters& filters) {

	m_distinctValuesFilters = filters.m_distinctValuesFilters;

	return *this;
}

bool DistinctValueFilteringParameters::operator==(const DistinctValueFilteringParameters& filters) {

	if (m_distinctValuesFilters != filters.m_distinctValuesFilters) {

		return false;
	}

	return true;
}

bool DistinctValueFilteringParameters::operator!=(const DistinctValueFilteringParameters& filters) {

	return (!operator==(filters));
}

void DistinctValueFilteringParameters::Clear() {

	m_distinctValuesFilters.clear();
}

bool DistinctValueFilteringParameters::HasFilters() const {

	return (!m_distinctValuesFilters.empty());
}

void DistinctValueFilteringParameters::SetDistinctValueFilters(const ColumnDistinctValuesFilterMap& filterMap) {

	m_distinctValuesFilters = filterMap;
}

void DistinctValueFilteringParameters::SetDistinctValueFilter(const QString& column, const QSet<QString>& distinctValues) {

	m_distinctValuesFilters[column] = distinctValues;
}

void DistinctValueFilteringParameters::RemoveDistinctValueFilter(const QString& column) {

	m_distinctValuesFilters.erase(column);
}

const DistinctValueFilteringParameters::ColumnDistinctValuesFilterMap& DistinctValueFilteringParameters::GetDistinctValueFilters() const {

	return m_distinctValuesFilters;
}

QString DistinctValueFilteringParameters::GenerateQuery(const SynGlyphX::InputTable& table) const {

	QString query = "SELECT * FROM \"" + QString::fromStdWString(boost::uuids::to_wstring(table.GetDatasourceID()));
	if (table.GetTable() != SynGlyphX::Datasource::SingleTableName) {

		query += ":" + QString::fromStdWString(table.GetTable());
	}

	query += "\" WHERE ";

	for (const auto& filter : m_distinctValuesFilters) {

		query += SourceDataCache::CreateInString(filter.first, filter.second);
	}

	return query;
}