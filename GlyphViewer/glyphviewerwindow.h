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
#include "datamappingmodel.h"
#include "glyphforestmodel.h"
#include "antzforestwidget.h"
#include "cachemanager.h"
#include "glyphvieweroptions.h"
#include "sourcedatacache.h"
#include "filteringwidget.h"
#include "pseudotimefilterwidget.h"
#include "linkedwidgetsmanager.h"
#include "itemfocusselectionmodel.h"
#include "antzcsvwriter.h"
#include "glyphpropertieswidgetscontainer.h"
#include "dataengineconnection.h"
#include "glyphengine.h"
#include "portablevisualizationexport.h"
#include "legendsdisplaywidget.h"

class GlyphViewerWindow : public SynGlyphX::MainWindow
{
	Q_OBJECT

public:
	GlyphViewerWindow(QWidget *parent = 0);
	~GlyphViewerWindow();
	void closeJVM();

protected:
	virtual void ReadSettings();
	virtual void WriteSettings();

private slots:
	void OpenProject();
	void RefreshVisualization();
	void CloseVisualization();
	void ChangeMapDownloadSettings();
	void ShowOpenGLSettings();
	void ChangeStereoMode();
	void ImportFilesFromANTz();
	void ChangeOptions();
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void CreatePortableVisualization(SynGlyphX::PortableVisualizationExport::Platform platform);

private:
	virtual bool LoadRecentFile(const QString& filename);
	bool LoadNewVisualization(const QString& filename);
	void LoadVisualization(const QString& filename);
	void LoadANTzCompatibilityVisualization(const QString& filename);
	void LoadDataTransform(const QString& filename);
	void ValidateDataMappingFile(const QString& filename);
	void LoadFilesIntoModel(const SynGlyphXANTz::ANTzCSVWriter::FilenameList& filesToLoad, const QStringList& baseImageFilenames);
	void CreateMenus();
	void CreateDockWidgets();
	void EnableLoadedVisualizationDependentActions(bool enable);
	void ChangeOptions(const GlyphViewerOptions& oldOptions, const GlyphViewerOptions& newOptions);
	void ClearAllData();
	void CreateANTzWidget();
	GlyphViewerOptions CollectOptions();
	bool DoesVisualizationNeedToBeRecreated(const SynGlyphX::DataTransformMapping& mapping) const;
	void CreateExportToPortableVisualizationSubmenu();
	void DownloadBaseImages(DataEngine::GlyphEngine& ge);
	void CreateLoadingScreen();

	QMenu* m_fileMenu;
	QMenu* m_toolsMenu;
	QToolBar* m_toolbar;
	QAction* m_stereoAction;
	QAction* m_showAnimation;
	QAction* m_showTagsAction;
	QAction* m_clearSelectionAction;
	QList<QAction*> m_loadedVisualizationDependentActions;

	QDockWidget* m_glyphListDockWidget;
	LegendsDisplayWidget* m_legendsWidget;
	QDockWidget* m_legendsDockWidget;

	SynGlyphX::DataMappingModel* m_mappingModel;
	CacheManager m_cacheManager;
	bool m_showErrorFromTransform;

	LinkedWidgetsManager* m_linkedWidgetsManager;

	SynGlyphXANTz::GlyphForestModel* m_glyphForestModel;
	SynGlyphX::ItemFocusSelectionModel* m_glyphForestSelectionModel;
	SynGlyphXANTz::ANTzForestWidget* m_antzWidget;
	GlyphTreeListView* m_treeView;
	GlyphPropertiesWidgetsContainer* m_glyphPropertiesWidgetContainer;
	SourceDataCache::SharedPtr m_sourceDataCache;
	FilteringWidget* m_filteringWidget;
	FilteringManager* m_filteringManager;
	PseudoTimeFilterWidget* m_pseudoTimeFilterWidget;
	DataEngine::DataEngineConnection::SharedPtr m_dataEngineConnection;
	SynGlyphX::PortableVisualizationExport m_portableVisualizationExport;
	SourceDataInfoModel* m_columnsModel;
};

#endif // GLYPHVIEWERWINDOW_H
