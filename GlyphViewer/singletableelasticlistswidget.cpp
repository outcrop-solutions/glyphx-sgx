#include "singletableelasticlistswidget.h"

const unsigned int SingleTableElasticListsWidget::Spacing = 2;

SingleTableElasticListsWidget::SingleTableElasticListsWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, const QString& table, QWidget *parent)
	: SynGlyphX::VerticalScrollArea(parent),
	m_sourceDataCache(sourceDataCache),
	m_table(table)
{
	setFrameShape(QFrame::Shape::NoFrame);
	m_innerWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(m_innerWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(Spacing);

	SynGlyphX::TableColumns columns = m_sourceDataCache->GetColumnsForTable(table);
	for (const auto& column : columns) {

		SynGlyphX::ElasticListWidget* elasticListWidget = new SynGlyphX::ElasticListWidget(this);
		elasticListWidget->SetTitle(column.first);
		layout->addWidget(elasticListWidget);
		m_elasticListMap[column.first.toStdString()] = elasticListWidget;
		QObject::connect(elasticListWidget, &SynGlyphX::ElasticListWidget::SelectionChanged, this, &SingleTableElasticListsWidget::OnElasticWidgetSelectionChanged);
	}

	layout->addStretch(1);

	m_innerWidget->setLayout(layout);

	setWidget(m_innerWidget);

	//PopulateElasticLists();
}

SingleTableElasticListsWidget::~SingleTableElasticListsWidget()
{

}

void SingleTableElasticListsWidget::PopulateElasticLists(const SynGlyphX::IndexSet& indexSet) {

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

void SingleTableElasticListsWidget::OnElasticWidgetSelectionChanged() {

	SynGlyphX::SourceDataCache::ColumnValueData newSelection;
	for (auto elasticListWidget : m_elasticListMap) {

		const std::set<QString>& columnSelection = elasticListWidget.second->GetSelectedRawData();
		if (!columnSelection.empty()) {

			newSelection[QString::fromStdString(elasticListWidget.first)] = columnSelection;
		}
	}

	emit SelectionChanged(m_table, newSelection);
}