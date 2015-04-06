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

#ifndef SOURCEDATASELECTIONWIDGET_H
#define SOURCEDATASELECTIONWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QCheckBox>
#include "sourcedatawidget.h"
#include "singlewidgetdialog.h"
#include "glyphforestmodel.h"
#include <unordered_map>
#include "elasticlistswidget.h"

class SourceDataSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	SourceDataSelectionWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent);
	~SourceDataSelectionWidget();

	void SetHideUnselectedTreesCheckbox(bool checked);
	bool GetHideUnselectedTreesCheckbox() const;

signals:
	void OptionsChanged();

private slots:
	void OnSourceWidgetWindowHidden();
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnModelReset();
	void OnComboBoxChanged(int current);
	void OnElasticListsSelectionChanged(const QString& table, const SynGlyphX::SourceDataCache::ColumnValueData& selection);

private:
	typedef std::unordered_map<std::string, ElasticListsWidget*> NameWidgetMap;

	void UpdateElasticListsAndSourceDataWidget(const QModelIndexList& selectedIndexes);
	void UpdateElasticLists(const SynGlyphX::SourceDataCache::IndexSetMap& dataIndexes = SynGlyphX::SourceDataCache::IndexSetMap());
	void ClearElasticLists();

	GlyphForestModel* m_model;
	QItemSelectionModel* m_selectionModel;
	QPushButton* m_sourceWidgetButton;
	QScopedPointer<SourceDataWidget> m_sourceDataWindow;
	QComboBox* m_tableComboBox;
	QCheckBox* m_hideUnselectedTreesCheckbox;

	QStackedLayout* m_elasticListsStackLayout;
	NameWidgetMap m_elasticListWidgetsForEachTable;

	SynGlyphX::SourceDataCache::SharedPtr m_sourceDataCache;
};

#endif // SOURCEDATASELECTIONWIDGET_H
