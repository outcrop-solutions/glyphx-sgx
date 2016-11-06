#include "multitableelasticlistswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QMessageBox>
#include "elasticlistwidget.h"
#include "application.h"
#include "sourcedatainfomodel.h"

MultiTableElasticListsWidget::MultiTableElasticListsWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
	: QWidget(parent),
	m_sourceDataInfoModel(columnsModel),
	m_filteringManager(filteringManager)
{
	m_elasticListsStackLayout = new QStackedLayout(this);
	m_elasticListsStackLayout->setContentsMargins(0, 0, 0, 0);
	ClearElasticLists();

	setLayout(m_elasticListsStackLayout);

	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, this, &MultiTableElasticListsWidget::OnFilterResultsChanged);
}

MultiTableElasticListsWidget::~MultiTableElasticListsWidget()
{

}

void MultiTableElasticListsWidget::OnFilterResultsChanged() {

	UpdateElasticListsAndSourceDataWidget();
}

void MultiTableElasticListsWidget::OnNewVisualization() {

	ClearElasticLists();
	SourceDataCache::ConstSharedPtr sourceDataCache = m_filteringManager->GetSourceDataCache();
	if (sourceDataCache->IsValid()) {

		for (int i = 0; i < m_sourceDataInfoModel->rowCount(); ++i) {

			QModelIndex datasourceIndex = m_sourceDataInfoModel->index(i, 0);
			for (int j = 0; j < m_sourceDataInfoModel->rowCount(datasourceIndex); ++j) {

				QModelIndex tableIndex = m_sourceDataInfoModel->index(j, 0, datasourceIndex);
				QString sourceDataCacheTablename = SourceDataCache::CreateTablename(m_sourceDataInfoModel->data(datasourceIndex, SourceDataInfoModel::IDRole).toString(),
					m_sourceDataInfoModel->data(tableIndex).toString());

				SingleTableElasticListsWidget::AliasAndFieldList aliasAndFieldList;

				for (int k = 0; k < m_sourceDataInfoModel->rowCount(tableIndex); ++k) {

					QModelIndex fieldIndex = m_sourceDataInfoModel->index(k, 0, tableIndex);
					aliasAndFieldList.push_back(SingleTableElasticListsWidget::AliasAndField(m_sourceDataInfoModel->data(fieldIndex).toString(),
						m_sourceDataInfoModel->data(fieldIndex, SourceDataInfoModel::FieldRole).toString()));
				}

				SingleTableElasticListsWidget* elasticListsWidgetForTable = new SingleTableElasticListsWidget(aliasAndFieldList, sourceDataCache, sourceDataCacheTablename, this);
				m_elasticListsStackLayout->addWidget(elasticListsWidgetForTable);
				m_elasticListWidgetsForEachTable[sourceDataCacheTablename.toStdWString()] = elasticListsWidgetForTable;
				QObject::connect(elasticListsWidgetForTable, &SingleTableElasticListsWidget::SelectionChanged, this, &MultiTableElasticListsWidget::OnElasticListsSelectionChanged);
			}
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
	if (!m_filteringManager->GenerateFilterResultsForTable(table, newParameters)) {

		QMessageBox::warning(this, tr("Filtering"), tr("No glyphs match the current combination of filters."));
	}
}