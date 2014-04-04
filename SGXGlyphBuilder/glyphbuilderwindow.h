#ifndef GLYPHBUILDERWINDOW_H
#define GLYPHBUILDERWINDOW_H

#include <QtWidgets/QMainWindow>
#include "glyphtreeview.h"

class GlyphBuilderWindow : public QMainWindow
{
    Q_OBJECT

public:
    GlyphBuilderWindow(QWidget *parent = 0);
    ~GlyphBuilderWindow();

private slots:
    void CreateNewGlyph();
    void SaveGlyph();

private:
    void CreateMenus();
    void CreateDockWidgets();

    GlyphTreeView* m_treeView;

    QMenu* m_fileMenu;
    QMenu* m_viewMenu;
};

#endif // GLYPHBUILDERWINDOW_H
