#include "datamapperwindow.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include "application.h"
#include "databaseservices.h"

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent)
{
	m_transform.reset(new SynGlyphX::DataTransform());
    CreateMenus();
    CreateDockWidgets();

	m_dataBindingWidget = new DataBindingWidget(this);
	setCentralWidget(m_dataBindingWidget);

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

	QAction* addGlyphTemplateAction = m_projectMenu->addAction(tr("Add Glyph Template"));
	QObject::connect(addGlyphTemplateAction, &QAction::triggered, this, &DataMapperWindow::AddGlyphTemplate);
    
    QAction* addDataSourcesAction = m_projectMenu->addAction(tr("Add Data Sources"));
    QObject::connect(addDataSourcesAction, &QAction::triggered, this, &DataMapperWindow::AddDataSources);

    m_projectMenu->addSeparator();

    QAction* baseImageAction = m_projectMenu->addAction(tr("Choose Base Image"));
    QObject::connect(baseImageAction, &QAction::triggered, this, &DataMapperWindow::ChangeBaseImage);

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
    m_dataSourceStats = new DataSourceStatsWidget(m_transform, rightDockWidget);

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

	QString openFile = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("SynGlyphX Data Transform Project Files (*.sdt)"));
	LoadDataTransform(openFile);
}

bool DataMapperWindow::SaveProject() {

	if (m_currentFilename.isEmpty()) {
		return SaveAsProject();
	}
	else {
		return SaveDataTransform(m_currentFilename);
	}
}

bool DataMapperWindow::SaveAsProject() {

	QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("SynGlyphX Data Transform Project Files (*.sdt)"));
	return SaveDataTransform(saveFile);
}

void DataMapperWindow::LoadRecentFile(const QString& filename) {

	if (AskUserToSave()) {
		LoadDataTransform(filename);
	}
}

void DataMapperWindow::LoadDataTransform(const QString& filename) {

	try {
		m_transform->ReadFromFile(filename.toStdString());

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

bool DataMapperWindow::SaveDataTransform(const QString& filename) {

	m_transform->WriteToFile(filename.toStdString());
	statusBar()->showMessage("Data transform successfully saved", 3000);
	return true;
}

bool DataMapperWindow::ValidateNewDatasource(const QString& datasource) {

	if (DatabaseServices::IsSQLiteDB(datasource)) {
		return true;
	}

	return false;
}

void DataMapperWindow::AddDataSources() {

	QStringList dataSources = QFileDialog::getOpenFileNames(this, tr("Add Data Source"), "", "SQLite databases (*.*)");

	if (dataSources.isEmpty()) {
		return;
	}

	unsigned int numNewDatasources = 0;
	for (const QString& datasource : dataSources) {

		if (!ValidateNewDatasource(datasource)) {

			QMessageBox::critical(this, tr("Failed To Add Data Source"), datasource + tr(" is not a recognized type"), QMessageBox::Ok);
			continue;
		}
			
		boost::uuids::uuid newDBID = m_transform->AddDatasource(datasource.toStdWString(), SynGlyphX::Datasource::SQLITE3);
		std::vector<std::wstring> tables;
		QSqlDatabase db = QSqlDatabase::addDatabase(DatabaseServices::GetQtDBType(SynGlyphX::Datasource::SQLITE3), QString::fromStdString(boost::uuids::to_string(newDBID)));
		db.setDatabaseName(datasource);

		try {
			if (!db.open()) {

				throw std::exception((datasource + tr(" could not be opened")).toStdString().c_str());
			}
			QStringList qtables = db.tables();
			DatabaseServices::RemoveAutoGeneratedTableNamesFromList(qtables);

			if (!qtables.isEmpty()) {

				for (const QString& qtable : qtables) {
					tables.push_back(qtable.toStdWString());
				}

				m_transform->AddTables(newDBID, tables);
			}
			else {

				throw std::exception((tr("No tables in ") + datasource).toStdString().c_str());
			}

			++numNewDatasources;
		}
		catch (const std::exception& e) {

			QSqlDatabase::removeDatabase(QString::fromStdString(boost::uuids::to_string(newDBID)));
			QMessageBox::critical(this, tr("Failed To Add Data Source"), datasource + tr(" could not be opened"), QMessageBox::Ok);
			continue;
		}
	}

	if (numNewDatasources > 0) {

		m_dataSourceStats->AddNewStatsViews(numNewDatasources);
	}
    
	EnableProjectDependentActions(true);
}

void DataMapperWindow::ExportToGlyphViewer() {

}

void DataMapperWindow::ChangeBaseImage() {

	EnableProjectDependentActions(true);
}

void DataMapperWindow::AddGlyphTemplate() {

	QStringList glyphTemplates = QFileDialog::getOpenFileNames(this, tr("Add Glyph Templates"), "", "SynGlyphX Glyph Template Files (*.sgt)");

	if (glyphTemplates.isEmpty()) {
		return;
	}

	EnableProjectDependentActions(true);
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