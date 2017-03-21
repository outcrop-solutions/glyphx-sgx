#include "singletableelasticlistswidget.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

const unsigned int SingleTableElasticListsWidget::Spacing = 16;

SingleTableElasticListsWidget::SingleTableElasticListsWidget(AliasAndFieldList aliasAndFieldList, SourceDataCache::ConstSharedPtr sourceDataCache, const QString& table, FilteringManager* filteringManager, QWidget *parent)
	: SynGlyphX::VerticalScrollArea(parent),
	m_sourceDataCache(sourceDataCache),
	m_table(table)
{
	setFrameShape(QFrame::Shape::NoFrame);
	m_innerWidget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(m_innerWidget);
	layout->setSpacing(Spacing);

	if (filteringManager->GetElasticListFields(table).size() > 0){

		for (const auto& field : filteringManager->GetElasticListFields(table)){

			for (const auto& aliasAndField : aliasAndFieldList) {

				if (aliasAndField.second == QString::fromStdWString(field)){
					AddFieldToElasticList(layout, aliasAndField);
					break;
				}
			}
		}
	}
	else{

		for (const auto& aliasAndField : aliasAndFieldList) {

			AddFieldToElasticList(layout, aliasAndField);
		}
	}

	std::vector<std::string> splt;
	boost::split(splt, table.toStdString(), boost::is_any_of(":"));
	m_fieldPropertiesMap = filteringManager->GetFieldPropertiesForTable(boost::lexical_cast<boost::uuids::uuid>(splt.at(0)), splt.size() == 2 ? QString(splt.at(1).c_str()).toStdWString() : L"OnlyTable");

	layout->addStretch(1);

	m_innerWidget->setLayout(layout);

	setWidget(m_innerWidget);
}

SingleTableElasticListsWidget::~SingleTableElasticListsWidget()
{

}

void SingleTableElasticListsWidget::AddFieldToElasticList(QVBoxLayout* layout, AliasAndField field){

	SynGlyphX::ElasticListWidget* elasticListWidget = new SynGlyphX::ElasticListWidget(this);

	std::string cleanedField = field.first.toStdString();
	std::replace(cleanedField.begin(), cleanedField.end(), '_', ' ');
	//elasticListWidget->SetTitle(aliasAndField.first);
	elasticListWidget->SetTitle(QString(cleanedField.c_str()));

	layout->addWidget(elasticListWidget);
	m_elasticListMap[field.second.toStdString()] = elasticListWidget;

	QObject::connect(elasticListWidget, &SynGlyphX::ElasticListWidget::SelectionChanged, this, &SingleTableElasticListsWidget::OnElasticWidgetSelectionChanged);
}

void SingleTableElasticListsWidget::PopulateElasticLists(const SynGlyphX::IndexSet& indexSet) {

	for (auto column : m_elasticListMap) {

		if (indexSet.empty() || (!column.second->HasSelection())) {

			SynGlyphX::ElasticListModel::Data elasticListData;
			SourceDataCache::SharedSQLQuery distinctValuesQuery = m_sourceDataCache->CreateDistinctValueAndCountQuery(m_table, QString::fromStdString(column.first), indexSet);
			distinctValuesQuery->exec();
			while (distinctValuesQuery->next()) {

				elasticListData.push_back(SynGlyphX::ElasticListModel::DataWithCount(distinctValuesQuery->value(0), distinctValuesQuery->value(1).toUInt()));
			}

			column.second->SetData(elasticListData);

			if (m_fieldPropertiesMap.find(QString::fromStdString(column.first).toStdWString()) != m_fieldPropertiesMap.end()){

				SynGlyphX::FieldProperties fp = m_fieldPropertiesMap.at(QString::fromStdString(column.first).toStdWString());
				QStringList qsl = column.second->GetElasticListModel()->GetFormattedData();

				for (int i = 0; i < qsl.size(); i++){
					qsl.replace(i, fp.transformData(qsl.at(i)));
				}
				column.second->GetElasticListModel()->ReplaceFormattedData(qsl);
			}
		}
	}
}

void SingleTableElasticListsWidget::OnElasticWidgetSelectionChanged() {

	FilteringParameters::ColumnDistinctValuesFilterMap newSelection;
	for (auto elasticListWidget : m_elasticListMap) {

		const QSet<QString>& columnSelection = elasticListWidget.second->GetSelectedRawData();
		if (!columnSelection.empty()) {

			newSelection.push_back(std::make_pair(QString::fromStdString(elasticListWidget.first), columnSelection));
		}
	}

	emit SelectionChanged(m_table, newSelection);
}