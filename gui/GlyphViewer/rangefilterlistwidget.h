///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#ifndef RANGEFILTERLISTWIDGET_H
#define RANGEFILTERLISTWIDGET_H

#include "FilteringTable.h"
#include "singlenumericrangefilterwidget.h"
#include "interval.h"
#include "filteringparameters.h"

class RangeFilterListWidget : public FilteringTable
{
	Q_OBJECT

public:
	RangeFilterListWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent);
	~RangeFilterListWidget();

protected:
	void ResetFiltersAfterAddOrRemove() override;
	void ClearData() override;
	void ResetForNewTable() override;
	void SaveFiltersInTableWidget() override;
	bool DoAnyTablesHaveFilters() const override;
	void GetFilteringParametersForTable(const QString& table, FilteringParameters& filteringParameters) override;
	QWidget* AddFilter(const QString& field, unsigned int span) override;
	void MoveRowData(unsigned int sourceSpan, unsigned int destinationSpan) override;

private slots:
	void OnRangesChanged();

private:
	typedef std::pair<SynGlyphX::DegenerateInterval, SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues> RangeAndDistinctValues;
	typedef std::pair<QString, std::vector<RangeAndDistinctValues>> Field2RangeAndDistinctValues;
	typedef std::vector<Field2RangeAndDistinctValues> Field2RangeAndDistinctValuesVector;
	typedef QMap<QString, Field2RangeAndDistinctValuesVector> Table2RangesAndDistinctValuesMap;

	FilteringParameters::ColumnRangeFilterMap GatherRangesBeforeSpan(int span);
	void ResetMinMaxExtentsForFilters(unsigned int startingSpan);
	SynGlyphX::SingleNumericRangeFilterWidget* GetRangeFilterWidgetFromCell(int row, int column = 1) const;
	SynGlyphX::SingleNumericRangeFilterWidget* GetRangeFilterWidgetFromGroup(const FilterWidgetGroupsManager::GroupedIndex& index) const;
	SynGlyphX::SingleNumericRangeFilterWidget* CreateRangeFilterWidget();

	//FilteringTable* m_rangeFiltersTableWidget;
	Table2RangesAndDistinctValuesMap m_table2RangesAndDistinctValuesMap;
};

#endif // RANGEFILTERLISTWIDGET_H
