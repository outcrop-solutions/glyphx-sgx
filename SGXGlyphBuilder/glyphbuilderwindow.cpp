#include "glyphbuilderwindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QWizard>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include "singleglyphwidget.h"
#include "csvreaderwriter.h"

GlyphBuilderWindow::GlyphBuilderWindow(QWidget *parent)
    : QMainWindow(parent),
    m_treeView(NULL)
{
    setWindowTitle(tr("SynGlyphX Glyph Builder"));

    m_glyphTreeModel = new GlyphTreeModel(this);

    m_3dView = new ANTzWidget(m_glyphTreeModel, this);
    setCentralWidget(m_3dView);

    CreateMenus();
    CreateDockWidgets();

    statusBar()->showMessage("Glyph Builder Started", 3000);
}

GlyphBuilderWindow::~GlyphBuilderWindow()
{

}

void GlyphBuilderWindow::CreateMenus() {
    
    //Create File Menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* openAction = m_fileMenu->addAction(tr("&Open Template"));
    QObject::connect(openAction, SIGNAL(triggered()), this, SLOT(OpenTemplate()));

    QAction* saveAction = m_fileMenu->addAction(tr("&Save As Template"));
    QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(SaveAsTemplate()));

    m_fileMenu->addSeparator();

    QAction* exportAction = m_fileMenu->addAction(tr("Export to CSV"));
    QObject::connect(exportAction, SIGNAL(triggered()), this, SLOT(SaveGlyph()));

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
    m_treeView->setModel(m_glyphTreeModel);

    QObject::connect(m_treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), m_3dView, SLOT(UpdateSelection(const QItemSelection&, const QItemSelection&)));
    
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
            m_glyphTreeModel->CreateFromTemplates(SynGlyphX::Glyph::GetRoot());
            m_3dView->ResetCamera();
        }
    }
}

void GlyphBuilderWindow::SaveGlyph() {

    QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Glyph Tree To CSV"), "", tr("CSV Files (*.csv)"));
    if (!saveFile.isEmpty()) {
        /*SynGlyphX::CSVReaderWriter& writer = SynGlyphX::CSVReaderWriter::GetInstance();
        try {
            writer.Write(saveFile.toStdString(), SynGlyphX::Glyph::GetRoot());
            QMessageBox::information(this, "Save File Succeeded", "CSV file successfully saved");
        }
        catch (const std::exception& e) {
            QString title = "Save File Failed";
            QMessageBox::warning(this, title, title + ": " + e.what());
        }*/

        if (m_glyphTreeModel->SaveToCSV(saveFile.toStdString())) {
            statusBar()->showMessage("File successfully saved", 3000);
        }
        else {
            QString title = "Save File Failed";
            QMessageBox::warning(this, title, "Failed to save file");
        }
    }
}

void GlyphBuilderWindow::OpenTemplate() {

    QString openFile = QFileDialog::getOpenFileName(this, tr("Open Template"), "", tr("SynGlyphX Glyph Template Files (*.sgt)"));
    if (!openFile.isEmpty()) {
        if (m_glyphTreeModel->LoadFromFile(openFile.toStdString())) {
            statusBar()->showMessage("Template successfully opened", 3000);
            m_3dView->ResetCamera();
        }
        else {
            QString title = "Loading Template Failed";
            QMessageBox::warning(this, title, "Failed to load template");
        }
    }
}

void GlyphBuilderWindow::SaveAsTemplate() {
    
    QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Glyph Tree As Template"), "", tr("SynGlyphX Glyph Template Files (*.sgt)"));
    if (!saveFile.isEmpty()) {

        if (m_glyphTreeModel->SaveToCSV(saveFile.toStdString())) {
            statusBar()->showMessage("Template successfully saved", 3000);
        }
        else {
            QString title = "Saving Template Failed";
            QMessageBox::warning(this, title, "Failed to save template");
        }
    }
}