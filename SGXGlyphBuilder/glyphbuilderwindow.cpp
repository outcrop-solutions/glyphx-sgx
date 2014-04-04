#include "glyphbuilderwindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>

GlyphBuilderWindow::GlyphBuilderWindow(QWidget *parent)
    : QMainWindow(parent),
    m_treeView(NULL)
{
    setWindowTitle("SynGlyphX Glyph Builder");

    CreateMenus();
    CreateDockWidgets();
}

GlyphBuilderWindow::~GlyphBuilderWindow()
{

}

void GlyphBuilderWindow::CreateMenus() {
    
    //Create File Menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    QAction* newGlyphAction = m_fileMenu->addAction(tr("Create New Glyph"));
    QObject::connect(newGlyphAction, SIGNAL(triggered()), this, SLOT(CreateNewGlyph()));

    m_fileMenu->addSeparator();

    QAction* saveAction = m_fileMenu->addAction(tr("&Save"));
    QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(SaveGlyph()));

    m_fileMenu->addSeparator();

    QAction* exitAction = m_fileMenu->addAction(tr("E&xit"));
    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));
}

void GlyphBuilderWindow::CreateDockWidgets() {

    //Add Tree View to dock widget on left side
    QDockWidget* leftDockWidget = new QDockWidget("Glyph Tree", this);
    m_treeView = new GlyphTreeView(leftDockWidget);
    leftDockWidget->setWidget(m_treeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
    m_viewMenu->addAction(leftDockWidget->toggleViewAction());
}

void GlyphBuilderWindow::CreateNewGlyph() {

}

void GlyphBuilderWindow::SaveGlyph() {

}