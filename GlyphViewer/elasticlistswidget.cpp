#include "elasticlistswidget.h"

ElasticListsWidget::ElasticListsWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, const QString& table, QWidget *parent)
	: QWidget(parent),
	m_sourceDataCache(sourceDataCache),
	m_table(table)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	SynGlyphX::TableColumns columns = m_sourceDataCache->GetColumnsForTable(table);
	for (const QString& column : columns) {

		SynGlyphX::ElasticListWidget* elasticListWidget = new SynGlyphX::ElasticListWidget(this);
		elasticListWidget->SetTitle(column);
		layout->addWidget(elasticListWidget);
		m_elasticListMap[column.toStdString()] = elasticListWidget;
	}

	setLayout(layout);

	PopulateElasticLists();
}

ElasticListsWidget::~ElasticListsWidget()
{

}

void ElasticListsWidget::PopulateElasticLists(const SynGlyphX::SourceDataCache::IndexSet& indexSet) {

	for (auto column : m_elasticListMap) {

		SynGlyphX::ElasticListWidget::Data elasticListData;
		SynGlyphX::SharedSQLQuery distinctValuesQuery = m_sourceDataCache->CreateDistinctValueQuery(m_table, QString::fromStdString(column.first), indexSet);
		distinctValuesQuery->exec();
		while (distinctValuesQuery->next()) {

			QString columnValue = distinctValuesQuery->value(0).toString();
			elasticListData.push_back(std::pair<QString, QString>(columnValue, QString::number(m_sourceDataCache->GetValueCount(m_table, QString::fromStdString(column.first), columnValue, indexSet))));
		}

		column.second->SetData(elasticListData);
	}
}