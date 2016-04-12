#include "filteringparameters.h"


FilteringParameters::FilteringParameters()
{
}


FilteringParameters::~FilteringParameters()
{
}

void FilteringParameters::Clear() {

	m_distinctValuesFilters.clear();
	m_rangeFilters.clear();
}

void FilteringParameters::SetDistinctValueFilter(const QString& column, const QSet<QString>& distinctValues) {

	m_distinctValuesFilters[column] = distinctValues;
}

void FilteringParameters::RemoveDistinctValueFilter(const QString& column) {

	m_distinctValuesFilters.erase(column);
}

void FilteringParameters::SetRangeFilter(const QString& column, const SynGlyphX::DegenerateInterval& rangeFilter) {

	for (auto& filter : m_rangeFilters) {

		if (filter.first == column) {

			filter.second = rangeFilter;
			return;
		}
	}

	m_rangeFilters.push_back(ColumnRangeFilter(column, rangeFilter));
}

void FilteringParameters::RemoveRangeFilter(const QString& column) {

	for (ColumnRangeFilterMap::iterator iT = m_rangeFilters.begin(); iT != m_rangeFilters.end(); ++iT) {

		if (iT->first == column) {

			m_rangeFilters.erase(iT);
			return;
		}
	}
}