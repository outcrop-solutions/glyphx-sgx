#include "datamapperwindow.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include "application.h"

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent)
{
    CreateMenus();
    CreateDockWidgets();
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

    m_fileMenu->addActions(m_recentFileActions);

    m_fileMenu->addSeparator();

    QAction* exportAction = CreateMenuAction(m_fileMenu, tr("Export to Glyph Viewer"));
    QObject::connect(exportAction, &QAction::triggered, this, &DataMapperWindow::ExportToGlyphViewer);

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
    QDockWidget* leftDockWidget = new QDockWidget("Glyph Tree", this);
    m_glyphTreeView = new QTreeView(leftDockWidget);
    //m_treeView->setModel(m_glyphTreeModel);
    //m_treeView->setSelectionModel(m_selectionModel);
    m_glyphTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

    leftDockWidget->setWidget(m_glyphTreeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
    m_viewMenu->addAction(leftDockWidget->toggleViewAction());
}

void DataMapperWindow::ShowAboutBox() {

    QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
    QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
}

void DataMapperWindow::CreateNewProject() {

}

void DataMapperWindow::OpenProject() {

}

void DataMapperWindow::SaveProject() {

}

void DataMapperWindow::SaveAsProject() {

}

void DataMapperWindow::LoadRecentFile(const QString& filename) {

}

void DataMapperWindow::AddDataSources() {

}

void DataMapperWindow::ExportToGlyphViewer() {

}

void DataMapperWindow::ChangeBaseImage() {

}