#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include "mainwindow.h"
#include <QtWidgets/QTreeView>

class DataMapperWindow : public SynGlyphX::MainWindow
{
    Q_OBJECT

public:
    DataMapperWindow(QWidget *parent = 0);
    ~DataMapperWindow();

protected:
    virtual void LoadRecentFile(const QString& filename);

private slots:
    void ShowAboutBox();
    void CreateNewProject();
    void OpenProject();
    void SaveProject();
    void SaveAsProject();
    void AddDataSources();
    void ExportToGlyphViewer();
    void ChangeBaseImage();

private:
    void CreateMenus();
    void CreateDockWidgets();

    QMenu* m_fileMenu;
    QMenu* m_projectMenu;
    QMenu* m_viewMenu;
    QTreeView* m_glyphTreeView;
};

#endif // DATAMAPPERWINDOW_H
