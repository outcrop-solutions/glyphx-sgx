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

#pragma once

#include <QtWidgets/QWidget>

class SourceDataInfoModel;
class FilteringManager;
class FilteringParameters;
class QAction;
class QTableWidget;
class QPushButton;
class QItemSelection;
class QTableWidgetItem;

class FilteringTable : public QWidget
{
	Q_OBJECT

public:
	FilteringTable(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, const QString& label, bool includeMoveUpDown, QWidget *parent);
	~FilteringTable();

public slots:
	void OnNewVisualization();
	void SwitchTable(const QString& table);
	void OnRemoveAllFilters();

protected slots:
	void OnRemoveSelectedFilters();
	void OnFilterSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void OnFilterResultsChanged();

	void OnMoveUpRow();
	void OnMoveDownRow();

	void OnUpdateFilters();
	void OnAddFilter();

protected:
	virtual void UpdateFromSelectedRowsRemoved(unsigned int startingRowToUpdate) = 0;
	virtual void ClearData() = 0;
	virtual void ResetForNewTable() = 0;
	virtual void SaveFiltersInTableWidget() = 0;
	virtual bool DoAnyTablesHaveFilters() const = 0;
	virtual void GetFilteringParametersForTable(const QString& table, FilteringParameters& filteringParameters) = 0;
	virtual void AddFilters(const QSet<QString>& fields) = 0;
	virtual void MoveRow(unsigned int sourceRow, unsigned int destinationRow);

	void UpdatedEnableStateForButton(QAction* action, QPushButton* button);
	void ClearFiltersFromTableWidget();
	QString GetTextFromCell(int row) const;
	QStringList Separate(const QString& datasourceTable) const;
	QTableWidgetItem* CreateItem(const QString& text);

	QAction* m_removeSelectedContextMenuAction;
	QAction* m_moveRowUpContextMenuAction;
	QAction* m_moveRowDownContextMenuAction;

	QPushButton* m_addButton;
	QPushButton* m_removeAllButton;
	QPushButton* m_updateButton;
	QPushButton* m_moveUpButton;
	QPushButton* m_moveDownButton;

	QTableWidget* m_filterTableWidget;

	SourceDataInfoModel* m_columnsModel;
	FilteringManager* m_filteringManager;

	QString m_currentTable;
};

//#pragma once
