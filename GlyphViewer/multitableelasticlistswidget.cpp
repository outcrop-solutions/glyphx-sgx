#include "multitableelasticlistswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include "elasticlistwidget.h"
#include "application.h"

MultiTableElasticListsWidget::MultiTableElasticListsWidget(FilteringManager* filteringManager, QWidget *parent)
	: QWidget(parent),
	m_filteringManager(filteringManager)
	//m_sourceDataCache(sourceDataCache)
{
	m_elasticListsStackLayout = new QStackedLayout(this);
	m_elasticListsStackLayout->setContentsMargins(0, 0, 0, 0);
	ClearElasticLists();

	setLayout(m_elasticListsStackLayout);

	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, this, &MultiTableElasticListsWidget::OnFilterResultsChanged);
	QObject::connect(m_filteringManager->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &MultiTableElasticListsWidget::OnModelReset);
}

MultiTableElasticListsWidget::~MultiTableElasticListsWidget()
{

}

void MultiTableElasticListsWidget::OnFilterResultsChanged() {

	UpdateElasticListsAndSourceDataWidget();
}

void MultiTableElasticListsWidget::OnModelReset() {

	ClearElasticLists();
	SourceDataCache::ConstSharedPtr sourceDataCache = m_filteringManager->GetSourceDataCache();
	if (sourceDataCache->IsValid()) {

		const SourceDataCache::TableNameMap& tableNameMap = sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			SingleTableElasticListsWidget* elasticListsWidgetForTable = new SingleTableElasticListsWidget(sourceDataCache, formattedName.first, this);
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

	const FilteringManager::IndexSetMap& filterResultsByTable = m_filteringManager->GetFilterResultsByTable();
	//bool isSelectionNotEmpty = !sourceDataSelectionSets.empty();
	if (!filterResultsByTable.empty()) {

		//m_sourceDataWindow->UpdateTables(sourceDataSelectionSets);
		UpdateElasticLists(filterResultsByTable);
	}
	else {

		UpdateElasticLists();
	}
}

void MultiTableElasticListsWidget::UpdateElasticLists(const FilteringManager::IndexSetMap& dataIndexes) {

	for (auto table : m_filteringManager->GetSourceDataCache()->GetFormattedNames()) {

		//int c = m_sourceDataCache->GetNumberOfRowsInTable(table.first);
		SingleTableElasticListsWidget* singleTableElasticListsWidget = m_elasticListWidgetsForEachTable.at(table.first.toStdWString());
		FilteringManager::IndexSetMap::const_iterator dataIndexesForTable = dataIndexes.find(table.first);
		if (dataIndexesForTable == dataIndexes.end()) {

			singleTableElasticListsWidget->PopulateElasticLists();
		}
		else {

			singleTableElasticListsWidget->PopulateElasticLists(dataIndexesForTable->second);
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

void MultiTableElasticListsWidget::OnElasticListsSelectionChanged(const QString& table, const FilteringParameters::ColumnDistinctValuesFilterMap& selection) {

	FilteringParameters newParameters; 
	const FilteringManager::Table2FiltersMap& table2FiltersMap = m_filteringManager->GetTable2FiltersMap();
	if (table2FiltersMap.count(table) > 0) {

		newParameters = table2FiltersMap[table];
	}
	newParameters.SetDistinctValueFilters(selection);
	m_filteringManager->GenerateFilterResultsForTable(table, newParameters);
}