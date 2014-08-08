#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include "mainwindow.h"
#include <QtWidgets/QTreeView>
#include <boost/shared_ptr.hpp>
#include "datasourcestatswidget.h"
#include "databindingwidget.h"
#include "glyphtemplatesmodel.h"
#include "minmaxglyphmodel.h"
#include "datatransformmodel.h"
#include "antzwidget.h"
#include "glyphtreemodel.h"
#include "selectiontranslator.h"

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
	void ChangeMapDownloadSettings();

private:
    void CreateMenus();
    void CreateDockWidgets();
	void CreateCenterWidget();
	void LoadDataTransform(const QString& filename);
	bool SaveDataTransform(const QString& filename);
	bool AskUserToSave();
	void EnableProjectDependentActions(bool enable);
	bool ValidateNewDatasource(const QString& datasource);

    QMenu* m_fileMenu;
    QMenu* m_projectMenu;
    QMenu* m_viewMenu;
	QMenu* m_toolsMenu;
    QTreeView* m_glyphTreeView;
	DataSourceStatsWidget* m_dataSourceStats;
	QList<QAction*> m_projectDependentActions;
	DataBindingWidget* m_dataBindingWidget;
	//GlyphTemplatesModel* m_glyphTemplatesModel;
	MinMaxGlyphModel* m_minMaxGlyphModel;
	DataTransformModel* m_dataTransformModel;
	ANTzWidget* m_antzWidget;
	GlyphTreeModel* m_glyphTreeModel;
	SelectionTranslator* m_selectionTranslator;
};

#endif // DATAMAPPERWINDOW_H
