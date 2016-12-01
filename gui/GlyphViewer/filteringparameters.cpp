#include "filteringparameters.h"


FilteringParameters::FilteringParameters() :
	DistinctValueFilteringParameters() {

}

FilteringParameters::FilteringParameters(const FilteringParameters& filters) :
	DistinctValueFilteringParameters(filters),
	m_keywordFilters(filters.m_keywordFilters),
    m_rangeFilters(filters.m_rangeFilters)
{
}

FilteringParameters::~FilteringParameters()
{
}

FilteringParameters& FilteringParameters::operator=(const FilteringParameters& filters) {

	DistinctValueFilteringParameters::operator=(filters);
	m_rangeFilters = filters.m_rangeFilters;
	m_keywordFilters = filters.m_keywordFilters;

	return *this;
}

bool FilteringParameters::operator==(const FilteringParameters& filters) {

	if (DistinctValueFilteringParameters::operator!=(filters)) {

		return false;
	}

	if (m_rangeFilters != filters.m_rangeFilters) {

		return false;
	}

	if (m_keywordFilters != filters.m_keywordFilters) {

		return false;
	}

	return true;
}

bool FilteringParameters::operator!=(const FilteringParameters& filters) {

	return (!operator==(filters));
}

void FilteringParameters::Clear() {

	DistinctValueFilteringParameters::Clear();
	m_rangeFilters.clear();
	m_keywordFilters.clear();
}

bool FilteringParameters::HasFilters() const {

	return (DistinctValueFilteringParameters::HasFilters() || !m_rangeFilters.empty() || !m_keywordFilters.empty());
}

void FilteringParameters::SetRangeFilters(const ColumnRangeFilterMap& filterMap) {

	m_rangeFilters = filterMap;
}

void FilteringParameters::SetRangeFilter(const QString& column, const SynGlyphX::DegenerateIntervalUnion& rangeFilter) {

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

const FilteringParameters::ColumnRangeFilterMap& FilteringParameters::GetRangeFilters() const {

	return m_rangeFilters;
}

void FilteringParameters::SetKeywordFilters(const ColumnKeywordFilterMap& filterMap) {

	m_keywordFilters = filterMap;
}

void FilteringParameters::SetKeywordFilter(const QString& column, const MultiKeywordFilter& keywordFilter) {

	for (auto& filter : m_keywordFilters) {

		if (filter.first == column) {

			filter.second = keywordFilter;
			return;
		}
	}

	m_keywordFilters.push_back(ColumnKeywordFilter(column, keywordFilter));
}

void FilteringParameters::RemoveKeywordFilter(const QString& column) {

	for (ColumnKeywordFilterMap::iterator iT = m_keywordFilters.begin(); iT != m_keywordFilters.end(); ++iT) {

		if (iT->first == column) {

			m_keywordFilters.erase(iT);
			return;
		}
	}
}

const FilteringParameters::ColumnKeywordFilterMap& FilteringParameters::GetKeywordFilters() const {

	return m_keywordFilters;
}
