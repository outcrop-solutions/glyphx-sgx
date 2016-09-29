#include "singletableelasticlistswidget.h"

const unsigned int SingleTableElasticListsWidget::Spacing = 16;

SingleTableElasticListsWidget::SingleTableElasticListsWidget(AliasAndFieldList aliasAndFieldList, SourceDataCache::ConstSharedPtr sourceDataCache, const QString& table, QWidget *parent)
	: SynGlyphX::VerticalScrollArea(parent),
	m_sourceDataCache(sourceDataCache),
	m_table(table)
{
	setFrameShape(QFrame::Shape::NoFrame);
	m_innerWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(m_innerWidget);
	layout->setSpacing(Spacing);

	for (const auto& aliasAndField : aliasAndFieldList) {

		SynGlyphX::ElasticListWidget* elasticListWidget = new SynGlyphX::ElasticListWidget(this);

		std::string cleanedField = aliasAndField.first.toStdString();
		std::replace(cleanedField.begin(), cleanedField.end(), '_', ' ');
		//elasticListWidget->SetTitle(aliasAndField.first);
		elasticListWidget->SetTitle(QString(cleanedField.c_str()));

		layout->addWidget(elasticListWidget);
		m_elasticListMap[aliasAndField.second.toStdString()] = elasticListWidget;
		QObject::connect(elasticListWidget, &SynGlyphX::ElasticListWidget::SelectionChanged, this, &SingleTableElasticListsWidget::OnElasticWidgetSelectionChanged);
	}

	layout->addStretch(1);

	m_innerWidget->setLayout(layout);

	setWidget(m_innerWidget);
}

SingleTableElasticListsWidget::~SingleTableElasticListsWidget()
{

}

void SingleTableElasticListsWidget::PopulateElasticLists(const SynGlyphX::IndexSet& indexSet) {

	for (auto column : m_elasticListMap) {

		if (indexSet.empty() || (!column.second->HasSelection())) {

			SynGlyphX::ElasticListModel::Data elasticListData;
			SourceDataCache::SharedSQLQuery distinctValuesQuery = m_sourceDataCache->CreateDistinctValueAndCountQuery(m_table, QString::fromStdString(column.first), indexSet);
			distinctValuesQuery->exec();
			while (distinctValuesQuery->next()) {

				elasticListData.push_back(SynGlyphX::ElasticListModel::DataWithCount(distinctValuesQuery->value(0), distinctValuesQuery->value(1).toULongLong()));
			}

			column.second->SetData(elasticListData);
		}
	}
}

void SingleTableElasticListsWidget::OnElasticWidgetSelectionChanged() {

	FilteringParameters::ColumnDistinctValuesFilterMap newSelection;
	for (auto elasticListWidget : m_elasticListMap) {

		const QSet<QString>& columnSelection = elasticListWidget.second->GetSelectedRawData();
		if (!columnSelection.empty()) {

			newSelection[QString::fromStdString(elasticListWidget.first)] = columnSelection;
		}
	}

	emit SelectionChanged(m_table, newSelection);
}