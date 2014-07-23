#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include "mainwindow.h"
#include <QtWidgets/QTreeView>
#include <boost/shared_ptr.hpp>
#include "datatransform.h"
#include "datasourcestatswidget.h"
#include "databindingwidget.h"
#include "glyphtemplatesmodel.h"

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
	void AddGlyphTemplate();

private:
    void CreateMenus();
    void CreateDockWidgets();
	void LoadDataTransform(const QString& filename);
	bool SaveDataTransform(const QString& filename);
	bool AskUserToSave();
	void EnableProjectDependentActions(bool enable);
	bool ValidateNewDatasource(const QString& datasource);

    QMenu* m_fileMenu;
    QMenu* m_projectMenu;
    QMenu* m_viewMenu;
    QTreeView* m_glyphTreeView;
	SynGlyphX::DataTransform::SharedPtr m_transform;
	DataSourceStatsWidget* m_dataSourceStats;
	QList<QAction*> m_projectDependentActions;
	DataBindingWidget* m_dataBindingWidget;
	GlyphTemplatesModel* m_glyphTemplatesModel;
};

#endif // DATAMAPPERWINDOW_H
