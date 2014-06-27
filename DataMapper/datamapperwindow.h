#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include "mainwindow.h"
#include <QtWidgets/QTreeView>
#include <QtSql/QSqlDatabase>
#include "datasourcestatswidget.h"
#include "databindingwidget.h"

class DataMapperWindow : public SynGlyphX::MainWindow
{
    Q_OBJECT

public:
    DataMapperWindow(QWidget *parent = 0);
    ~DataMapperWindow();

protected:
    virtual void LoadRecentFile(const QString& filename);
	virtual void closeEvent(QCloseEvent* event);

private slots:
    void ShowAboutBox();
    void CreateNewProject();
    void OpenProject();
    bool SaveProject();
    bool SaveAsProject();
    void AddDataSources();
    void ExportToGlyphViewer();
    void ChangeBaseImage();

private:
    void CreateMenus();
    void CreateDockWidgets();
	void LoadProjectDatabase(const QString& filename);
	bool SaveProjectDatabase(const QString& filename);
	bool AskUserToSave();
	void EnableProjectDependentActions(bool enable);

    QMenu* m_fileMenu;
    QMenu* m_projectMenu;
    QMenu* m_viewMenu;
    QTreeView* m_glyphTreeView;
	QSqlDatabase m_projectDatabase;
	DataSourceStatsWidget* m_dataSourceStats;
	QList<QAction*> m_projectDependentActions;
	DataBindingWidget* m_dataBindingWidget;
};

#endif // DATAMAPPERWINDOW_H
