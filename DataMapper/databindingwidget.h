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

#ifndef DATABINDINGWIDGET_H
#define DATABINDINGWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QDataWidgetMapper>
#include <QtWidgets/QTableView>
#include "bindinglineedit.h"
#include "richtexteditor.h"
#include "datatransformmapping.h"
#include "singleglyphrolestablemodel.h"
#include "nonmappablegeometrywidget.h"
#include "mappingfunctionwidget.h"
#include "tablesubsetproxymodel.h"
#include <map>

class DataBindingWidget : public QTabWidget
{
	Q_OBJECT

public:
	DataBindingWidget(SingleGlyphRolesTableModel* model, QWidget *parent = 0);
	~DataBindingWidget();

public slots:
	void CommitChanges();
	void OnBaseObjectChanged();

private slots:
	void OnModelDataChanged();
	void OnNonMappablePropertiesUpdated();

private:
	void CreateGeometryTopologyTab();
	void CreateAnimationTable();
	void CreateTagAndDescriptionWidget();
	void CreateBasePropertiesTable();
	QTableView* CreateSubsetTableView(const SynGlyphX::TableSubsetProxyModel::Subset& rowSubset, const SynGlyphX::TableSubsetProxyModel::Subset& columnSubset = SynGlyphX::TableSubsetProxyModel::Subset());
	void CreateIntegerPropertyWidgets(QTableView* tableView, int modelRow, int min = 0, int max = 255);
	void CreateDoublePropertyWidgets(QTableView* tableView, int modelRow, double min = -100000.0, double max = 100000.0, bool addToPositionXYList = false);
	void CreateColorPropertyWidgets(QTableView* tableView, int modelRow);
	void CreateGeometryShapePropertyWidgets(QTableView* tableView, int modelRow);
	void CreateVirtualTopologyTypePropertyWidgets(QTableView* tableView, int modelRow);
	QDataWidgetMapper* CreateMapper(QWidget* parent, QWidget* valueWidget, MappingFunctionWidget* mappingFunctionWidget, int modelRow);
	void AddRowOfWidgetsToTable(QTableView* tableView, const QList<QWidget*> widgets, int modelRow, bool addToPositionXYList = false);
	void EnablePositionXYMixMaxWidgets(bool enable);

	BindingLineEdit* m_tagLineEdit;
	SynGlyphX::RichTextEditor* m_descriptionEdit;
	std::map<QDataWidgetMapper*, unsigned int> m_dataWidgetMappersAndRows;
	SingleGlyphRolesTableModel* m_model;
	QList<QWidget*> m_positionXYMinMaxWidgets;
	SynGlyphX::NonMappableGeometryWidget* m_nonMappableGeometryWidget;
};

#endif // DATABINDINGWIDGET_H
