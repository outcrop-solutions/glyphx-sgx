#include "glyphbuilderwindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QWizard>
#include <QtWidgets/QVBoxLayout>
#include "singleglyphwidget.h"

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

    int numberOfBranches = QInputDialog::getInt(this, "Create Glyph Tree", "Number Of Branches In Glyph Tree", 6, 1, 214783647, 1, &createTree);
    if (createTree) {
        QWizard wizard(this);
        wizard.setOptions(QWizard::IndependentPages | QWizard::IgnoreSubTitles | QWizard::NoBackButtonOnStartPage);
        wizard.setWindowTitle(tr("Create Glyph Tree"));

        for (int i = 1; i <= numberOfBranches; ++i) {
            QWizardPage* page = new QWizardPage(&wizard);
            QVBoxLayout* layout = new QVBoxLayout(this);
            page->setLayout(layout);
            page->setTitle(QString::number(i).prepend("Glyphs for branch level "));
            SingleGlyphWidget* glyphWidget = new SingleGlyphWidget((i != numberOfBranches), page);
            layout->addWidget(glyphWidget);
            wizard.addPage(page);
        }

        if (wizard.exec() == QDialog::Accepted) {

        }
    }
}

void GlyphBuilderWindow::SaveGlyph() {

}