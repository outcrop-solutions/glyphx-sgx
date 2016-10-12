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
#include <QtCore/QAbstractTableModel>
#include <set>
#include "FrontEndFilter.h"

class QComboBox;
class QTableView;
class QItemSelection;

//class FrontEndFilterModel : public QAbstractTableModel {
//
//	Q_OBJECT
//
//public:
//	FrontEndFilterModel(QObject* parent = nullptr);
//	~FrontEndFilterModel();
//
//	void SetFilters(const SynGlyphX::MultiTableFrontEndFilters& filters);
//	const SynGlyphX::MultiTableFrontEndFilters& GetFilters() const;
//
//	void SetCurrentTable(const SynGlyphX::InputTable& table);
//	SynGlyphX::InputTable GetCurrentTable();
//
//	void AddField(const SynGlyphX::InputField& newField);
//	void RemoveFields(const std::set<std::wstring>& fieldsToRemove);
//
//	void RemoveTable(const SynGlyphX::InputTable& table);
//
//	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
//	int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 3; }
//	Qt::ItemFlags flags(const QModelIndex& index) const override;
//	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
//
//	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
//	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
//
//	QStringList mimeTypes() const override;
//	bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
//	QMimeData* mimeData(const QModelIndexList& indexes) const override;
//	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
//
//private:
//	SynGlyphX::MultiTableFrontEndFilters m_filters;
//	SynGlyphX::MultiTableFrontEndFilters::iterator m_currentTableFilters;
//};

//class FilterSetupWidget : public QWidget
//{
//	Q_OBJECT
//
//public:
//	FilterSetupWidget(QWidget *parent);
//	~FilterSetupWidget();
//
//	const SynGlyphX::MultiTableFrontEndFilters& GetFilters() const;
//	void SetFilters(const SynGlyphX::MultiTableFrontEndFilters& filters);
//
//protected:
//	void dragEnterEvent(QDragEnterEvent* event) override;
//	void dropEvent(QDropEvent* event) override;
//
//private slots:
//	void OnTableSelected(int index);
//	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
//	void OnRemoveSelected();
//	void OnRemoveTable();
//	void OnRemoveAllTables();
//
//private:
//	bool IsTableInUse(const SynGlyphX::InputTable& table) const;
//	void AddTable(const SynGlyphX::InputTable& table);
//	void UpdateContextMenuEnableStates();
//
//	QComboBox* m_tableComboBox;
//	QTableView* m_table;
//	FrontEndFilterModel* m_model;
//	std::vector<SynGlyphX::InputTable> m_inputTables;
//
//	QAction* m_removeSelectedAction;
//	QAction* m_removeTableAction;
//	QAction* m_removeAllTablesAction;
//};

class FilterTableWidget;
class FilterSetupWidget : public QWidget
{
	Q_OBJECT

public:
	FilterSetupWidget(QWidget *parent);
	~FilterSetupWidget() {}

	const SynGlyphX::MultiTableFrontEndFilters& GetFilters() const;
	void SetFilters(const SynGlyphX::MultiTableFrontEndFilters& filters);

protected:
	//void dragEnterEvent(QDragEnterEvent* event) override;
	//void dropEvent(QDropEvent* event) override;
private:
	FilterTableWidget* m_table;
	SynGlyphX::MultiTableFrontEndFilters m_filters;
	//QAction* m_newAction;
	//QAction* m_removeTableAction;
	//QAction* m_removeAllTablesAction;
};

//#pragma once
