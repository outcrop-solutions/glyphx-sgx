#include "selectedsourcedatawidget.h"

SelectedSourceDataWidget::SelectedSourceDataWidget(const SynGlyphX::ItemFocusSelectionModel* selectionModel,
													SourceDataCache::ConstSharedPtr sourceDataCache,
													SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping,
													QWidget *parent)
	: SourceDataWidget(sourceDataCache, dataTransformMapping, parent),
	m_selectionModel(selectionModel)
{
	setWindowTitle(tr("Source Data Of Selected Glyphs"));
	QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &SelectedSourceDataWidget::OnSelectionChanged);
	QObject::connect(m_selectionModel->model(), &QAbstractItemModel::modelReset, this, &SelectedSourceDataWidget::OnNewVisualization);
}

SelectedSourceDataWidget::~SelectedSourceDataWidget()
{

}

void SelectedSourceDataWidget::OnNewVisualization() {

	m_glyphTemplateRangeToTableMap.clear();

	if (m_selectionModel->model()->rowCount() > 0) {

		QMap<QString, unsigned long> countsForEachTable;
		for (const auto& glyphGraph : m_dataTransformMapping->GetGlyphGraphs()) {

			if (glyphGraph.second->IsTransformable()) {

				QString tableName = SourceDataCache::CreateTablename(glyphGraph.second->GetInputFields().begin()->second);
				if (countsForEachTable.count(tableName) == 0) {

					countsForEachTable[tableName] = m_sourceDataCache->GetNumberOfRowsInTable(tableName);
				}
			}
		}

		unsigned long startingIndex = 0;
		for (const auto& glyphGraph : m_dataTransformMapping->GetGlyphGraphs()) {

			if (glyphGraph.second->IsTransformable()) {

				QString tableName = SourceDataCache::CreateTablename(glyphGraph.second->GetInputFields().begin()->second);
				SynGlyphX::DegenerateInterval range(startingIndex, startingIndex + countsForEachTable[tableName] - 1);
				startingIndex += countsForEachTable[tableName];

				m_glyphTemplateRangeToTableMap[range] = tableName;
			}
		}
	}
}

void SelectedSourceDataWidget::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	m_selectedIndexesPerTable.clear();
	const QItemSelection& selection = m_selectionModel->selection();
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
				m_selectedIndexesPerTable[range.second] = indexesForTable;
				if (endOfTableRange != rootIndexRows.end()) {

					startOfTableRange = endOfTableRange;
				}
				else {

					break;
				}
			}
		}

		UpdateTables();
	}
	else {

		ClearTables();
	}
}

SynGlyphX::IndexSet SelectedSourceDataWidget::GetSourceIndexesForTable(const QString& table) {

	if (m_selectedIndexesPerTable.contains(table)) {

		return m_selectedIndexesPerTable[table];
	}
	else {

		return SynGlyphX::IndexSet();
	}
}