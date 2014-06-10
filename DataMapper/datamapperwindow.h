#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include <QtWidgets/QMainWindow>

class DataMapperWindow : public QMainWindow
{
    Q_OBJECT

public:
    DataMapperWindow(QWidget *parent = 0);
    ~DataMapperWindow();

private slots:
    void ShowAboutBox();
    void CreateNewProject();
    void AddDataFiles();
    void ExportToGlyphViewer();

private:
    void CreateMenus();

    QMenu* m_fileMenu;
    QMenu* m_projectMenu;
    QMenu* m_viewMenu;
    QMenu* m_helpMenu;
};

#endif // DATAMAPPERWINDOW_H
