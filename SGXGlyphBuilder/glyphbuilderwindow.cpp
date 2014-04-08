#include "glyphbuilderwindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QInputDialog>

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

    QAction* saveAction = m_fileMenu->addAction(tr("&Save"));
    QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(SaveGlyph()));

    m_fileMenu->addSeparator();

    QAction* exitAction = m_fileMenu->addAction(tr("E&xit"));
    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    //Create Glyph Menu
    m_glyphMenu = menuBar()->addMenu(tr("Glyph"));

    QAction* newGlyphTreeAction = m_glyphMenu->addAction(tr("Create New Glyph Tree"));
    QObject::connect(newGlyphTreeAction, SIGNAL(triggered()), this, SLOT(CreateNewGlyphTree()));
}

void GlyphBuilderWindow::CreateDockWidgets() {

    //Add Tree View to dock widget on left side
    QDockWidget* leftDockWidget = new QDockWidget("Glyph Tree", this);
    m_treeView = new GlyphTreeView(leftDockWidget);
    leftDockWidget->setWidget(m_treeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
    m_viewMenu->addAction(leftDockWidget->toggleViewAction());
}

void GlyphBuilderWindow::CreateNewGlyphTree() {
    bool createTree = false;

    int numberOfLevels = QInputDialog::getInt(this, "Create Glyph Tree", "Number Of Levels In Glyph Tree", 6, 1, 214783647, 1, &createTree);
    if (createTree) {

    }
}

void GlyphBuilderWindow::SaveGlyph() {

}