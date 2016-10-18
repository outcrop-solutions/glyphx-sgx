#include "filteringmanager.h"
#include "datatransformmodel.h"
#include <boost/optional.hpp>

FilteringManager::FilteringManager(SynGlyphX::DataTransformModel* DataTransformModel, SourceDataCache::SharedPtr sourceDataCache, SynGlyphX::ItemFocusSelectionModel* sceneSelectionModel, QObject *parent)
	: QObject(parent),
	m_DataTransformModel(DataTransformModel),
	m_sourceDataCache(sourceDataCache),
	m_sceneSelectionModel(sceneSelectionModel)
{
	QObject::connect(m_sceneSelectionModel->model(), &QAbstractItemModel::modelReset, this, &FilteringManager::OnSceneModelReset);
	//QObject::connect(m_sceneSelectionModel, &QItemSelectionModel::selectionChanged, this, &FilteringManager::OnSceneSelectionChanged);
}

FilteringManager::~FilteringManager()
{

}

const SynGlyphX::ItemFocusSelectionModel* FilteringManager::GetSceneSelectionModel() const {

	return m_sceneSelectionModel;
}

void FilteringManager::OnSceneModelReset() {

	Clear();

	if (m_sceneSelectionModel->model()->rowCount() > 0) {

		QMap<QString, unsigned long> countsForEachTable;
		for (const auto& glyphGraph : m_DataTransformModel->GetDataMapping()->GetGlyphGraphs()) {

			if (glyphGraph.second->IsTransformable()) {
				QString tableName = 
					SourceDataCache::CreateTablename(m_DataTransformModel->GetDataMapping()->GetInputTable(glyphGraph.first));
				if (countsForEachTable.count(tableName) == 0) {

					countsForEachTable[tableName] = m_sourceDataCache->GetNumberOfRowsInTable(tableName);
				}
			}
		}

		unsigned long startingIndex = 0;
		for (const auto& glyphGraph : m_DataTransformModel->GetDataMapping()->GetGlyphGraphs()) {

			if (glyphGraph.second->IsTransformable()) {
				QString tableName = 
					SourceDataCache::CreateTablename(m_DataTransformModel->GetDataMapping()->GetInputTable(glyphGraph.first));
				SynGlyphX::ProperInterval range(startingIndex, startingIndex + countsForEachTable.value(tableName), SynGlyphX::ProperInterval::Min);
				startingIndex += countsForEachTable.value(tableName);

				m_tableToGlyphTreeRangesMap.insert(tableName, range);

				m_glyphTemplateRangeToTableMap[range] = tableName;
			}
		}
	}
}

bool FilteringManager::GenerateFilterResultsForTable(const QString& table, const FilteringParameters& filters, bool updateFocus) {

	if (!filters.HasFilters()) {

		ClearFiltersForTable(table);
		return true;
	}
	else {

		m_filtersForEachTable[table] = filters;

		SynGlyphX::IndexSet newSelectionSet;
		SynGlyphX::IndexSet intersectionOfOldAndNewSelectionSet;
		SynGlyphX::IndexSet deselectIndexSet;
		SynGlyphX::IndexSet selectIndexSet;

		if (m_filterResultsPerTableFromLoadingFilter.count(table) > 0) {

			SynGlyphX::IndexSet newFilterSelectionSet = m_sourceDataCache->GetIndexesFromTableThatPassFilters(table, filters);
			std::set_intersection(m_filterResultsPerTableFromLoadingFilter[table].begin(), m_filterResultsPerTableFromLoadingFilter[table].end(),
				newFilterSelectionSet.begin(), newFilterSelectionSet.end(),
				std::inserter(newSelectionSet, newSelectionSet.end()));
		}
		else {

			newSelectionSet = m_sourceDataCache->GetIndexesFromTableThatPassFilters(table, filters);
		}

		if (newSelectionSet.empty()) {

			return false;
		}

		if (m_filterResultsByTable.count(table) > 0) {

			std::set_intersection(m_filterResultsByTable[table].begin(), m_filterResultsByTable[table].end(),
				newSelectionSet.begin(), newSelectionSet.end(),
				std::inserter(intersectionOfOldAndNewSelectionSet, intersectionOfOldAndNewSelectionSet.end()));

			if (intersectionOfOldAndNewSelectionSet.empty()) {

				deselectIndexSet = m_filterResultsByTable[table];
				selectIndexSet = newSelectionSet;
			}
			else {

				std::set_difference(m_filterResultsByTable[table].begin(), m_filterResultsByTable[table].end(),
					intersectionOfOldAndNewSelectionSet.begin(), intersectionOfOldAndNewSelectionSet.end(),
					std::inserter(deselectIndexSet, deselectIndexSet.end()));
				std::set_difference(newSelectionSet.begin(), newSelectionSet.end(),
					intersectionOfOldAndNewSelectionSet.begin(), intersectionOfOldAndNewSelectionSet.end(),
					std::inserter(selectIndexSet, selectIndexSet.end()));
			}
		}
		else {

			selectIndexSet = newSelectionSet;
		}

		if (updateFocus) {

			if (!deselectIndexSet.empty()) {

				QItemSelection deselection;
				AddSceneIndexesToSelection(deselection, table, deselectIndexSet);
				ClearSourceDataSelectionForTable(deselection, selectIndexSet.empty());
			}

			if (!selectIndexSet.empty()) {

				QItemSelection itemSelection;
				AddSceneIndexesToSelection(itemSelection, table, selectIndexSet);

				m_sceneSelectionModel->select(itemSelection, QItemSelectionModel::Select);
				if (!itemSelection.empty()) {

					m_sceneSelectionModel->SetFocus(itemSelection.indexes(), SynGlyphX::ItemFocusSelectionModel::FocusFlag::Focus);
				}
			}
		}

		m_filterResultsByTable[table] = newSelectionSet;
		UpdateGlyphIndexedFilterResults();
		return true;
	}
}

void FilteringManager::GenerateLoadingFilterResultsForTable(const QString& table, const FilteringParameters::ColumnDistinctValuesFilterMap& filters) {

	m_filterResultsPerTableFromLoadingFilter[table] = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, filters);

	if (m_filtersForEachTable.count(table) > 0) {

		m_filterResultsByTable[table].clear();
		SynGlyphX::IndexSet filterResults = m_sourceDataCache->GetIndexesFromTableThatPassFilters(table, m_filtersForEachTable[table]);
		std::set_intersection(m_filterResultsPerTableFromLoadingFilter[table].begin(), m_filterResultsPerTableFromLoadingFilter[table].end(),
			filterResults.begin(), filterResults.end(),
			std::inserter(m_filterResultsByTable[table], m_filterResultsByTable[table].end()));
	}
	else {

		m_filterResultsByTable[table] = m_filterResultsPerTableFromLoadingFilter[table];
	}

	UpdateGlyphIndexedFilterResults();
}

void FilteringManager::SetFilterIndexesForTable(const QString& table, const SynGlyphX::IndexSet& filterSet, bool updateFocus) {

	if (m_filterResultsPerTableFromLoadingFilter.count(table) > 0) {

		m_filterResultsByTable[table].clear();
		std::set_intersection(m_filterResultsPerTableFromLoadingFilter[table].begin(), m_filterResultsPerTableFromLoadingFilter[table].end(),
			filterSet.begin(), filterSet.end(),
			std::inserter(m_filterResultsByTable[table], m_filterResultsByTable[table].end()));
	}
	else {

		m_filterResultsByTable[table] = filterSet;
	}

	if (m_filtersForEachTable.count(table) > 0) {

		m_filtersForEachTable.remove(table);
	}

	UpdateGlyphIndexedFilterResults();
}

void FilteringManager::Clear() {

	m_filterResultsByTable.clear();
	m_filtersForEachTable.clear();

	m_glyphTemplateRangeToTableMap.clear();
	m_tableToGlyphTreeRangesMap.clear();
	m_filterResultsPerTableFromLoadingFilter.clear();

	m_filterResultsIndexedToGlyphs.clear();
}

void FilteringManager::ClearAllFilters() {

	if (m_filterResultsPerTableFromLoadingFilter.empty()) {

		m_filterResultsByTable.clear();
	}
	else {

		m_filterResultsByTable = m_filterResultsPerTableFromLoadingFilter;
	}

	m_filtersForEachTable.clear();

	UpdateGlyphIndexedFilterResults();
}

void FilteringManager::ClearFiltersForTable(const QString& table, bool updateFocus) {

	if (m_filterResultsByTable.count(table) > 0) {

		QItemSelection itemSelection;
		AddSceneIndexesFromTableToSelection(itemSelection, table);
		ClearSourceDataSelectionForTable(itemSelection, updateFocus);
		if (m_filterResultsPerTableFromLoadingFilter.count(table) > 0) {

			m_filterResultsByTable[table] = m_filterResultsPerTableFromLoadingFilter[table];
		}
		else {

			m_filterResultsByTable.erase(table);
		}

		m_filtersForEachTable.remove(table);
		UpdateGlyphIndexedFilterResults();
	}
}

const FilteringManager::Table2FiltersMap& FilteringManager::GetTable2FiltersMap() const {

	return m_filtersForEachTable;
}

const FilteringManager::TableToGlyphTemplateRangesMap& FilteringManager::GetTableToGlyphTemplateRangesMap() const {

	return m_tableToGlyphTreeRangesMap;
}

void FilteringManager::ClearSourceDataSelectionForTable(QItemSelection& itemSelection, bool updateFocus) {

	m_sceneSelectionModel->select(itemSelection, QItemSelectionModel::Deselect);
	if (updateFocus && (!itemSelection.empty())) {

		m_sceneSelectionModel->SetFocus(itemSelection.indexes(), SynGlyphX::ItemFocusSelectionModel::FocusFlag::Unfocus);
	}
}

void FilteringManager::AddSceneIndexesFromTableToSelection(QItemSelection& selection, const QString& table) {

	AddSceneIndexesToSelection(selection, table, m_filterResultsByTable.at(table));
}

void FilteringManager::AddSceneIndexesToSelection(QItemSelection& selection, const QString& table, const SynGlyphX::IndexSet& indexSet) {

	const QAbstractItemModel* modelForSourceSelection = m_sceneSelectionModel->model();
	const auto& ranges = m_tableToGlyphTreeRangesMap.values(table);
	
	for (const auto& range : ranges) {

		unsigned int min = static_cast<unsigned int>(range.GetMin());
		for (auto row : indexSet) {

			QModelIndex index = modelForSourceSelection->index(row + min, 0);
			selection.select(index, index);
		}
	}
}

void FilteringManager::UpdateGlyphIndexedFilterResults() {

	SynGlyphX::IndexSet newfilterResultsIndexedToGlyphs;

	for (TableToGlyphTemplateRangesMap::iterator tableRange = m_tableToGlyphTreeRangesMap.begin(); tableRange != m_tableToGlyphTreeRangesMap.end(); ++tableRange) {

		unsigned int min = static_cast<unsigned int>(tableRange.value().GetMin());
		if (m_filterResultsByTable.count(tableRange.key()) > 0) {
		
			for (auto row : m_filterResultsByTable.at(tableRange.key())) {

				newfilterResultsIndexedToGlyphs.insert(row + min);
			}
		}
		//Uncomment if it is decided to show other tables when a filter operates only on one table
		/*else {

			for (unsigned long i = min; i < static_cast<unsigned int>(tableRange.value().GetMax()); ++i) {

				newfilterResultsIndexedToGlyphs.insert(i);
			}
		}*/
	}

	if (newfilterResultsIndexedToGlyphs != m_filterResultsIndexedToGlyphs) {

		m_filterResultsIndexedToGlyphs = newfilterResultsIndexedToGlyphs;
		emit FilterResultsChanged(m_filterResultsIndexedToGlyphs);
	}
}

const FilteringManager::IndexSetMap& FilteringManager::GetFilterResultsByTable() const {

	return m_filterResultsByTable;
}

const SynGlyphX::IndexSet& FilteringManager::GetGlyphIndexedFilterResults() const {

	return m_filterResultsIndexedToGlyphs;
}

SourceDataCache::ConstSharedPtr FilteringManager::GetSourceDataCache() const {

	return m_sourceDataCache;
}

const SynGlyphX::DataTransformModel* FilteringManager::GetDataTransformModel() const {

	return m_DataTransformModel;
}

boost::optional<std::pair<unsigned int, unsigned long>> FilteringManager::GetGlyphTemplateAndTableIndex(unsigned long rootID) const {

	unsigned int index = 0;
	for (const auto& range : m_glyphTemplateRangeToTableMap) {

		if (range.first.IsValueInInterval(rootID)) {

			return std::pair<unsigned int, unsigned long>(index, rootID - range.first.GetMin());
		}

		++index;
	}

	return boost::none;
}