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

    QAction* saveAsProjectAction = CreateMenuAction(m_fileMenu, tr("Save As Project"), QKeySequence::SaveAs);
    QObject::connect(saveAsProjectAction, &QAction::triggered, this, &DataMapperWindow::SaveAsProject);

    m_fileMenu->addSeparator();

    QAction* exportAction = CreateMenuAction(m_fileMenu, tr("Export to Glyph Viewer"));
    QObject::connect(exportAction, &QAction::triggered, this, &DataMapperWindow::ExportToGlyphViewer);

	m_fileMenu->addActions(m_recentFileActions);

    m_fileMenu->addSeparator();

    QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
    QObject::connect(exitAction, &QAction::triggered, this, &DataMapperWindow::close);

    //Create Edit Menu
    m_projectMenu = menuBar()->addMenu(tr("Project"));
    
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

	QDockWidget* bottomDockWidget = new QDockWidget(tr("Data Stats"), this);
	m_dataSourceStats = new DataSourceStatsWidget(bottomDockWidget);

	bottomDockWidget->setWidget(m_dataSourceStats);
	addDockWidget(Qt::BottomDockWidgetArea, bottomDockWidget);
	m_viewMenu->addAction(bottomDockWidget->toggleViewAction());
}

void DataMapperWindow::ShowAboutBox() {

    QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
    QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
}

void DataMapperWindow::CreateNewProject() {

	if (!AskUserToSave()) {
		return;
	}


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

	m_projectDatabase = QSqlDatabase::addDatabase("QSQLITE");
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

	m_dataSourceStats->RebuildStatsViews();

	SetCurrentFile(filename);

	statusBar()->showMessage("Project successfully opened", 3000);
}

bool DataMapperWindow::SaveProjectDatabase(const QString& filename) {

	return false;
}

void DataMapperWindow::AddDataSources() {

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