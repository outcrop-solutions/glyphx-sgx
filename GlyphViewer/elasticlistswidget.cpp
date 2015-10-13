#include "elasticlistswidget.h"

const unsigned int ElasticListsWidget::Spacing = 2;

ElasticListsWidget::ElasticListsWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, const QString& table, QWidget *parent)
	: SynGlyphX::VerticalScrollArea(parent),
	m_sourceDataCache(sourceDataCache),
	m_table(table)
{
	m_innerWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(m_innerWidget);
	layout->setSpacing(Spacing);

	SynGlyphX::TableColumns columns = m_sourceDataCache->GetColumnsForTable(table);
	for (const QString& column : columns) {

		SynGlyphX::ElasticListWidget* elasticListWidget = new SynGlyphX::ElasticListWidget(this);
		elasticListWidget->SetTitle(column);
		layout->addWidget(elasticListWidget);
		m_elasticListMap[column.toStdString()] = elasticListWidget;
		QObject::connect(elasticListWidget, &SynGlyphX::ElasticListWidget::SelectionChanged, this, &ElasticListsWidget::OnElasticWidgetSelectionChanged);
	}

	layout->addStretch(1);

	m_innerWidget->setLayout(layout);

	setWidget(m_innerWidget);

	//PopulateElasticLists();
}

ElasticListsWidget::~ElasticListsWidget()
{

}

void ElasticListsWidget::PopulateElasticLists(const SynGlyphX::IndexSet& indexSet) {

	for (auto column : m_elasticListMap) {

		SynGlyphX::ElasticListModel::Data elasticListData;
		SynGlyphX::SharedSQLQuery distinctValuesQuery = m_sourceDataCache->CreateDistinctValueAndCountQuery(m_table, QString::fromStdString(column.first), indexSet);
		distinctValuesQuery->exec();
		while (distinctValuesQuery->next()) {

			elasticListData.push_back(SynGlyphX::ElasticListModel::DataWithCount(distinctValuesQuery->value(0), distinctValuesQuery->value(1).toULongLong()));
		}

		column.second->SetData(elasticListData);
	}
}

void ElasticListsWidget::OnElasticWidgetSelectionChanged() {

	SynGlyphX::SourceDataCache::ColumnValueData newSelection;
	for (auto elasticListWidget : m_elasticListMap) {

		const std::set<QString>& columnSelection = elasticListWidget.second->GetSelectedRawData();
		if (!columnSelection.empty()) {

			newSelection[QString::fromStdString(elasticListWidget.first)] = columnSelection;
		}
	}

	emit SelectionChanged(m_table, newSelection);
}