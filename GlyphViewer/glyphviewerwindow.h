#ifndef GLYPHVIEWERWINDOW_H
#define GLYPHVIEWERWINDOW_H

#include <QtWidgets/QMainWindow>
#include "mainwindow.h"

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
	void ConvertTransformToANTz(SynGlyphX::DataTransform& transform);

	QMenu* m_fileMenu;
	QMenu* m_viewMenu;
	QMenu* m_toolsMenu;
	QString m_cacheDir;
};

#endif // GLYPHVIEWERWINDOW_H
