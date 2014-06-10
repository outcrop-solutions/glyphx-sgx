#include "datamapperwindow.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include "application.h"

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : QMainWindow(parent)
{
    CreateMenus();
}

DataMapperWindow::~DataMapperWindow()
{

}

void DataMapperWindow::CreateMenus() {

    //Create File Menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* newProjectAction = m_fileMenu->addAction(tr("&New Project"));
    QObject::connect(newProjectAction, &QAction::triggered, this, &DataMapperWindow::CreateNewProject);

    m_fileMenu->addSeparator();

    QAction* exitAction = m_fileMenu->addAction(tr("E&xit"));
    QObject::connect(exitAction, &QAction::triggered, this, &DataMapperWindow::close);

    //Create Edit Menu
    m_projectMenu = menuBar()->addMenu(tr("Project"));
    
    QAction* addDataFilesAction = m_projectMenu->addAction(tr("Add Data Files"));
    QObject::connect(addDataFilesAction, &QAction::triggered, this, &DataMapperWindow::AddDataFiles);

    m_projectMenu->addSeparator();

    QAction* exportAction = m_projectMenu->addAction(tr("Export to Glyph Viewer"));
    QObject::connect(exportAction, &QAction::triggered, this, &DataMapperWindow::ExportToGlyphViewer);

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    m_helpMenu = menuBar()->addMenu(tr("Help"));
    QAction* aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
    QObject::connect(aboutBoxAction, SIGNAL(triggered()), this, SLOT(ShowAboutBox()));
}

void DataMapperWindow::ShowAboutBox() {

    QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
    QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
}

void DataMapperWindow::CreateNewProject() {

}

void DataMapperWindow::AddDataFiles() {

}

void DataMapperWindow::ExportToGlyphViewer() {

}