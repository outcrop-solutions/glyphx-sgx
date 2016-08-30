#include "keywordfilterlistwidget.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include "keywordfilterwidget.h"

KeywordFilterListWidget::KeywordFilterListWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
	: FilteringTable(columnsModel, filteringManager, SynGlyphX::InputField::Type::Text, tr("Keyword & Options"), false, parent)
{
	
}

KeywordFilterListWidget::~KeywordFilterListWidget()
{

}

void KeywordFilterListWidget::ResetForNewTable() {

	//Insert ranges and extents for new table
	const FilteringParameters::ColumnKeywordFilterMap& keywordFilterMap = m_table2FiltersMap[m_currentTable];
	unsigned int numberOfFields = keywordFilterMap.size();

	unsigned int numberOfRows = 0;
	for (unsigned int j = 0; j < numberOfFields; ++j) {

		FilterWidgetGroupsManager::FilterWidgetGroup filterWidgetGroup;
		for (unsigned int k = 0; k < keywordFilterMap[j].second.size(); ++k) {

			KeywordFilterWidget* filterWidget = CreateFilter();
			filterWidget->Set(keywordFilterMap[j].second[k]);
			m_filterListTableWidget->setCellWidget(numberOfRows + k, 1, filterWidget);
			filterWidgetGroup.push_back(filterWidget);
		}

		//m_filterListTableWidget->setSpan(numberOfRows, 0, newField2RangeAndExtentList[j].second.size(), 1);
		numberOfRows += keywordFilterMap[j].second.size();

		m_filterGroups[m_currentTable].AddGroup(filterWidgetGroup, keywordFilterMap[j].first);
	}
}

void KeywordFilterListWidget::ClearData() {

	m_table2FiltersMap.clear();
}

QWidget* KeywordFilterListWidget::AddFilter(const QString& field, unsigned int span) {

	return CreateFilter();
}

KeywordFilterWidget* KeywordFilterListWidget::CreateFilter() {

	KeywordFilterWidget* filterWidget = new KeywordFilterWidget(m_filterListTableWidget);
	QObject::connect(filterWidget, &KeywordFilterWidget::FilterUpdated, this, &KeywordFilterListWidget::OnFilterChanged);
	return filterWidget;
}

void KeywordFilterListWidget::ResetFiltersAfterAddOrRemove() {

	
}

void KeywordFilterListWidget::GetFilteringParametersForTable(const QString& table, FilteringParameters& filteringParameters) {

	FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
	for (const auto& columnAndKeywordFilter : m_table2FiltersMap[table]) {

		keywordFilterMap.push_back(FilteringParameters::ColumnKeywordFilter(columnAndKeywordFilter.first, columnAndKeywordFilter.second));
	}
	filteringParameters.SetKeywordFilters(keywordFilterMap);
}

KeywordFilterWidget* KeywordFilterListWidget::GetKeywordFilterWidgetFromCell(int row, int column) const {

	return dynamic_cast<KeywordFilterWidget*>(m_filterListTableWidget->cellWidget(row, column));
}

void KeywordFilterListWidget::SaveFiltersInTableWidget() {

	FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
	for (unsigned int k = 0; k < m_filterGroups[m_currentTable].GetNumberOfGroups(); ++k) {

		FilteringParameters::ColumnKeywordFilter columnKeywordFilter;
		columnKeywordFilter.first = m_filterGroups[m_currentTable].GetFields().at(k);

		unsigned int groupSize = m_filterGroups[m_currentTable].GetCountForGroup(k);
		for (unsigned int i = 0; i < groupSize; ++i) {

			KeywordFilterWidget* filterWidget = GetKeywordFilterWidgetFromGroup(FilterWidgetGroupsManager::GroupedIndex(k, i));
			columnKeywordFilter.second.push_back(filterWidget->Get());
		}

		keywordFilterMap.push_back(columnKeywordFilter);
	}

	m_table2FiltersMap[m_currentTable] = keywordFilterMap;
}

KeywordFilterWidget* KeywordFilterListWidget::GetKeywordFilterWidgetFromGroup(const FilterWidgetGroupsManager::GroupedIndex& index) const {

	return dynamic_cast<KeywordFilterWidget*>(m_filterGroups[m_currentTable].GetWidget(index));
}

bool KeywordFilterListWidget::DoAnyTablesHaveFilters() const {

	for (const auto& filtersInTable : m_table2FiltersMap) {

		if (!filtersInTable.empty()) {

			return true;
		}
	}

	return false;
}