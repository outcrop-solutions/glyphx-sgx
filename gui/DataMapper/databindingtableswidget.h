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
#ifndef DATABINDINGTABLESWIDGET_H
#define DATABINDINGTABLESWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include "verticaltabordertableview.h"
#include "bindinglineedit.h"
#include "glyphrolestablemodel.h"
#include "surfaceradiobuttonwidget.h"
#include "mappingfunctionwidget.h"
#include "tablesubsetproxymodel.h"
#include <map>

class QDataWidgetMapper;
class QDoubleSpinBox;

class DataBindingTablesWidget : public QTabWidget
{
	Q_OBJECT

public:
	DataBindingTablesWidget(GlyphRolesTableModel* model, QWidget *parent = 0);
	~DataBindingTablesWidget();

public slots:
	void CommitChanges();
	void OnBaseObjectChanged();

private slots:
	void OnModelDataChanged();
	void OnTorusRatioChanged();

private:
	void CreateURLTab();
	void CreateGeometryTopologyTab();
	void CreateAnimationTable();
	void CreateTagAndDescriptionWidget();
	void CreateBasePropertiesTable();
	SynGlyphX::VerticalTabOrderTableView* CreateSubsetTableView(const SynGlyphX::TableSubsetProxyModel::Subset& rowSubset, const SynGlyphX::TableSubsetProxyModel::Subset& columnSubset = SynGlyphX::TableSubsetProxyModel::Subset());
	void CreateIntegerPropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow, int min = 0, int max = 255);
	void CreateDoublePropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow, double min = -100000.0, double max = 100000.0, bool addToPositionXYList = false);
	void CreateColorPropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow);
	void CreateGeometryShapePropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow);
	void CreateVirtualTopologyTypePropertyWidgets(SynGlyphX::VerticalTabOrderTableView* tableView, int modelRow);
	QDataWidgetMapper* AddRowOfWidgetsToTable(SynGlyphX::VerticalTabOrderTableView* tableView, QWidget* valueWidget, QWidget* valueWidgetToMap, MappingFunctionWidget* mappingFunctionWidget, int modelRow, bool addToPositionXYList = false);
	void EnablePositionXYMixMaxWidgets(bool enable);

	std::map<QDataWidgetMapper*, unsigned int> m_dataWidgetMappersAndRows;
	GlyphRolesTableModel* m_model;
	QList<QWidget*> m_positionXYMinMaxWidgets;
	SynGlyphX::SurfaceRadioButtonWidget* m_surfaceRadioButtonWidget;
	QDoubleSpinBox* m_torusRatioSpinBox;
	QDataWidgetMapper* m_torusRatioMapper;

	static const QMargins s_cellMargins;
};

#endif // DATABINDINGTABLESWIDGET_H
