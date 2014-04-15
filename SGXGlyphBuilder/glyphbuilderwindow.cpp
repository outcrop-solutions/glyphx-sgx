#include "glyphbuilderwindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QWizard>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include "singleglyphwidget.h"
#include "csvreaderwriter.h"

GlyphBuilderWindow::GlyphBuilderWindow(QWidget *parent)
    : QMainWindow(parent),
    m_treeView(NULL)
{
    setWindowTitle(tr("SynGlyphX Glyph Builder"));

    CreateMenus();
    CreateDockWidgets();

    m_3dView = new ANTzWidget(this);
    setCentralWidget(m_3dView);
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

    int numberOfBranches = QInputDialog::getInt(this, "Create Glyph Tree", "Number Of Branches In Glyph Tree (Including Root)", 6, 1, 214783647, 1, &createTree);
    if (createTree) {
        QWizard wizard(this);
        wizard.setOptions(QWizard::IndependentPages | QWizard::IgnoreSubTitles | QWizard::NoBackButtonOnStartPage);
        wizard.setWindowTitle(tr("Create Glyph Tree"));

        //Qt will take care of deleting the objects in this vector
        std::vector<SingleGlyphWidget*> glyphWidgets;

        for (int i = 0; i < numberOfBranches; ++i) {
            QWizardPage* page = new QWizardPage(&wizard);
            QVBoxLayout* layout = new QVBoxLayout(this);
            page->setLayout(layout);
            SingleGlyphWidget* glyphWidget = new SingleGlyphWidget((i != numberOfBranches), page);
            glyphWidgets.push_back(glyphWidget);
            if (i == 0) {
                page->setTitle("Glyphs for root level");
                glyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::GetRoot());
            }
            else {
                page->setTitle(QString::number(i).prepend("Glyphs for branch level "));
                glyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::GetTemplate());
            }
            layout->addWidget(glyphWidget);
            wizard.addPage(page);
        }

        if (wizard.exec() == QDialog::Accepted) {
            //This code needs to be updated to handle more than one child
            boost::shared_ptr<SynGlyphX::Glyph> parentGlyph = SynGlyphX::Glyph::GetRoot();
            parentGlyph->ClearChildren();
            glyphWidgets[0]->SetGlyphFromWidget(parentGlyph);
            for (int i = 1; i < numberOfBranches; ++i) {
                boost::shared_ptr<SynGlyphX::Glyph> childGlyph(new SynGlyphX::Glyph());
                glyphWidgets[i]->SetGlyphFromWidget(childGlyph);
                parentGlyph->AddChild(childGlyph);
                parentGlyph = childGlyph;
            }
        }
    }
}

void GlyphBuilderWindow::SaveGlyph() {

    QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Glyph Tree To CSV"), "", tr("CSV Files (*.csv)"));
    if (!saveFile.isEmpty()) {
        SynGlyphX::CSVReaderWriter& writer = SynGlyphX::CSVReaderWriter::GetInstance();
        try {
            writer.Write(saveFile.toStdString(), SynGlyphX::Glyph::GetRoot());
            QMessageBox::information(this, "Save File Succeeded", "CSV file successfully saved");
        }
        catch (const std::exception& e) {
            QString title = "Save File Failed";
            QMessageBox::warning(this, title, title + ": " + e.what());
        }
    }
}