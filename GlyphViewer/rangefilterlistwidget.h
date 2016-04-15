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

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedLayout>
#include "singlenumericrangefilterwidget.h"
#include <QtWidgets/QTableWidget>
#include "filteringmanager.h"
#include "sourcedatainfomodel.h"
#include "interval.h"

class RangeFilterListWidget : public QWidget
{
	Q_OBJECT

public:
	RangeFilterListWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent);
	~RangeFilterListWidget();

public slots:
	void OnNewVisualization();
	void SwitchTable(const QString& table);

private slots:
	void OnAddFilter();
	void OnRemoveSelectedFilters();
	void OnRemoveAllFilters();
	void OnUpdateFilters();
	void OnRangesChanged();
	void OnFilterResultsChanged();
	void OnFilterSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void OnMoveUpRow();
	void OnMoveDownRow();

private:
	typedef std::pair<SynGlyphX::DegenerateInterval, SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues> RangeAndDistinctValues;
	typedef std::pair<QString, RangeAndDistinctValues> Field2RangeAndDistinctValues;
	typedef std::vector<Field2RangeAndDistinctValues> Field2RangeAndDistinctValuesVector;
	typedef QMap<QString, Field2RangeAndDistinctValuesVector> Table2RangesAndDistinctValuesMap;

	void UpdatedEnableStateForButton(QAction* action, QPushButton* button);
	void ResetMinMaxExtentsForFilters(unsigned int startingRow);
	void MoveRow(unsigned int sourceRow, unsigned int destinationRow);

	SynGlyphX::SingleNumericRangeFilterWidget* GetRangeFilterWidgetFromCell(int row, int column = 1) const;
	QString GetTextFromCell(int row, int column = 0) const;
	QStringList Separate(const QString& datasourceTable) const;
	void ClearFiltersFromTableWidget();
	QTableWidgetItem* CreateItem(const QString& text);
	void SaveRangesFromFiltersInTableWidget();
	bool DoAnyTablesHaveFilters() const;

	FilteringManager* m_filteringManager;

	QPushButton* m_addButton;
	QPushButton* m_removeAllButton;
	QPushButton* m_updateButton;
	QPushButton* m_moveUpButton;
	QPushButton* m_moveDownButton;
	QTableWidget* m_rangeFiltersTableWidget;

	QAction* m_removeSelectedContextMenuAction;
	QAction* m_moveRowUpContextMenuAction;
	QAction* m_moveRowDownContextMenuAction;

	Table2RangesAndDistinctValuesMap m_table2RangesAndDistinctValuesMap;

	SourceDataInfoModel* m_columnsModel;

	QString m_currentTable;
};

#endif // RANGEFILTERLISTWIDGET_H
