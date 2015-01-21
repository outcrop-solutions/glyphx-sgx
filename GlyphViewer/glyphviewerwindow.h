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

#ifndef GLYPHVIEWERWINDOW_H
#define GLYPHVIEWERWINDOW_H

#include <QtWidgets/QMainWindow>
#include "glyphtreelistview.h"
#include "mainwindow.h"
#include "datatransformmapping.h"
#include "glyphforestmodel.h"
#include "antzviewerwidget.h"
#include "cachemanager.h"

class GlyphViewerWindow : public SynGlyphX::MainWindow
{
	Q_OBJECT

public:
	GlyphViewerWindow(QWidget *parent = 0);
	~GlyphViewerWindow();

private slots:
	void ShowAboutBox();
	void OpenProject();
	void RefreshVisualization();
	void CloseVisualization();
	void ChangeMapDownloadSettings();
	void ShowOpenGLSettings();
	void ChangeStereoMode();
	void ImportFilesFromANTz();

private:
	virtual void LoadRecentFile(const QString& filename);
	void LoadNewVisualization(const QString& filename);
	void LoadVisualization(const QString& filename);
	void LoadANTzCompatibilityVisualization(const QString& filename);
	void LoadDataTransform(const QString& filename);
	void LoadFilesIntoModel(const QStringList& csvFiles, const QStringList& baseImageFilenames);
	void CreateMenus();
	void CreateDockWidgets();
	void EnableLoadedVisualizationDependentActions(bool enable);

	QMenu* m_fileMenu;
	QMenu* m_viewMenu;
	QMenu* m_toolsMenu;
	QAction* m_stereoAction;
	QList<QAction*> m_loadedVisualizationDependentActions;

	CacheManager m_cacheManager;

	GlyphForestModel* m_glyphForestModel;
	ANTzViewerWidget* m_antzWidget;
	GlyphTreeListView* m_treeView;
};

#endif // GLYPHVIEWERWINDOW_H
