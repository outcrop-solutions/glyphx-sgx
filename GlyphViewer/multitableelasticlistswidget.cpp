#include "multitableelasticlistswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include "elasticlistwidget.h"

MultiTableElasticListsWidget::MultiTableElasticListsWidget(SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	m_elasticListsStackLayout = new QStackedLayout(this);
	m_elasticListsStackLayout->setContentsMargins(0, 0, 0, 0);
	ClearElasticLists();

	setLayout(m_elasticListsStackLayout);

	QObject::connect(m_selectionModel, &SourceDataSelectionModel::SelectionChanged, this, &MultiTableElasticListsWidget::OnSelectionChanged);
	QObject::connect(m_selectionModel->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &MultiTableElasticListsWidget::OnModelReset);
}

MultiTableElasticListsWidget::~MultiTableElasticListsWidget()
{

}

void MultiTableElasticListsWidget::OnSelectionChanged() {

	UpdateElasticListsAndSourceDataWidget();
}

void MultiTableElasticListsWidget::OnModelReset() {

	ClearElasticLists();
	if (m_sourceDataCache->IsValid()) {

		const SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			SingleTableElasticListsWidget* elasticListsWidgetForTable = new SingleTableElasticListsWidget(m_sourceDataCache, formattedName.first, this);
			m_elasticListsStackLayout->addWidget(elasticListsWidgetForTable);
			m_elasticListWidgetsForEachTable[formattedName.first.toStdWString()] = elasticListsWidgetForTable;
			QObject::connect(elasticListsWidgetForTable, &SingleTableElasticListsWidget::SelectionChanged, this, &MultiTableElasticListsWidget::OnElasticListsSelectionChanged);
		}

		UpdateElasticListsAndSourceDataWidget();
	}
}

void MultiTableElasticListsWidget::SwitchTable(const QString& table) {

	m_elasticListsStackLayout->setCurrentWidget(m_elasticListWidgetsForEachTable.at(table.toStdWString()));
}

void MultiTableElasticListsWidget::UpdateElasticListsAndSourceDataWidget() {

	const SourceDataSelectionModel::IndexSetMap& sourceDataSelectionSets = m_selectionModel->GetSourceDataSelection();
	bool isSelectionNotEmpty = !sourceDataSelectionSets.empty();
	if (isSelectionNotEmpty) {

		//m_sourceDataWindow->UpdateTables(sourceDataSelectionSets);
		UpdateElasticLists(sourceDataSelectionSets);
	}
	else {

		UpdateElasticLists();
	}
}

void MultiTableElasticListsWidget::UpdateElasticLists(const SourceDataSelectionModel::IndexSetMap& dataIndexes) {

	for (auto table : m_sourceDataCache->GetFormattedNames()) {

		int c = m_sourceDataCache->GetNumberOfRowsInTable(table.first);
		SingleTableElasticListsWidget* SingleTableElasticListsWidget = m_elasticListWidgetsForEachTable.at(table.first.toStdWString());
		SourceDataSelectionModel::IndexSetMap::const_iterator dataIndexesForTable = dataIndexes.find(table.first);
		if (dataIndexesForTable == dataIndexes.end()) {

			SingleTableElasticListsWidget->PopulateElasticLists();
		}
		else {

			SingleTableElasticListsWidget->PopulateElasticLists(dataIndexesForTable->second);
		}
	}
}

void MultiTableElasticListsWidget::ClearElasticLists() {

	for (auto widget : m_elasticListWidgetsForEachTable) {

		m_elasticListsStackLayout->removeWidget(widget.second);
		delete widget.second;
	}
	m_elasticListWidgetsForEachTable.clear();
}

void MultiTableElasticListsWidget::OnElasticListsSelectionChanged(const QString& table, const SourceDataCache::ColumnValueData& selection) {

	if (selection.empty()) {

		m_selectionModel->ClearSourceDataSelectionForTable(table);
	}
	else {

		const SourceDataSelectionModel::IndexSetMap& indexSets = m_selectionModel->GetSourceDataSelection();
		SourceDataSelectionModel::IndexSetMap::const_iterator previousSelection = indexSets.find(table);
		SynGlyphX::IndexSet indexSet;
		
		if (previousSelection != indexSets.end()) {

			indexSet = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, selection, previousSelection->second);
		}
		else {

			indexSet = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, selection);
		}
		
		m_selectionModel->SetSourceDataSelectionForTable(table, indexSet);
	}
}