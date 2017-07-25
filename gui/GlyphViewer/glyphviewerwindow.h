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
#include "mainwindow.h"
#include "datatransformmodel.h"
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
#include "DistinctValueFilteringParameters.h"
#include "SettingsStoredFileList.h"

class HomePageWidget;
class FrontEndFilterListWidget; 
class InteractiveLegend;
namespace SynGlyphX
{
	class SceneViewer;
	class GlyphForestInfoModel;
}

class GlyphViewerWindow : public SynGlyphX::MainWindow
{
	Q_OBJECT

public:
	GlyphViewerWindow(QWidget *parent = 0);
	~GlyphViewerWindow();
	void closeJVM();

	bool IsUserLoggedIn();
	void UpdateUserMenu();

	static const SynGlyphX::SettingsStoredFileList& GetSubsetFileListInstance();
	static void ClearSubsetFileList() { s_subsetFileList.ClearFiles(); }
	static void ClearRecentFileList();
	static void AddSubsetVisualization(const QString& filename);
	void CreateUserSettingsDialog(QStringList groups);
	void SetSelectedGroup(QString selected){ m_groupsComboBox->setCurrentIndex(m_groupsComboBox->findText(selected)); }

public slots:
	bool LoadNewVisualization(const QString& filename, MultiTableDistinctValueFilteringParameters filters = MultiTableDistinctValueFilteringParameters(), bool useFEFilterListWidget = false);
	void Logout();

protected:
	void ReadSettings() override;
	void WriteSettings() override;

	void closeEvent(QCloseEvent* event) override;

	bool DoesHelpExist() const override;

	void UpdateFilenameWindowTitle(const QString& title) override;
	QString GetApplicationDisplayName() const override;

private slots:
	void OpenVisualisation();
	void OpenProject();
	void RefreshVisualization();
	void CloseVisualization();
	void ChangeMapDownloadSettings();
	void ShowOpenGLSettings();
	void ChangeStereoMode();
	void ChangeOptions();
	void ChangeBackgroundColor();
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void CreatePortableVisualization(SynGlyphX::PortableVisualizationExport::Platform platform);
	void RemapRootPositionMappings();
	void ToggleInteractiveLegend();
	void OnStereoSetup(bool stereoEnabled);
	void OnShowHideHUDAxis(bool show);
	void OnShowHideSceneAxis(bool show);
	void OnEnableDisableFlyToObjectAction( bool enable );
	void OnEnableDisableFreeSelectionCamera( bool enable );
	void OnEnableDisableSelEffect( bool enable );
	void OnEnableDisableSuperimposedGadgets( bool enable );
	void OnOpenURLs();
	void OnPropertiesActivated();
	bool LoadRecentFile(const QString& filename) override;
	void SwitchVisualizationGroup();

private:
	class HUDGenerationInfo {

	public:
		HUDGenerationInfo(const SynGlyphX::InputTable& table, const QMap<unsigned int, QString>& fields, const QMap<unsigned int, QString>& displayNames) :
			m_table(table), m_fields(fields), m_displayNames(displayNames) {}
		HUDGenerationInfo(const HUDGenerationInfo& info) :
			m_table(info.m_table), m_fields(info.m_fields), m_displayNames(info.m_displayNames) {}
		~HUDGenerationInfo() {}

		const SynGlyphX::InputTable& GetTable() const { return m_table; }
		const QMap<unsigned int, QString>& GetFields() const { return m_fields; }
		const QMap<unsigned int, QString>& GetDisplayNames() const { return m_displayNames; }

	private:
		SynGlyphX::InputTable m_table;
		QMap<unsigned int, QString> m_fields;
		QMap<unsigned int, QString> m_displayNames;
	};
	void LoadVisualization(const QString& filename, const MultiTableDistinctValueFilteringParameters& filters = MultiTableDistinctValueFilteringParameters());
	void LoadDataTransform(const QString& filename, const MultiTableDistinctValueFilteringParameters& filters);
	void ValidateDataMappingFile(SynGlyphX::DataTransformMapping::SharedPtr mapping, const QString& filename);
	void LoadFilesIntoModel();
	void CreateMenus();
	void CreateDockWidgets();
	void EnableLoadedVisualizationDependentActions(bool enable);
	void ChangeOptions(const GlyphViewerOptions& oldOptions, const GlyphViewerOptions& newOptions);
	void ClearAllData();
	void CreateSceneViewer();
	GlyphViewerOptions CollectOptions();
	bool DoesVisualizationNeedToBeRecreated(const SynGlyphX::DataTransformMapping& mapping) const;
	void CreateExportToPortableVisualizationSubmenu();
	void DownloadBaseImages(DataEngine::GlyphEngine& ge);
	void CreateLoadingScreen();
	void CreateInteractionToolbar();
	void UpdateAxisNamesAndSourceDataPosition();

	QMenu* m_fileMenu;
	QMenu* m_toolsMenu;
	QMenu* m_toolbarsSubMenu;
	QToolBar* m_fileToolbar;
	QToolBar* m_showHideToolbar;
	QToolBar* m_interactionToolbar;

	QAction* m_stereoAction;
	QAction* m_showAnimation;
	QAction* m_showTagsAction;
	QAction* m_hideTagsAction;
	QAction* m_hideAllTagsAction;
	QAction* m_clearSelectionAction;
	QAction* m_resetCameraToDefaultPosition;
	QAction* m_remapRootPositionMappingsAction;
	QAction* m_enableDisableSelEffectActionMenu;
	QAction* m_exportGlyphPortableAction;
	QAction* m_ToggleFEFilterListAction;
	QAction* m_interactiveLegendAction;
	QList<QAction*> m_loadedVisualizationDependentActions;

	QAction* m_showHideHUDAxisAction;
	QAction* m_showHideSceneAxisAction;
	QAction* m_enableDisableFlyToObjectAction;
	QAction* m_enableDisableFreeSelectionCameraAction;
	QAction* m_enableDisableSelEffectAction;
	QAction* m_enableDisableSuperimposedGlyphGadgets;

	QAction* m_openURLAction;
	QAction* m_propertiesAction;

	LegendsDisplayWidget* m_legendsWidget;
	QDockWidget* m_interactiveLegendDock = nullptr;
	InteractiveLegend* m_interactiveLegend = nullptr;
	QDockWidget* m_legendsDockWidget;
	QDockWidget* m_leftDockWidget;
	QDockWidget* m_rightDockWidget;
	QDockWidget* m_bottomDockWidget;

	SynGlyphX::DataTransformModel* m_mappingModel;
	CacheManager m_cacheManager;
	bool m_showErrorFromTransform;
	bool m_showHomePage;
	QString m_defaultProject;
	QStringList m_currentBaseImages;

	QComboBox* m_groupsComboBox;

	LinkedWidgetsManager* m_linkedWidgetsManager;

	SynGlyphX::GlyphForestInfoModel* m_glyphForestModel;
	SynGlyphX::ItemFocusSelectionModel* m_glyphForestSelectionModel;
	SynGlyphX::SceneViewer* m_viewer;
	GlyphPropertiesWidgetsContainer* m_glyphPropertiesWidgetContainer;
	SourceDataCache::SharedPtr m_sourceDataCache;
	FilteringWidget* m_filteringWidget;
	FilteringManager* m_filteringManager;
	FrontEndFilterListWidget* m_FEfilterListWidget;
	PseudoTimeFilterWidget* m_pseudoTimeFilterWidget;
	DataEngine::DataEngineConnection::SharedPtr m_dataEngineConnection;
	SynGlyphX::PortableVisualizationExport m_portableVisualizationExport;
	SourceDataInfoModel* m_columnsModel;

	HomePageWidget* m_homePage;

	QCheckBox* m_hideFilteredCheckBox;

	std::vector<HUDGenerationInfo> m_hudGenerationInfo;

	static SynGlyphX::SettingsStoredFileList s_subsetFileList;
	static QMap<QString, MultiTableDistinctValueFilteringParameters> s_recentFilters;
};

#endif // GLYPHVIEWERWINDOW_H
