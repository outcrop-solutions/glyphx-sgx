#ifndef GLYPHVIEWERWINDOW_H
#define GLYPHVIEWERWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTreeView>
#include "mainwindow.h"
#include "datatransform.h"
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

private:
	virtual void LoadRecentFile(const QString& filename);
	void LoadDataTransform(const QString& filename);
	void CreateMenus();
	void CreateDockWidgets();
	void ConvertTransformToANTz(const QString& transformFilename);

	QMenu* m_fileMenu;
	QMenu* m_viewMenu;
	QMenu* m_toolsMenu;
	QString m_cacheDir;

	GlyphForestModel* m_glyphForestModel;
	ANTzViewerWidget* m_antzWidget;
	QTreeView* m_treeView;
};

#endif // GLYPHVIEWERWINDOW_H
