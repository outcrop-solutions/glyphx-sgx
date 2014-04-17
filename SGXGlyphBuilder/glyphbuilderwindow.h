#ifndef GLYPHBUILDERWINDOW_H
#define GLYPHBUILDERWINDOW_H

#include <QtWidgets/QMainWindow>
#include "glyphtreeview.h"
#include "antzwidget.h"
#include "glyphtreemodel.h"

class GlyphBuilderWindow : public QMainWindow
{
    Q_OBJECT

public:
    GlyphBuilderWindow(QWidget *parent = 0);
    ~GlyphBuilderWindow();

private slots:
    void CreateNewGlyphTree();
    void SaveGlyph();

private:
    void CreateMenus();
    void CreateDockWidgets();

    ANTzWidget* m_3dView;

    GlyphTreeView* m_treeView;

    QMenu* m_fileMenu;
    QMenu* m_viewMenu;
    QMenu* m_glyphMenu;

    GlyphTreeModel* m_glyphTreeModel;
};

#endif // GLYPHBUILDERWINDOW_H
