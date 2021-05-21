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

#ifndef GLYPHEDVIEWERWINDOW_H
#define GLYPHEDVIEWERWINDOW_H

#include <QtWidgets/QMainWindow>
#include "mainwindow.h"
#include "datatransformmodel.h"
#include "cachemanager.h"
#include "glyphvieweroptions.h"
#include "linkedwidgetsmanager.h"
#include "itemfocusselectionmodel.h"
#include "antzcsvwriter.h"
#include "glyphpropertieswidgetscontainer.h"
#include "dataengineconnection.h"
#include "glyphengine.h"
#include "legendsdisplaywidget.h"
#include "SettingsStoredFileList.h"
#include <QtWidgets/QStackedWidget>
#include <QtWebSockets/QWebSocket>
#include <QtCore/QFile>
#include <QtWebEngineWidgets/QWebEngineView>

class InteractiveLegendWindow;
namespace SynGlyphX
{
	class SceneViewer;
}

class GlyphEdViewerWindow : public SynGlyphX::MainWindow
{
	Q_OBJECT

public:
	GlyphEdViewerWindow(QWidget *parent = 0);
	~GlyphEdViewerWindow();
	void closeJVM();

protected:
	void resizeEvent(QResizeEvent* resizeEvent) override;
	void closeEvent(QCloseEvent* event) override;

	bool DoesHelpExist() const override;

	void UpdateFilenameWindowTitle(const QString& title) override;
	QString GetApplicationDisplayName() const override;

private slots:
	void ShowOpenGLSettings();
	void ChangeStereoMode();
	void ChangeBackgroundColor();
	void OnStereoSetup(bool stereoEnabled);
	void OnShowHideHUDAxis(bool show);
	void OnShowHideSceneAxis(bool show);
	void OnEnableDisableFlyToObjectAction( bool enable );
	void OnEnableDisableFreeSelectionCamera( bool enable );
	void OnEnableDisableSelEffect( bool enable );
	void OnEnableDisableSuperimposedGadgets( bool enable );
	void OnSocketConnect();
	void OnSocketLaunch(QString message);
	void OnSocketSslErrors(const QList<QSslError> &errors);
	void OnSocketClosed();
	void ToggleInteractiveLegend();

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
	void LoadVisualization(const QString& filename);
	void LoadDataTransform(const QString& filename);
	void ValidateDataMappingFile(SynGlyphX::DataTransformMapping::SharedPtr mapping, const QString& filename);
	void LoadFilesIntoModel();
	void CreateMenus();
	void CreateDockWidgets();
	void EnableLoadedVisualizationDependentActions(bool enable);
	void ClearAllData();
	void CreateANTzWidget();
	GlyphViewerOptions CollectOptions();
	bool DoesVisualizationNeedToBeRecreated(const SynGlyphX::DataTransformMapping& mapping) const;
	void CreateExportToPortableVisualizationSubmenu();
	void CreateInteractionToolbar();
	SynGlyphX::IndexSet parseFilters(QString response);
	std::vector<std::string> MakeDataRequest(QString query, QString sdt, QStringList legends, QString inst);
	void DownloadBaseImages(DataEngine::GlyphEngine& ge);

	QMenu* m_fileMenu;
	QMenu* m_toolsMenu;
	QMenu* m_toolbarsSubMenu;
	QToolBar* m_interactionToolbar;

	QAction* m_stereoAction;
	QAction* m_showAnimation;
	QAction* m_showTagsAction;
	QAction* m_hideTagsAction;
	QAction* m_hideAllTagsAction;
	QAction* m_clearSelectionAction;
	QAction* m_resetCameraToDefaultPosition;
	QAction* m_enableDisableSelEffectActionMenu;
	QAction* m_interactiveLegendAction;
	QList<QAction*> m_loadedVisualizationDependentActions;

	QAction* m_showHideHUDAxisAction;
	QAction* m_showHideSceneAxisAction;
	QAction* m_enableDisableFlyToObjectAction;
	QAction* m_enableDisableFreeSelectionCameraAction;
	QAction* m_enableDisableSelEffectAction;
	QAction* m_enableDisableSuperimposedGlyphGadgets;

	QAction* m_openURLAction;

	QStackedWidget* centerWidgetsContainer;
	LegendsDisplayWidget* m_legendsWidget;
	QDockWidget* m_interactiveLegendDock = nullptr;
	InteractiveLegendWindow* m_interactiveLegend = nullptr;
	QDockWidget* m_legendsDockWidget;
	QDockWidget* m_rightDockWidget;

	SynGlyphX::DataTransformModel* m_mappingModel;
	CacheManager m_cacheManager;
	bool m_showErrorFromTransform;
	bool m_showHomePage;
	QString m_defaultProject;
	QWebSocket m_webSocket;
	QFile file;
	QString uid;
	QString cache_location;
	DataEngine::DataEngineConnection::SharedPtr m_dataEngineConnection;
	bool zero_results;
	std::vector<std::string> compass;
	bool collapsed;
	QString table_name;
	QStringList filter_ids;
	QWebEngineView* dlg;

	LinkedWidgetsManager* m_linkedWidgetsManager;

	SynGlyphX::SceneViewer* m_viewer;
	GlyphPropertiesWidgetsContainer* m_glyphPropertiesWidgetContainer;

	std::vector<HUDGenerationInfo> m_hudGenerationInfo;
};

#endif // GLYPHEDVIEWERWINDOW_H
