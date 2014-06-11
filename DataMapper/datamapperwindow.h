#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include "mainwindow.h"

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
    void AddDataFiles();
    void ExportToGlyphViewer();
    void ChangeBaseImage();

private:
    void CreateMenus();

    QMenu* m_fileMenu;
    QMenu* m_projectMenu;
    QMenu* m_viewMenu;
    QMenu* m_helpMenu;
};

#endif // DATAMAPPERWINDOW_H
