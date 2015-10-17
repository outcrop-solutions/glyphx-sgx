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

#ifndef MULTITABLEELASTICLISTSWIDGET_H
#define MULTITABLEELASTICLISTSWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QCheckBox>
#include "sourcedatawidget.h"
#include "singlewidgetdialog.h"
#include <unordered_map>
#include "singletableelasticlistswidget.h"
#include "linkedwidgetsmanager.h"
#include "sourcedataselectionmodel.h"

class MultiTableElasticListsWidget : public QWidget
{
	Q_OBJECT

public:
	MultiTableElasticListsWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent);
	~MultiTableElasticListsWidget();

	void SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgets);

private slots:
	void OnSourceWidgetWindowHidden();
	void OnSelectionChanged();
	void OnModelReset();
	void OnComboBoxChanged(int current);
	void OnElasticListsSelectionChanged(const QString& table, const SynGlyphX::SourceDataCache::ColumnValueData& selection);
	void Clear();

private:
	typedef std::unordered_map<std::string, SingleTableElasticListsWidget*> NameWidgetMap;

	void UpdateElasticListsAndSourceDataWidget();
	void UpdateElasticLists(const SourceDataSelectionModel::IndexSetMap& dataIndexes = SourceDataSelectionModel::IndexSetMap());
	void ClearElasticLists();
	void EnableButtons(bool enable);

	SourceDataSelectionModel* m_selectionModel;
	QPushButton* m_sourceWidgetButton;
	QPushButton* m_clearButton;
	QScopedPointer<SourceDataWidget> m_sourceDataWindow;
	QComboBox* m_tableComboBox;
	QCheckBox* m_hideUnselectedTreesCheckbox;

	QStackedLayout* m_elasticListsStackLayout;
	NameWidgetMap m_elasticListWidgetsForEachTable;

	SynGlyphX::SourceDataCache::SharedPtr m_sourceDataCache;
};

#endif // MULTITABLEELASTICLISTSWIDGET_H
