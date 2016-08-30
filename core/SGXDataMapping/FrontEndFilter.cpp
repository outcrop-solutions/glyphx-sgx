#include "FrontEndFilter.h"

namespace SynGlyphX {

	FrontEndFilterOptions::FrontEndFilterOptions() :
		m_isRequired(true),
		m_muliselectAllowed(false) {


	}

	FrontEndFilterOptions::FrontEndFilterOptions(bool required, bool allowMultiselect) :
		m_isRequired(required),
		m_muliselectAllowed(allowMultiselect)  {


	}

	FrontEndFilterOptions::FrontEndFilterOptions(const FrontEndFilterOptions& options) :
		m_isRequired(options.m_isRequired),
		m_muliselectAllowed(options.m_muliselectAllowed) {

	}

	FrontEndFilterOptions::~FrontEndFilterOptions() {


	}

	FrontEndFilterOptions& FrontEndFilterOptions::operator=(const FrontEndFilterOptions& options) {

		m_isRequired = options.m_isRequired;
		m_muliselectAllowed = options.m_muliselectAllowed;

		return *this;
	}

	bool FrontEndFilterOptions::operator==(const FrontEndFilterOptions& options) const {

		return ((m_isRequired == options.m_isRequired) && (m_muliselectAllowed == options.m_muliselectAllowed));
	}

	bool FrontEndFilterOptions::operator!=(const FrontEndFilterOptions& options) const {

		return !operator==(options);
	}

	void FrontEndFilterOptions::SetRequired(bool required) {

		m_isRequired = required;
	}

	bool FrontEndFilterOptions::IsRequired() const {

		return m_isRequired;
	}

	void FrontEndFilterOptions::SetAllowMultiselect(bool allow) {

		m_muliselectAllowed = allow;
	}

	bool FrontEndFilterOptions::IsMultiselectAllowed() const {

		return m_muliselectAllowed;
	}

	/*FrontEndFilterManager::FrontEndFilterManager()
	{
	}


	FrontEndFilterManager::~FrontEndFilterManager()
	{
	}

	void FrontEndFilterManager::Clear() {

		m_filtersForAllTables.clear();
	}

	void FrontEndFilterManager::Remove(const InputField& field) {

		MultiTableFrontEndFilters::iterator tableIterator = m_filtersForAllTables.find(field);
		if (tableIterator != m_filtersForAllTables.end()) {

			SingleTableFrontEndFilters::iterator iT = tableIterator->second.find(field.GetField());
			if (iT != tableIterator->second.end()) {

				tableIterator->second.erase(iT);
			}
			if (tableIterator->second.empty()) {

				m_filtersForAllTables.erase(tableIterator);
			}
		}
	}

	void FrontEndFilterManager::Add(const InputField& field, const FrontEndFilterOptions& options) {

		if (HasFilter(field)) {

			throw std::invalid_argument("Field already exists within front end filter list.");
		}

		MultiTableFrontEndFilters::iterator tableIterator = m_filtersForAllTables.find(field);
		if (tableIterator == m_filtersForAllTables.end()) {

			SingleTableFrontEndFilters newTable;
			newTable[field.GetField()] = options;
			m_filtersForAllTables[field] = newTable;
		}
		else {

			tableIterator->second[field.GetField()] = options;
		}
	}

	void FrontEndFilterManager::SetOptions(const InputField& field, const FrontEndFilterOptions& options) {

		if (!HasFilter(field)) {

			throw std::invalid_argument("Field is not in front end filter list.");
		}

		MultiTableFrontEndFilters::iterator tableIterator = m_filtersForAllTables.find(field);
		if (tableIterator != m_filtersForAllTables.end()) {

			tableIterator->second[field.GetField()] = options;
		}
	}

	bool FrontEndFilterManager::HasFilter(const InputField& field) const {

		MultiTableFrontEndFilters::const_iterator tableIterator = m_filtersForAllTables.find(field);
		if (tableIterator != m_filtersForAllTables.end()) {

			return (tableIterator->second.count(field.GetField()) != 0);
		}

		return false;
	}

	const FrontEndFilterManager::MultiTableFrontEndFilters& FrontEndFilterManager::GetFilters() const {

		return m_filtersForAllTables;
	}

	const FrontEndFilterManager::SingleTableFrontEndFilters& FrontEndFilterManager::GetFiltersForTable(const InputTable& table) const {

		MultiTableFrontEndFilters::const_iterator tableIterator = m_filtersForAllTables.find(table);
		if (tableIterator == m_filtersForAllTables.end()) {

			throw std::invalid_argument("Table can't be found within front end filter list.");
		}

		return tableIterator->second;
	}*/

} //namespace SynGlyphX