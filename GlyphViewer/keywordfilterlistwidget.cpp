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
	unsigned int newSize = keywordFilterMap.size();
	m_removeAllButton->setEnabled(newSize > 0);

	unsigned int oldRowCount = m_filterTableWidget->rowCount();
	m_filterTableWidget->setRowCount(newSize);
	for (unsigned int i = oldRowCount; i < newSize; ++i) {

		KeywordFilterWidget* filter = new KeywordFilterWidget(this);
		m_filterTableWidget->setCellWidget(i, 1, filter);
	}
	for (unsigned int j = 0; j < newSize; ++j) {

		m_filterTableWidget->setItem(j, 0, CreateItem(keywordFilterMap[j].first));
		KeywordFilterWidget* filterWidget = GetKeywordFilterWidgetFromCell(j);
		filterWidget->Set(keywordFilterMap[j].second);
	}
}

void KeywordFilterListWidget::ClearData() {

	m_table2FiltersMap.clear();
}

void KeywordFilterListWidget::AddFilters(const QSet<QString>& fields) {

	try {

		QStringList datasourceTable = Separate(m_currentTable);

		unsigned int nextRow = m_filterTableWidget->rowCount();
		m_filterTableWidget->setRowCount(nextRow + fields.count());
		boost::uuids::string_generator gen;
		FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
		for (unsigned int row = 0; row < nextRow; ++row) {

			keywordFilterMap.push_back(FilteringParameters::ColumnKeywordFilter(GetTextFromCell(row), GetKeywordFilterWidgetFromCell(row)->Get()));
		}

		for (const auto& field : fields) {

			KeywordFilterWidget* filter = new KeywordFilterWidget(this);
				
			m_filterTableWidget->setItem(nextRow, 0, CreateItem(field));
			m_filterTableWidget->setCellWidget(nextRow++, 1, filter);

			m_removeAllButton->setEnabled(true);
			QObject::connect(filter, &KeywordFilterWidget::FilterUpdated, this, &KeywordFilterListWidget::OnFilterChanged);
		}

		m_updateButton->setEnabled(true);
	}
	catch (const std::exception& e) {

		throw;
	}
}

void KeywordFilterListWidget::UpdateFromSelectedRowsRemoved(unsigned int rowToStartUpdates) {

	
}

void KeywordFilterListWidget::GetFilteringParametersForTable(const QString& table, FilteringParameters& filteringParameters) {

	FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
	for (const auto& columnAndKeywordFilter : m_table2FiltersMap[table]) {

		keywordFilterMap.push_back(FilteringParameters::ColumnKeywordFilter(columnAndKeywordFilter.first, columnAndKeywordFilter.second));
	}
	filteringParameters.SetKeywordFilters(keywordFilterMap);
}

KeywordFilterWidget* KeywordFilterListWidget::GetKeywordFilterWidgetFromCell(int row, int column) const {

	return dynamic_cast<KeywordFilterWidget*>(m_filterTableWidget->cellWidget(row, column));
}

void KeywordFilterListWidget::SaveFiltersInTableWidget() {

	FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
	for (unsigned int k = 0; k < m_filterTableWidget->rowCount(); ++k) {

		FilteringParameters::ColumnKeywordFilter columnKeywordFilter;
		columnKeywordFilter.first = GetTextFromCell(k);
		KeywordFilterWidget* filterWidget = GetKeywordFilterWidgetFromCell(k);
		columnKeywordFilter.second = filterWidget->Get();

		keywordFilterMap.push_back(columnKeywordFilter);
	}

	m_table2FiltersMap[m_currentTable] = keywordFilterMap;
}

bool KeywordFilterListWidget::DoAnyTablesHaveFilters() const {

	for (const auto& filtersInTable : m_table2FiltersMap) {

		if (!filtersInTable.empty()) {

			return true;
		}
	}

	return false;
}

void KeywordFilterListWidget::MoveRow(unsigned int sourceRow, unsigned int destinationRow) {


}