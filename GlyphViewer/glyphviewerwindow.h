#ifndef GLYPHVIEWERWINDOW_H
#define GLYPHVIEWERWINDOW_H

#include <QtWidgets/QMainWindow>
#include "glyphtreelistview.h"
#include "mainwindow.h"
#include "datatransformmapping.h"
#include "glyphforestmodel.h"
#include "antzviewerwidget.h"

class GlyphViewerWindow : public SynGlyphX::MainWindow
{
	Q_OBJECT

public:
	GlyphViewerWindow(QWidget *parent = 0);
	~GlyphViewerWindow();

private slots:
	void ShowAboutBox();
	void OpenProject();
	void ChangeMapDownloadSettings();
	void ShowOpenGLSettings();
	void ChangeStereoMode();
	void ImportFilesFromANTz();

private:
	virtual void LoadRecentFile(const QString& filename);
	void LoadVisualization(const QString& filename);
	void LoadANTzCompatibilityVisualization(const QString& filename);
	void LoadDataTransform(const QString& filename);
	void LoadFilesIntoModel(const QStringList& csvFiles, const QString& baseImageFilename);
	void CreateMenus();
	void CreateDockWidgets();

	QMenu* m_fileMenu;
	QMenu* m_viewMenu;
	QMenu* m_toolsMenu;
	QAction* m_stereoAction;

	QString m_cacheDir;

	GlyphForestModel* m_glyphForestModel;
	ANTzViewerWidget* m_antzWidget;
	GlyphTreeListView* m_treeView;
};

#endif // GLYPHVIEWERWINDOW_H
