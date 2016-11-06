#include "rangefilterlistwidget.h"
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include "filteringparameters.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "inputfield.h"
#include "filteringmanager.h"

RangeFilterListWidget::RangeFilterListWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
	: FilteringTable(columnsModel, filteringManager, SynGlyphX::InputField::Type::Real, tr("Range"), true, parent)
{

}

RangeFilterListWidget::~RangeFilterListWidget()
{

}

void RangeFilterListWidget::ResetForNewTable() {

	//Insert ranges and extents for new table
	const Field2RangeAndDistinctValuesVector& newField2RangeAndExtentList = m_table2RangesAndDistinctValuesMap[m_currentTable];
	unsigned int numberOfFields = newField2RangeAndExtentList.size();

	unsigned int numberOfRows = 0;
	for (unsigned int j = 0; j < numberOfFields; ++j) {

		FilterWidgetGroupsManager::FilterWidgetGroup filterWidgetGroup;
		for (unsigned int k = 0; k < newField2RangeAndExtentList[j].second.size(); ++k) {

			SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = CreateRangeFilterWidget();
			filterWidget->blockSignals(true);
			filterWidget->SetSliderPositionValuesAndMaxExtents(newField2RangeAndExtentList[j].second[k].second);
			filterWidget->SetRange(newField2RangeAndExtentList[j].second[k].first);
			m_filterListTableWidget->setCellWidget(numberOfRows + k, 1, filterWidget);
			filterWidget->blockSignals(false);

			filterWidgetGroup.push_back(filterWidget);
		}

		//m_filterListTableWidget->setSpan(numberOfRows, 0, newField2RangeAndExtentList[j].second.size(), 1);
		numberOfRows += newField2RangeAndExtentList[j].second.size();

		m_filterGroups[m_currentTable].AddGroup(filterWidgetGroup, newField2RangeAndExtentList[j].first);
	}
}

SynGlyphX::SingleNumericRangeFilterWidget* RangeFilterListWidget::CreateRangeFilterWidget() {

	SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, m_filterListTableWidget);
	QObject::connect(filter, &SynGlyphX::SingleNumericRangeFilterWidget::RangeUpdated, this, &RangeFilterListWidget::OnRangesChanged);
	return filter;
}

QWidget* RangeFilterListWidget::AddFilter(const QString& field, unsigned int span) {

	QStringList datasourceTable = Separate(m_currentTable);

	FilteringParameters::ColumnRangeFilterMap rangeFilterMap = GatherRangesBeforeSpan(span);

	boost::uuids::string_generator gen;
	SynGlyphX::SingleNumericRangeFilterWidget* filter = CreateRangeFilterWidget();
	filter->blockSignals(true);
	SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);

	SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues sliderPositionValues = m_filteringManager->GetSourceDataCache()->GetSortedNumericDistictValues(inputField, rangeFilterMap);
	filter->SetSliderPositionValuesAndMaxExtents(sliderPositionValues);
	filter->SetRange(SynGlyphX::DegenerateInterval(*sliderPositionValues.begin(), *sliderPositionValues.rbegin()));
	
	filter->blockSignals(false);
	return filter;
}

FilteringParameters::ColumnRangeFilterMap RangeFilterListWidget::GatherRangesBeforeSpan(int span) {

	FilteringParameters::ColumnRangeFilterMap rangeFilterMap;
	for (auto i = 0; i < span; ++i) {

		unsigned int rowSpan = m_filterGroups[m_currentTable].GetCountForGroup(i);
		SynGlyphX::DegenerateIntervalUnion ranges;
		for (unsigned int j = 0; j < rowSpan; ++j) {

			ranges.push_back(GetRangeFilterWidgetFromGroup(FilterWidgetGroupsManager::GroupedIndex(i, j))->GetRange());
		}
		rangeFilterMap.push_back(FilteringParameters::ColumnRangeFilter(m_filterGroups[m_currentTable].GetFields().at(i), ranges));
	}

	return rangeFilterMap;
}

void RangeFilterListWidget::ResetFiltersAfterAddOrRemove() {

	ResetMinMaxExtentsForFilters(0);
}

void RangeFilterListWidget::GetFilteringParametersForTable(const QString& table, FilteringParameters& filteringParameters) {
	
	try {
			
		FilteringParameters::ColumnRangeFilterMap rangeFilterMap;
		for (const auto& field2RangeAndExtent : m_table2RangesAndDistinctValuesMap[table]) {

			SynGlyphX::DegenerateIntervalUnion ranges;
			for (const auto& rangeAndDistinct : field2RangeAndExtent.second) {

				ranges.push_back(rangeAndDistinct.first);
			}
			rangeFilterMap.push_back(FilteringParameters::ColumnRangeFilter(field2RangeAndExtent.first, ranges));
		}
		filteringParameters.SetRangeFilters(rangeFilterMap);
	}
	catch (const std::exception&) {

		throw;
	}
}

void RangeFilterListWidget::OnRangesChanged() {

	SynGlyphX::SingleNumericRangeFilterWidget* updatedFilter = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(sender());
	
	int row = 0;
	for (; row < m_filterListTableWidget->rowCount(); ++row) {

		if (m_filterListTableWidget->cellWidget(row, 1) == updatedFilter) {

			break;
		}
	}
	ResetMinMaxExtentsForFilters(m_filterGroups[m_currentTable].GetGroupedIndex(row).first + 1);

	OnFilterChanged();
}

SynGlyphX::SingleNumericRangeFilterWidget* RangeFilterListWidget::GetRangeFilterWidgetFromCell(int row, int column) const {

	return dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(m_filterListTableWidget->cellWidget(row, column));
}

SynGlyphX::SingleNumericRangeFilterWidget* RangeFilterListWidget::GetRangeFilterWidgetFromGroup(const FilterWidgetGroupsManager::GroupedIndex& index) const {

	return dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(m_filterGroups[m_currentTable].GetWidget(index));
}

void RangeFilterListWidget::SaveFiltersInTableWidget() {

	Field2RangeAndDistinctValuesVector field2RangeAndExtentList;
	for (unsigned int k = 0; k < m_filterGroups[m_currentTable].GetNumberOfGroups(); ++k) {

		Field2RangeAndDistinctValues field2RangeAndExtent;
		field2RangeAndExtent.first = m_filterGroups[m_currentTable].GetFields().at(k);

		unsigned int groupSize = m_filterGroups[m_currentTable].GetCountForGroup(k);
		for (unsigned int i = 0; i < groupSize; ++i) {

			SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = GetRangeFilterWidgetFromGroup(FilterWidgetGroupsManager::GroupedIndex(k, i));
			field2RangeAndExtent.second.push_back(RangeAndDistinctValues(filterWidget->GetRange(), filterWidget->GetSliderPositionValues()));
		}

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

void RangeFilterListWidget::ResetMinMaxExtentsForFilters(unsigned int startingSpan) {

	FilteringParameters::ColumnRangeFilterMap columnIntervalMap = GatherRangesBeforeSpan(startingSpan);
	QStringList datasourceTable = Separate(m_currentTable);
	boost::uuids::string_generator gen;

	for (unsigned int j = startingSpan; j < m_filterGroups[m_currentTable].GetNumberOfGroups(); ++j) {

		QString field = m_filterGroups[m_currentTable].GetFields().at(j);
		SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
		SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues sliderPositionValues = m_filteringManager->GetSourceDataCache()->GetSortedNumericDistictValues(inputField, columnIntervalMap);
		
		SynGlyphX::DegenerateIntervalUnion intervalUnion;
		for (unsigned int k = 0; k < m_filterGroups[m_currentTable].GetCountForGroup(j); ++k) {

			SynGlyphX::SingleNumericRangeFilterWidget* filter = GetRangeFilterWidgetFromGroup(FilterWidgetGroupsManager::GroupedIndex(j, k));
			filter->blockSignals(true);
			filter->SetSliderPositionValuesAndMaxExtents(sliderPositionValues);
			filter->blockSignals(false);
			intervalUnion.push_back(filter->GetRange());
		}
		
		columnIntervalMap.push_back(std::pair<QString, SynGlyphX::DegenerateIntervalUnion>(field, intervalUnion));
	}
}

void RangeFilterListWidget::ClearData() {

	m_table2RangesAndDistinctValuesMap.clear();
}

void RangeFilterListWidget::MoveRowData(unsigned int sourceSpan, unsigned int destinationSpan) {

	Field2RangeAndDistinctValuesVector& newField2RangeAndExtentList = m_table2RangesAndDistinctValuesMap[m_currentTable];

	Field2RangeAndDistinctValues spanToSwap = newField2RangeAndExtentList.at(sourceSpan);
	Field2RangeAndDistinctValuesVector::iterator iT = newField2RangeAndExtentList.begin();
	std::advance(iT, sourceSpan);
	newField2RangeAndExtentList.erase(iT);

	iT = newField2RangeAndExtentList.begin();
	std::advance(iT, destinationSpan);
	newField2RangeAndExtentList.insert(iT, spanToSwap);
}