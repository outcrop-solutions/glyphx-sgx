#include "sourcedataselectionmodel.h"

SourceDataSelectionModel::SourceDataSelectionModel(SynGlyphX::DataMappingModel* dataMappingModel, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SynGlyphX::ItemFocusSelectionModel* sceneSelectionModel, QObject *parent)
	: QObject(parent),
	m_dataMappingModel(dataMappingModel),
	m_sourceDataCache(sourceDataCache),
	m_sceneSelectionModel(sceneSelectionModel)
{
	QObject::connect(m_sceneSelectionModel->model(), &QAbstractItemModel::modelReset, this, &SourceDataSelectionModel::OnSceneModelReset);
}

SourceDataSelectionModel::~SourceDataSelectionModel()
{

}

void SourceDataSelectionModel::OnSceneModelReset() {

	std::unordered_map<QString, unsigned long> countsForEachTable;
	for (const auto& glyphGraph : m_dataMappingModel->GetDataMapping()->GetGlyphGraphs()) {

		if (!glyphGraph.second->IsTransformable()) {

			QString tableName = SynGlyphX::SourceDataCache::CreateTablename(glyphGraph.second->GetInputFields().begin()->second);
			if (countsForEachTable.count(tableName) == 0) {

				countsForEachTable[tableName] = m_sourceDataCache->GetNumberOfRowsInTable(tableName);
			}
		}
	}

	m_glyphTemplateRangeToTableMap.clear();
	m_tableToGlyphTreeRangesMap.clear();

	unsigned long startingIndex = 0;
	for (const auto& glyphGraph : m_dataMappingModel->GetDataMapping()->GetGlyphGraphs()) {

		if (!glyphGraph.second->IsTransformable()) {

			QString tableName = SynGlyphX::SourceDataCache::CreateTablename(glyphGraph.second->GetInputFields().begin()->second);
			SynGlyphX::Range range(startingIndex, startingIndex + countsForEachTable.at(tableName));
			startingIndex += countsForEachTable.at(tableName);

			std::pair<QString, SynGlyphX::Range> newPair(tableName, range);
			m_tableToGlyphTreeRangesMap.insert(newPair);

			m_glyphTemplateRangeToTableMap[range] = tableName;
		}
	}
}

void SourceDataSelectionModel::SetNewSourceDataSelection(const SynGlyphX::SourceDataCache::IndexSetMap& selectedIndexMap) {


}

void SourceDataSelectionModel::SetSourceDataSelectionForTable(const QString& table, const SynGlyphX::IndexSet& newSelectionSet) {

	if (m_selectedSourceDataSets.count(table) > 0) {

		ClearSourceDataSelectionForTable(table);
	}
	
	QItemSelection itemSelection;
	AddSceneIndexesFromTableToSelection(itemSelection, table);

	m_sceneSelectionModel->select(itemSelection, QItemSelectionModel::Select);
	m_sceneSelectionModel->SetFocus(itemSelection.indexes(), SynGlyphX::ItemFocusSelectionModel::FocusFlag::Focus);
	m_selectedSourceDataSets[table] = newSelectionSet;
}

void SourceDataSelectionModel::ClearSourceDataSelection() {

	m_sceneSelectionModel->ClearAll();
	m_selectedSourceDataSets.clear();
}

void SourceDataSelectionModel::ClearSourceDataSelectionForTable(const QString& table) {

	if (m_selectedSourceDataSets.count(table) == 0) {

		throw std::invalid_argument("Can't remove table from source data selection that isn't part of the selection");
	}

	QItemSelection itemSelection;
	AddSceneIndexesFromTableToSelection(itemSelection, table);

	m_sceneSelectionModel->select(itemSelection, QItemSelectionModel::Deselect);
	m_sceneSelectionModel->SetFocus(itemSelection.indexes(), SynGlyphX::ItemFocusSelectionModel::FocusFlag::Unfocus);
	m_selectedSourceDataSets.erase(table);
}

void SourceDataSelectionModel::AddSceneIndexesFromTableToSelection(QItemSelection& selection, const QString& table) {

	const QAbstractItemModel* modelForSourceSelection = m_sceneSelectionModel->model();
	std::pair<std::unordered_multimap<QString, SynGlyphX::Range>::iterator, std::unordered_multimap<QString, SynGlyphX::Range>::iterator> ranges = m_tableToGlyphTreeRangesMap.equal_range(table);
	for (auto row : m_selectedSourceDataSets[table]) {

		for (std::unordered_multimap<QString, SynGlyphX::Range>::iterator range = ranges.first; range != ranges.second; ++range) {

			QModelIndex index = modelForSourceSelection->index(row + range->second.GetMin(), 0);
			selection.select(index, index);
		}
	}
}

const SynGlyphX::SourceDataCache::IndexSetMap& SourceDataSelectionModel::GetSourceDataSelection() const {

	return m_selectedSourceDataSets;
}

void SourceDataSelectionModel::OnSceneSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

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
}