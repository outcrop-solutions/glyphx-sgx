#include "LoadingFilterWidget.h"
#include "TitleListWidget.h"
#include <QtWidgets/QLayout>
#include "glyphengine.h"
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include "datasource.h"
#include "sourcedatacache.h"
#include "stringconvert.h"
#include <boost/uuid/uuid_io.hpp>
#include "datatransformmapping.h"
#include "groupboxsinglewidget.h"
#include <QtCore/QCollator>

LoadingFilterMissingChoice::LoadingFilterMissingChoice(const std::wstring& field) :
	std::runtime_error(""),
	m_field(field) {


}

SingleLoadingFilterWidget::SingleLoadingFilterWidget(bool isRequired, QWidget* parent) :
	SynGlyphX::TitleListWidget(parent),
	m_isRequired(isRequired) {


}


LoadingFilterWidget::LoadingFilterWidget(QWidget *parent)
	: QWidget(parent)
{
	
}

LoadingFilterWidget::~LoadingFilterWidget()
{

}

void LoadingFilterWidget::SetFilters(DataEngine::GlyphEngine& glyphEngine, const SynGlyphX::DataTransformMapping& mapping) {

	//QWidget* innerWidget = new QWidget(this);
	//setWidget(innerWidget);

	//QVBoxLayout* innerWidgetLayout = new QVBoxLayout(innerWidget);
	QVBoxLayout* innerWidgetLayout = new QVBoxLayout(this);

	const SynGlyphX::MultiTableFrontEndFilters& filters = mapping.GetFrontEndFilters();
	//for (const auto& filtersForTable : filters) {

		QSplitter* splitter = AddFiltersForTable(glyphEngine, mapping);
		//QString title = QString::fromStdWString(mapping.GetFormattedName(filtersForTable.first.GetDatasourceID(), filtersForTable.first.GetTable()));
		//std::string cleanedTitle = QString::fromStdWString(mapping.GetFormattedName(filtersForTable.first.GetDatasourceID(), filtersForTable.first.GetTable())).toStdString();
		//std::replace(cleanedTitle.begin(), cleanedTitle.end(), '_', ' ');
		//QString title(cleanedTitle.c_str());
		QString title = "Filters:";
		if (filters.size() == 1){
			innerWidgetLayout->addWidget(splitter);
		} 
		else{
			SynGlyphX::GroupBoxSingleWidget* groupBox = new SynGlyphX::GroupBoxSingleWidget(title, splitter, this);
			groupBox->setContentsMargins(0, 0, 0, 0);
			groupBox->setStyleSheet("QGroupBox{font-family:'Calibri', Helvetica, Arial, Sans; font-weight: bold; text-transform: uppercase; font-size: 16px; line-height: 24px;}");
			innerWidgetLayout->addWidget(groupBox);
		}
	//}

	//int stretchSize = 3 - filters.size();
	//if (stretchSize > 0) {

	//	innerWidgetLayout->addStretch(stretchSize);
	//}

	//innerWidget->setLayout(innerWidgetLayout);
	setLayout(innerWidgetLayout);
}

QSplitter* LoadingFilterWidget::AddFiltersForTable(DataEngine::GlyphEngine& glyphEngine, const SynGlyphX::DataTransformMapping& mapping) {

	QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

	splitter->setChildrenCollapsible(false);
	splitter->setHandleWidth(1);
	splitter->setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
		"border: 1px solid #777; width: 0px; margin-top: 0px; margin-bottom: 0px; border-radius: 2px; }");

	FieldToWidgetMap fieldToWidgetMap;
	std::vector<SynGlyphX::InputTable> tables;

	const SynGlyphX::MultiTableFrontEndFilters& filters = mapping.GetFrontEndFilters();
	for (const auto& filter : filters) {

		SingleLoadingFilterWidget* filterWidget = new SingleLoadingFilterWidget(filter.isRequired, this);
		filterWidget->layout()->setContentsMargins(0, 0, 0, 0);
		filterWidget->SetAllowMultiselect(filter.isMultiselectAllowed);
		filterWidget->ShowSelectAllButton(true);
		filterWidget->ShowSearchFilter(true);
		QStringList distinctValues;

		for (const auto& inputField : filter.fields){
			SynGlyphX::InputTable table(inputField);
			tables.push_back(table);	

			QString id = QString::fromStdWString(boost::uuids::to_wstring(table.GetDatasourceID()));
			QString tableName = QString::fromStdWString(table.GetTable());

			QString qField = QString::fromStdWString(inputField.GetField());
			auto fieldToAliasMap = mapping.GetFieldToAliasMapForTable(table);
			if (fieldToAliasMap.count(inputField.GetField()) == 0) {

				std::string cleanedField = qField.toStdString();
				std::replace( cleanedField.begin(), cleanedField.end(), '_', ' ');
				filterWidget->SetTitle(QString(cleanedField.c_str()));
			}
			else {

				filterWidget->SetTitle(QString::fromStdWString(fieldToAliasMap.at(inputField.GetField())));
			}
			auto map = m_filterListWidgets.find(table);
			if (map != m_filterListWidgets.end()) { //fieldToWidgetMap arleady exists for the table, add widget
				m_filterListWidgets[table][inputField.GetField()] = filterWidget;
			}
			else {// create fieldToWidgetMap for the table
				FieldToWidgetMap fieldToWidgetMap;
				fieldToWidgetMap[inputField.GetField()] = filterWidget;
				m_filterListWidgets[table] = fieldToWidgetMap;
			}

			QStringList distinctValuesTable = glyphEngine.DistinctValuesForField(id, tableName, qField);
			distinctValues.append(distinctValuesTable);
		}
		//m_filterListWidgets[table] = fieldToWidgetMap;
		distinctValues.removeDuplicates();
		QCollator collator;
		collator.setNumericMode(true);
		collator.setCaseSensitivity(Qt::CaseInsensitive);
		std::sort(
			distinctValues.begin(),
			distinctValues.end(),
			[&collator](const QString &value1, const QString &value2)
		{
			return collator.compare(value1, value2) < 0;
		});
		filterWidget->SetItems(distinctValues);

		splitter->addWidget(filterWidget);		
	}	

	return splitter;
}

bool LoadingFilterWidget::AreSelectionsValid() const {

	for (const auto& tableFilterWidgets : m_filterListWidgets) {

		for (const auto& filterWidget : tableFilterWidgets.second) {

			if (filterWidget.second->IsRequired() && !filterWidget.second->AreAnyItemsSelected()) {

				return false;
			}
		}
	}

	return true;
}

bool LoadingFilterWidget::IsQueryNeeded(const SynGlyphX::InputTable& table) const {

	for (const auto& filterWidget : m_filterListWidgets.find(table)->second) {

		if (filterWidget.second->AreAnyItemsSelected()) {

			return true;
		}
	}

	return false;
}

MultiTableDistinctValueFilteringParameters LoadingFilterWidget::GetFilterValues() const {

	MultiTableDistinctValueFilteringParameters filteringParameters;

	for (const auto& tableFilterWidgets : m_filterListWidgets) {

		DistinctValueFilteringParameters filteringParametersForTable;
		for (const auto& filterWidget : tableFilterWidgets.second) {

			QSet<QString> filterData = filterWidget.second->GetSelectedLabels().toSet();
			if ((!filterData.isEmpty()) && (!filterWidget.second->AreAllItemsSelected())) {

				filteringParametersForTable.SetDistinctValueFilter(QString::fromStdWString(filterWidget.first), filterData);
			}
		}

		if (filteringParametersForTable.HasFilters()) {

			filteringParameters[tableFilterWidgets.first] = filteringParametersForTable;
		}
	}

	return filteringParameters;
}