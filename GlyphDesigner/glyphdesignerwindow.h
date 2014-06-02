#ifndef GLYPHDESIGNERWINDOW_H
#define GLYPHDESIGNERWINDOW_H

#include <QtWidgets/QMainWindow>
#include "glyphtreeview.h"
#include "antzwidget.h"
#include "glyphtreemodel.h"
#include "sharedactionmanager.h"

class GlyphDesignerWindow : public QMainWindow
{
    Q_OBJECT

public:
    GlyphDesignerWindow(QWidget *parent = 0);
    ~GlyphDesignerWindow();

private slots:
    void CreateNewGlyphTree();
    void SaveGlyph();
    void SaveAsTemplate();
    void OpenTemplate();
    void ShowAboutBox();
    void EditingModeChanged(QAction* action);

private:
    void CreateMenus();
    void CreateDockWidgets();

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
};

#endif // GLYPHDESIGNERWINDOW_H
