#include "filteringmanager.h"

FilteringManager::FilteringManager(SynGlyphX::DataMappingModel* dataMappingModel, SourceDataCache::SharedPtr sourceDataCache, SynGlyphX::ItemFocusSelectionModel* sceneSelectionModel, QObject *parent)
	: QObject(parent),
	m_dataMappingModel(dataMappingModel),
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

	m_glyphTemplateRangeToTableMap.clear();
	m_tableToGlyphTreeRangesMap.clear();

	if (m_sceneSelectionModel->model()->rowCount() > 0) {

		QMap<QString, unsigned long> countsForEachTable;
		for (const auto& glyphGraph : m_dataMappingModel->GetDataMapping()->GetGlyphGraphs()) {

			if (glyphGraph.second->IsTransformable()) {

				QString tableName = SourceDataCache::CreateTablename(glyphGraph.second->GetInputFields().begin()->second);
				if (countsForEachTable.count(tableName) == 0) {

					countsForEachTable[tableName] = m_sourceDataCache->GetNumberOfRowsInTable(tableName);
				}
			}
		}

		unsigned long startingIndex = 0;
		for (const auto& glyphGraph : m_dataMappingModel->GetDataMapping()->GetGlyphGraphs()) {

			if (glyphGraph.second->IsTransformable()) {

				QString tableName = SourceDataCache::CreateTablename(glyphGraph.second->GetInputFields().begin()->second);
				SynGlyphX::ProperInterval range(startingIndex, startingIndex + countsForEachTable.value(tableName), SynGlyphX::ProperInterval::Min);
				startingIndex += countsForEachTable.value(tableName);

				m_tableToGlyphTreeRangesMap.insert(tableName, range);

				m_glyphTemplateRangeToTableMap[range] = tableName;
			}
		}
	}
}

void FilteringManager::SetFilterResultsForTable(const QString& table, const SourceDataCache::ColumnIntervalMap& columnRanges, bool updateFocus) {

	if (columnRanges.empty()) {

		ClearFilterResultsForTable(table);
	}
	else {

		SetFilterResultsForTable(table, m_sourceDataCache->GetIndexesFromTableInRanges(table, columnRanges), updateFocus);
	}
}

void FilteringManager::SetFilterResultsForTable(const QString& table, const SourceDataCache::ColumnValueData& sourceData, bool updateFocus) {

	if (sourceData.empty()) {

		ClearFilterResultsForTable(table);
	}
	else {

		SetFilterResultsForTable(table, m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, sourceData), updateFocus);
	}
}

void FilteringManager::SetFilterResultsForTable(const QString& table, const SynGlyphX::IndexSet& newSelectionSet, bool updateFocus) {

	SynGlyphX::IndexSet intersectionOfOldAndNewSelectionSet;
	SynGlyphX::IndexSet deselectIndexSet;
	SynGlyphX::IndexSet selectIndexSet;
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
}

void FilteringManager::ClearFilterResults() {

	m_filterResultsByTable.clear();
	m_filterResultsIndexedToGlyphs.clear();
	emit FilterResultsChanged(SynGlyphX::IndexSet());
}

void FilteringManager::ClearFilterResultsForTable(const QString& table, bool updateFocus) {

	if (m_filterResultsByTable.count(table) == 0) {

		throw std::invalid_argument("Can't remove table from source data selection that isn't part of the selection");
	}

	QItemSelection itemSelection;
	AddSceneIndexesFromTableToSelection(itemSelection, table);
	ClearSourceDataSelectionForTable(itemSelection, updateFocus);
	m_filterResultsByTable.erase(table);
	UpdateGlyphIndexedFilterResults();
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

	m_filterResultsIndexedToGlyphs.clear();

	for (TableToGlyphTemplateRangesMap::iterator tableRange = m_tableToGlyphTreeRangesMap.begin(); tableRange != m_tableToGlyphTreeRangesMap.end(); ++tableRange) {

		unsigned int min = static_cast<unsigned int>(tableRange.value().GetMin());
		for (auto row : m_filterResultsByTable.at(tableRange.key())) {

			m_filterResultsIndexedToGlyphs.insert(row + min);
		}
	}

	emit FilterResultsChanged(m_filterResultsIndexedToGlyphs);
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

const SynGlyphX::DataMappingModel* FilteringManager::GetDataMappingModel() const {

	return m_dataMappingModel;
}
/*
void FilteringManager::OnSceneSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	m_selectedSourceDataSets.clear();
	const QItemSelection& selection = m_sceneSelectionModel->selection();
	if (!selection.isEmpty()) {

		SynGlyphX::IndexSet rootIndexRows = SynGlyphX::ItemFocusSelectionModel::GetRootRows(selection.indexes());
		SynGlyphX::IndexSet::iterator startOfTableRange = rootIndexRows.begin();
		for (const auto& range : m_glyphTemplateRangeToTableMap) {

			SynGlyphX::IndexSet::iterator endOfTableRange = rootIndexRows.lower_bound(static_cast<unsigned long>(range.first.GetMax()));
			if (startOfTableRange != endOfTableRange) {

				SynGlyphX::IndexSet indexesForTable;
				for (SynGlyphX::IndexSet::iterator iT = startOfTableRange; iT != endOfTableRange; ++iT) {

					indexesForTable.insert(*iT - range.first.GetMin());
				}
				m_selectedSourceDataSets[range.second] = indexesForTable;
				if (endOfTableRange != rootIndexRows.end()) {

					startOfTableRange = endOfTableRange;
				}
				else {

					break;
				}
			}
		}
	}

	emit SelectionChanged();
}*/