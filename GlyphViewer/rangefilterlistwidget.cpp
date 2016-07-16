#include "rangefilterlistwidget.h"
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include "filteringparameters.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "inputfield.h"
#include "filteringmanager.h"

RangeFilterListWidget::RangeFilterListWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
	: FilteringTable(columnsModel, filteringManager, tr("Range"), true, parent)
{

}

RangeFilterListWidget::~RangeFilterListWidget()
{

}

void RangeFilterListWidget::ResetForNewTable() {

	//Insert ranges and extents for new table
	const Field2RangeAndDistinctValuesVector& newField2RangeAndExtentList = m_table2RangesAndDistinctValuesMap[m_currentTable];
	unsigned int newSize = newField2RangeAndExtentList.size();
	m_removeAllButton->setEnabled(newSize > 0);

	unsigned int oldRowCount = m_filterTableWidget->rowCount();
	m_filterTableWidget->setRowCount(newSize);
	for (unsigned int i = oldRowCount; i < newSize; ++i) {

		SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
		m_filterTableWidget->setCellWidget(i, 1, filter);
	}
	for (unsigned int j = 0; j < newSize; ++j) {

		m_filterTableWidget->setItem(j, 0, CreateItem(newField2RangeAndExtentList[j].first));
		SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = GetRangeFilterWidgetFromCell(j);
		filterWidget->SetSliderPositionValuesAndMaxExtents(newField2RangeAndExtentList[j].second.second);
		filterWidget->SetRange(newField2RangeAndExtentList[j].second.first);
	}
}

void RangeFilterListWidget::AddFilters(const QSet<QString>& fields) {

	try {

		QStringList datasourceTable = Separate(m_currentTable);

		unsigned int nextRow = m_filterTableWidget->rowCount();
		m_filterTableWidget->setRowCount(nextRow + fields.count());
		boost::uuids::string_generator gen;
		FilteringParameters::ColumnRangeFilterMap rangeFilterMap;
		for (unsigned int row = 0; row < nextRow; ++row) {

			rangeFilterMap.push_back(FilteringParameters::ColumnRangeFilter(GetTextFromCell(row), GetRangeFilterWidgetFromCell(row)->GetRange()));
		}

		for (const auto& field : fields) {

			SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
			SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);

			SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues sliderPositionValues = m_filteringManager->GetSourceDataCache()->GetSortedNumericDistictValues(inputField, rangeFilterMap);
			filter->SetSliderPositionValuesAndMaxExtents(sliderPositionValues);
			filter->SetRange(SynGlyphX::DegenerateInterval(*sliderPositionValues.begin(), *sliderPositionValues.rbegin()));

			m_filterTableWidget->setItem(nextRow, 0, CreateItem(field));
			m_filterTableWidget->setCellWidget(nextRow++, 1, filter);

			QObject::connect(filter, &SynGlyphX::SingleNumericRangeFilterWidget::RangeUpdated, this, &RangeFilterListWidget::OnRangesChanged);
		}
	}
	catch (const std::exception& e) {

		throw;
	}
}

void RangeFilterListWidget::UpdateFromSelectedRowsRemoved(unsigned int rowToStartUpdates) {

	ResetMinMaxExtentsForFilters(rowToStartUpdates);
}

void RangeFilterListWidget::GetFilteringParametersForTable(const QString& table, FilteringParameters& filteringParameters) {
	
	try {
			
		FilteringParameters::ColumnRangeFilterMap rangeFilterMap;
		for (const auto& field2RangeAndExtent : m_table2RangesAndDistinctValuesMap[table]) {

			rangeFilterMap.push_back(FilteringParameters::ColumnRangeFilter(field2RangeAndExtent.first, field2RangeAndExtent.second.first));
		}
		filteringParameters.SetRangeFilters(rangeFilterMap);
	}
	catch (const std::exception& e) {

		throw;
	}
}

void RangeFilterListWidget::OnRangesChanged() {

	SynGlyphX::SingleNumericRangeFilterWidget* updatedFilter = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(sender());
	
	unsigned int row = 0;
	for (; row < m_filterTableWidget->rowCount(); ++row) {

		if (m_filterTableWidget->cellWidget(row, 1) == updatedFilter) {

			break;
		}
	}
	ResetMinMaxExtentsForFilters(row + 1);

	m_updateButton->setEnabled(true);
}

SynGlyphX::SingleNumericRangeFilterWidget* RangeFilterListWidget::GetRangeFilterWidgetFromCell(int row, int column) const {

	return dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(m_filterTableWidget->cellWidget(row, column));
}

void RangeFilterListWidget::SaveFiltersInTableWidget() {

	Field2RangeAndDistinctValuesVector field2RangeAndExtentList;
	for (unsigned int k = 0; k < m_filterTableWidget->rowCount(); ++k) {

		Field2RangeAndDistinctValues field2RangeAndExtent;
		field2RangeAndExtent.first = GetTextFromCell(k);
		SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = GetRangeFilterWidgetFromCell(k);
		field2RangeAndExtent.second.first = filterWidget->GetRange();
		field2RangeAndExtent.second.second = filterWidget->GetSliderPositionValues();

		field2RangeAndExtentList.push_back(field2RangeAndExtent);
	}
	m_table2RangesAndDistinctValuesMap[m_currentTable] = field2RangeAndExtentList;
}

bool RangeFilterListWidget::DoAnyTablesHaveFilters() const {

	for (const auto& rangesInTable : m_table2RangesAndDistinctValuesMap) {

		if (!rangesInTable.empty()) {

			return true;
		}
	}

	return false;
}

void RangeFilterListWidget::ResetMinMaxExtentsForFilters(unsigned int startingRow) {

	FilteringParameters::ColumnRangeFilterMap columnIntervalMap;
	QStringList datasourceTable = Separate(m_currentTable);
	boost::uuids::string_generator gen;

	for (unsigned int i = 0; i < startingRow; ++i) {

		columnIntervalMap.push_back(FilteringParameters::ColumnRangeFilter(GetTextFromCell(i), GetRangeFilterWidgetFromCell(i)->GetRange()));
	}

	for (unsigned int j = startingRow; j < m_filterTableWidget->rowCount(); ++j) {

		QString field = GetTextFromCell(j);
		SynGlyphX::SingleNumericRangeFilterWidget* filter = GetRangeFilterWidgetFromCell(j);
		SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
		SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues sliderPositionValues = m_filteringManager->GetSourceDataCache()->GetSortedNumericDistictValues(inputField, columnIntervalMap);
		filter->blockSignals(true);
		filter->SetSliderPositionValuesAndMaxExtents(sliderPositionValues);
		filter->blockSignals(false);
		columnIntervalMap.push_back(std::pair<QString, SynGlyphX::DegenerateInterval>(field, SynGlyphX::DegenerateInterval(*sliderPositionValues.begin(), *sliderPositionValues.rbegin())));
	}
}

void RangeFilterListWidget::MoveRow(unsigned int sourceRow, unsigned int destinationRow) {

	SaveFiltersInTableWidget();

	Field2RangeAndDistinctValuesVector& currentTableRanges = m_table2RangesAndDistinctValuesMap[m_currentTable];
	Field2RangeAndDistinctValuesVector::iterator field2RangeMapIterator = currentTableRanges.begin();
	std::advance(field2RangeMapIterator, sourceRow);

	Field2RangeAndDistinctValues dataToMove = *field2RangeMapIterator;
	currentTableRanges.erase(field2RangeMapIterator);

	field2RangeMapIterator = currentTableRanges.begin();
	std::advance(field2RangeMapIterator, destinationRow);
	currentTableRanges.insert(field2RangeMapIterator, dataToMove);

	m_filterTableWidget->removeRow(sourceRow);
	m_filterTableWidget->insertRow(destinationRow);

	SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
	filter->SetSliderPositionValuesAndMaxExtents(dataToMove.second.second);
	filter->SetRange(dataToMove.second.first);

	m_filterTableWidget->setItem(destinationRow, 0, CreateItem(dataToMove.first));
	m_filterTableWidget->setCellWidget(destinationRow, 1, filter);

	ResetMinMaxExtentsForFilters(std::min(sourceRow, destinationRow));
}

void RangeFilterListWidget::ClearData() {

	m_table2RangesAndDistinctValuesMap.clear();
}