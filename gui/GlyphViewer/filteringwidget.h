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

#ifndef FILTERINGWIDGET_H
#define FILTERINGWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include "filteredsourcedatawidget.h"
#include "selectedsourcedatawidget.h"
#include "filteringmanager.h"
#include "linkedwidgetsmanager.h"
#include "sourcedatainfomodel.h"
#include "multitableelasticlistswidget.h"
#include "rangefilterlistwidget.h"
#include "keywordfilterlistwidget.h"

class FilteringWidget : public QWidget
{
	Q_OBJECT

public:
	FilteringWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent);
	~FilteringWidget();

	void SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgets);
	void OnNewVisualization();
	void CloseSourceDataWidgets();

	void SetLoadSubsetVisualization(bool loadSubsetVisualization);
	bool GetLoadSubsetVisualization() const;
	void SetLoadSubsetVisualizationInNewInstance(bool loadSubsetVisualizationInNewInstance);
	bool GetLoadSubsetVisualizationInNewInstance() const;
	void SetElasticListFields(std::map<std::wstring, std::vector<std::wstring>> elasticListMap);

signals:
	void LoadSubsetVisualization(QString filename);

private slots:
	void Clear();
	void OnSourceWidgetWindowHidden();
	void OnSelectedSourceWidgetWindowHidden();
	void OnFilterResultsChanged();
	void OnTableChanged(const QString& table);
	void OnUserSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void OnSubsetVisualizationCreated(const QString& subsetVisualizationFilename);

private:
	void EnableFilterRelatedButtons(bool enable);

	QComboBox* m_tableComboBox;
	QCheckBox* m_hideUnselectedTreesCheckbox;
	QPushButton* m_sourceWidgetButton;
	QPushButton* m_selectedSourceWidgetButton;
	QPushButton* m_createSubsetVizButton;
	QPushButton* m_clearButton;

	QScopedPointer<FilteredSourceDataWidget> m_filteredSourceDataWindow;
	QScopedPointer<SelectedSourceDataWidget> m_selectedSourceDataWindow;

	RangeFilterListWidget* m_rangeListFilterWidget;
	MultiTableElasticListsWidget* m_elasticListsWidget;
	KeywordFilterListWidget* m_keywordFilterListWidget;

	FilteringManager* m_filteringManager;

	bool m_loadSubsetVisualization;
	bool m_loadSubsetVisualizationInNewInstance;
};

#endif // FILTERINGWIDGET_H
