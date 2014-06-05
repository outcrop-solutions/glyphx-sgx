#ifndef GLYPHDESIGNERWINDOW_H
#define GLYPHDESIGNERWINDOW_H

#include "mainwindow.h"
#include "glyphtreeview.h"
#include "antzwidget.h"
#include "glyphtreemodel.h"
#include "sharedactionmanager.h"

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
    void ShowAboutBox();
    void EditingModeChanged(QAction* action);
    void SwitchBetweenFullAndNormalScreen();

private:
    void CreateMenus();
    void CreateDockWidgets();
    void LoadTemplate(const QString& filename);
    bool SaveTemplateFile(const QString& filename);
    bool AskUserToSave();

    ANTzWidget* m_3dView;

    GlyphTreeView* m_treeView;

    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_glyphMenu;
    QMenu* m_helpMenu;

    GlyphTreeModel* m_glyphTreeModel;
    QItemSelectionModel* m_selectionModel;

    SharedActionManager* m_sharedActions;

    QAction* m_fullScreenAction;
};

#endif // GLYPHDESIGNERWINDOW_H
