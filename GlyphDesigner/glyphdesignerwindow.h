#ifndef GLYPHDESIGNERWINDOW_H
#define GLYPHDESIGNERWINDOW_H

#include "mainwindow.h"
#include "glyphtreeview.h"
#include "antzsingleglyphtreewidget.h"
#include "minmaxglyphtreemodel.h"

class GlyphDesignerWindow : public SynGlyphX::MainWindow
{
    Q_OBJECT

public:
    GlyphDesignerWindow(QWidget *parent = 0);
    ~GlyphDesignerWindow();

protected:
    virtual void LoadRecentFile(const QString& filename);
    virtual void closeEvent(QCloseEvent* event);

private slots:
    void CreateNewGlyphTree();
    void ExportToCSV();
    bool SaveTemplate();
    bool SaveAsTemplate();
    void OpenTemplate();
    void EditingModeChanged(QAction* action);
	void OnModelChanged();

private:
    void CreateMenus();
    void CreateDockWidgets();
    void LoadTemplate(const QString& filename);
    bool SaveTemplateFile(const QString& filename);
    bool AskUserToSave();

    ANTzSingleGlyphTreeWidget* m_3dView;
    GlyphTreeView* m_treeView;

    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_glyphMenu;

    MinMaxGlyphTreeModel* m_glyphTreeModel;
	QItemSelectionModel* m_sharedSelectionModel;

	bool m_isFileLoadingOrDefaultGlyphSet;
};

#endif // GLYPHDESIGNERWINDOW_H
