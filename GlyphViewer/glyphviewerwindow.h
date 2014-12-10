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
