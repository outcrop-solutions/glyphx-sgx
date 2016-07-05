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

#ifndef LOADINGSCREENWIDGET_H
#define LOADINGSCREENWIDGET_H

#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QPushButton>
#include "sourcedatacache.h"

class GlyphViewerWindow;
class LoadingFilterWidget;

namespace SynGlyphX {

	class TitleListWidget;
}

class LoadingScreenWidget : public QFrame
{
	Q_OBJECT

public:
	LoadingScreenWidget(GlyphViewerWindow* mainWindow, QWidget *parent);
	~LoadingScreenWidget();

	static QString GetGlyphEdDir();

private slots:
	void OnLoadVisualization();
	void OnNewOptionSelected(int index);

private:
	class VisualizationData {

	public:
		VisualizationData() {}
		~VisualizationData() {}

		bool HasDataForFilter(unsigned int index) const { return index < m_filterTitles.size(); }

		QString m_title;
		QString m_sdtPath;
		QString m_tableInGlyphEd;
		
		std::vector<bool> m_mustHaveFilter;
		std::vector<QString> m_filterTitles;
		std::vector<QString> m_filterFieldNames;
		std::vector<bool> m_filterMultiselect;
		std::vector<QStringList> m_filterValues;
	};

	void CreateHomePageOptionsWidget();
	void CreateAllViewsWidget();
	void CreateRecentViewsWidget();
	void CreateMyViewsWidget();
	void CreateHelpWidget();
	void CreateDashboardWidget();

	void SetupVisualizationData();

	static QString s_glyphEdDir;

	QGridLayout* m_mainLayout;
	QStackedLayout* m_homePageWidgetsLayout;

	QButtonGroup* m_optionsButtonGroup;

	QPushButton* m_loadVisualizationButton;

	SynGlyphX::TitleListWidget* m_viewListWidget;
	QList<LoadingFilterWidget*> m_loadingFilterWidgets;
	QStackedWidget* m_loadingFilterStackedWidget;
	
	GlyphViewerWindow* m_mainWindow;
	SourceDataCache m_sourceDataCache;
	std::vector<VisualizationData> m_visualizationData;
};

#endif // LOADINGSCREENWIDGET_H
