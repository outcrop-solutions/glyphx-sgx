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

#include "sourcedatacache.h"
#include <QtWidgets/QFrame>
#include "dataengineconnection.h"

class QStackedWidget;
class QStackedLayout;
class QGridLayout;
class QButtonGroup;
class QPushButton;
class QVBoxLayout;
class QListWidget;
class QListWidgetItem;

class SharedVisualizationsWidget;

namespace SynGlyphX {

	class TitleListWidget;
}

class HomePageWidget : public QFrame
{
	Q_OBJECT

public:
	HomePageWidget(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent);
	~HomePageWidget();

signals:
	void LoadRecentFile(QString filename);
	bool LoadVisualization(const QString& sdtToLoad, const MultiTableDistinctValueFilteringParameters& userSelectedFilters);

private slots:
	void OnLoadVisualization();
	void OnNewOptionSelected(int index);
	void OnRecentListUpdated();
	void OnSubsetListUpdated();
	void OnRecentViewClicked(QListWidgetItem *item);

private:
	void CreateHomePageOptionsWidget();
	void CreateAllViewsWidget();
	void CreateMyViewsWidget();
	void CreateHelpWidget();
	void CreateDashboardWidget();
	void SetupGlyphEdViz();
	void ProduceGlyphEdCSV(const QString& sdtToLoad, const QString& tableInDB, unsigned int currentDataVisualization);

	QGridLayout* m_mainLayout;
	QStackedLayout* m_homePageWidgetsLayout;

	QButtonGroup* m_optionsButtonGroup;

	QPushButton* m_loadVisualizationButton;

	SharedVisualizationsWidget* m_allViewsFilteringWidget;
	QListWidget* m_recentViewsFilteringWidget;

	SynGlyphX::TitleListWidget* m_subsetViewsFilteringWidget;
	
	DataEngine::DataEngineConnection::SharedPtr m_dataEngineConnection;

	//GlyphEd only.  Will get rid of ASAP
	SourceDataCache m_sourceDataCache;
	QStringList m_glyphEdSDTFiles;
	QStringList m_glyphEdTablesInDB;
};

//#pragma once