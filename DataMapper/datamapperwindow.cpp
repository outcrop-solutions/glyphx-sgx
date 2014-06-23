#include "datamapperwindow.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include "application.h"
#include "databaseservices.h"

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent)
{
    CreateMenus();
    CreateDockWidgets();

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

DataMapperWindow::~DataMapperWindow()
{

}

void DataMapperWindow::CreateMenus() {

    //Create File Menu
    m_fileMenu = menuBar()->addMenu(tr("File"));

    QAction* newProjectAction = CreateMenuAction(m_fileMenu, tr("New Project"), QKeySequence::New);
    QObject::connect(newProjectAction, &QAction::triggered, this, &DataMapperWindow::CreateNewProject);

    QAction* openProjectAction = CreateMenuAction(m_fileMenu, tr("Open Project"), QKeySequence::Open);
    QObject::connect(openProjectAction, &QAction::triggered, this, &DataMapperWindow::OpenProject);

    QAction* saveProjectAction = CreateMenuAction(m_fileMenu, tr("Save Project"), QKeySequence::Save);
    QObject::connect(saveProjectAction, &QAction::triggered, this, &DataMapperWindow::SaveProject);
	m_projectDependentActions.push_back(saveProjectAction);

    QAction* saveAsProjectAction = CreateMenuAction(m_fileMenu, tr("Save As Project"), QKeySequence::SaveAs);
    QObject::connect(saveAsProjectAction, &QAction::triggered, this, &DataMapperWindow::SaveAsProject);
	m_projectDependentActions.push_back(saveAsProjectAction);

    m_fileMenu->addSeparator();

    QAction* exportAction = CreateMenuAction(m_fileMenu, tr("Export to Glyph Viewer"));
    QObject::connect(exportAction, &QAction::triggered, this, &DataMapperWindow::ExportToGlyphViewer);
	m_projectDependentActions.push_back(exportAction);

	m_fileMenu->addActions(m_recentFileActions);

    m_fileMenu->addSeparator();

    QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
    QObject::connect(exitAction, &QAction::triggered, this, &DataMapperWindow::close);

    //Create Edit Menu
    m_projectMenu = menuBar()->addMenu(tr("Project"));
    
    QAction* addDataSourcesAction = m_projectMenu->addAction(tr("Add Data Sources"));
    QObject::connect(addDataSourcesAction, &QAction::triggered, this, &DataMapperWindow::AddDataSources);
	m_projectDependentActions.push_back(addDataSourcesAction);

    m_projectMenu->addSeparator();

    QAction* baseImageAction = m_projectMenu->addAction(tr("Choose Base Image"));
    QObject::connect(baseImageAction, &QAction::triggered, this, &DataMapperWindow::ChangeBaseImage);
	m_projectDependentActions.push_back(baseImageAction);

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("View"));
    CreateFullScreenAction(m_viewMenu);

    m_viewMenu->addSeparator();

    m_helpMenu = menuBar()->addMenu(tr("Help"));
    QAction* aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
    QObject::connect(aboutBoxAction, SIGNAL(triggered()), this, SLOT(ShowAboutBox()));

	EnableProjectDependentActions(false);
}

void DataMapperWindow::CreateDockWidgets() {

    //Add Tree View to dock widget on left side
    QDockWidget* leftDockWidget = new QDockWidget(tr("Glyph Tree"), this);
    m_glyphTreeView = new QTreeView(leftDockWidget);
    //m_treeView->setModel(m_glyphTreeModel);
    //m_treeView->setSelectionModel(m_selectionModel);
    m_glyphTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

    leftDockWidget->setWidget(m_glyphTreeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
    m_viewMenu->addAction(leftDockWidget->toggleViewAction());

	QDockWidget* rightDockWidget = new QDockWidget(tr("Data Stats"), this);
	m_dataSourceStats = new DataSourceStatsWidget(rightDockWidget);

	rightDockWidget->setWidget(m_dataSourceStats);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
	m_viewMenu->addAction(rightDockWidget->toggleViewAction());
}

void DataMapperWindow::ShowAboutBox() {

    QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
    QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
}

void DataMapperWindow::CreateNewProject() {

	if (!AskUserToSave()) {
		return;
	}

	EnableProjectDependentActions(true);
}

void DataMapperWindow::OpenProject() {

	if (!AskUserToSave()) {
		return;
	}

	QString openFile = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("SynGlyphX Data Mapper Project Files (*.sdt)"));
	LoadProjectDatabase(openFile);
}

bool DataMapperWindow::SaveProject() {

	if (m_currentFilename.isEmpty()) {
		return SaveAsProject();
	}
	else {
		return SaveProjectDatabase(m_currentFilename);
	}
}

bool DataMapperWindow::SaveAsProject() {

	QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("SynGlyphX Data Mapper Project Files (*.sdt)"));
	return SaveProjectDatabase(saveFile);
}

void DataMapperWindow::LoadRecentFile(const QString& filename) {

	if (AskUserToSave()) {
		LoadProjectDatabase(filename);
	}
}

void DataMapperWindow::LoadProjectDatabase(const QString& filename) {

	m_projectDatabase = QSqlDatabase::addDatabase("QSQLITE", DatabaseServices::GetProjectDBConnectionName());
	m_projectDatabase.setDatabaseName(filename);
	if (!m_projectDatabase.open()) {

		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + m_projectDatabase.lastError().text(), QMessageBox::Ok);
		return;
	}

	QStringList tables = m_projectDatabase.tables();

	if (!tables.contains("DataSources")) {

		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: Incorrect format for project"), QMessageBox::Ok);
		m_projectDatabase.close();
		return;
	}

	try {
		m_dataSourceStats->RebuildStatsViews();
	}
	catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	SetCurrentFile(filename);

	EnableProjectDependentActions(true);

	statusBar()->showMessage("Project successfully opened", 3000);
}

bool DataMapperWindow::SaveProjectDatabase(const QString& filename) {

	return false;
}

void DataMapperWindow::AddDataSources() {

	QString dataSource = QFileDialog::getOpenFileName(this, tr("Add Data Source"), "", "SQLite databases (*.*)");
	if (!DatabaseServices::IsSQLiteDB(dataSource)) {
		QMessageBox::critical(this, tr("Failed To Add Data Source"), tr("Data source was not added because it was not a recognized type"), QMessageBox::Ok);
		return;
	}


}

void DataMapperWindow::ExportToGlyphViewer() {

}

void DataMapperWindow::ChangeBaseImage() {

}

bool DataMapperWindow::AskUserToSave() {

	if (isWindowModified()) {

		QMessageBox::StandardButton result = QMessageBox::warning(this, tr("Save Changes"), tr("The project has been modified.  Do you wish to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (result == QMessageBox::Save) {
			return SaveProject();
		}
		else if (result == QMessageBox::Cancel) {
			return false;
		}
	}

	return true;
}

void DataMapperWindow::closeEvent(QCloseEvent* event) {

	if (AskUserToSave()) {
		SynGlyphX::MainWindow::closeEvent(event);
	}
	else {
		event->ignore();
	}
}

void DataMapperWindow::EnableProjectDependentActions(bool enable) {

	for (int i = 0; i < m_projectDependentActions.length(); ++i) {
		m_projectDependentActions[i]->setEnabled(enable);
	}
}