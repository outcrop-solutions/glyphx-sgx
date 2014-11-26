#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include "mainwindow.h"
#include <QtWidgets/QListView>
#include <boost/shared_ptr.hpp>
#include "datasourcestatswidget.h"
#include "databindingwidget.h"
#include "minmaxglyphmodel.h"
#include "datatransformmodel.h"
#include "antzwidget.h"
#include "datamapping3dwidget.h"
#include "glyphtreesview.h"
#include "roledatafilterproxymodel.h"

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
    void ExportToANTz();
    void ChangeBaseImage();
	void AddGlyphTemplate();
	void ChangeMapDownloadSettings();
	void ChangeGlyphDefaults();
	void ChangeNewMappingDefaults();
	void OnGlyphTreesViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void CreateMenus();
    void CreateDockWidgets();
	void CreateCenterWidget();
	void LoadDataTransform(const QString& filename);
	bool SaveDataTransform(const QString& filename);
	bool AskUserToSave();
	void EnableProjectDependentActions(bool enable);
	bool ValidateNewDatasource(const QString& datasource);
	bool DoesANTzTemplateExist() const;
	void ProcessCSVFile(const QString& csvFile);
	void ReadNewMappingDefaults();
	void WriteNewMappingDefaults();
	void ClearAndInitializeDataMapping();
	void SelectLastGlyphTreeRoot();

    QMenu* m_fileMenu;
    QMenu* m_glyphMenu;
	QMenu* m_datasourceMenu;
    QMenu* m_viewMenu;
	QMenu* m_toolsMenu;
    
	SynGlyphX::RoleDataFilterProxyModel* m_glyphTreesModel;
	SynGlyphX::RoleDataFilterProxyModel* m_baseObjectsModel;
	QListView* m_baseObjectsView;
	GlyphTreesView* m_glyphTreesView;
	DataSourceStatsWidget* m_dataSourceStats;

	QList<QAction*> m_projectDependentActions;

	DataBindingWidget* m_dataBindingWidget;
	MinMaxGlyphModel* m_minMaxGlyphModel;

	DataTransformModel* m_dataTransformModel;
	
	DataMapping3DWidget* m_minMaxGlyph3DWidget;

	SynGlyphX::DataMappingDefaults m_newMappingDefaults;
};

#endif // DATAMAPPERWINDOW_H
