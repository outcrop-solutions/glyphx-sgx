#include "glyphdesignerwindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QWizard>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtGui/QCloseEvent>
#include "singleglyphwidget.h"
#include "csvreaderwriter.h"
#include "application.h"
#include "modalglyphwidget.h"

GlyphDesignerWindow::GlyphDesignerWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent),
    m_treeView(NULL)
{
    m_glyphTreeModel = new GlyphTreeModel(this);
    m_selectionModel = new QItemSelectionModel(m_glyphTreeModel, this);

    m_3dView = new ANTzWidget(m_glyphTreeModel, m_selectionModel, this);
    setCentralWidget(m_3dView);

    m_sharedActions = new SharedActionManager(m_glyphTreeModel, m_selectionModel, this);

    CreateMenus();
    CreateDockWidgets();

	QObject::connect(m_glyphTreeModel, &GlyphTreeModel::ModelChanged, this, &GlyphDesignerWindow::setWindowModified);

    //ReadSettings();

    statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

GlyphDesignerWindow::~GlyphDesignerWindow()
{

}

void GlyphDesignerWindow::CreateMenus() {
    
    //Create File Menu
    m_fileMenu = menuBar()->addMenu(tr("File"));

    QAction* openAction = CreateMenuAction(m_fileMenu, tr("Open Template"), QKeySequence::Open);
    QObject::connect(openAction, &QAction::triggered, this, &GlyphDesignerWindow::OpenTemplate);

    QAction* saveAction = CreateMenuAction(m_fileMenu, tr("Save Template"), QKeySequence::Save);
    QObject::connect(saveAction, &QAction::triggered, this, &GlyphDesignerWindow::SaveTemplate);

    QAction* saveAsAction = CreateMenuAction(m_fileMenu, tr("Save As Template"), QKeySequence::SaveAs);
    QObject::connect(saveAsAction, &QAction::triggered, this, &GlyphDesignerWindow::SaveAsTemplate);

    m_fileMenu->addSeparator();

    QAction* exportAction = m_fileMenu->addAction(tr("Export to CSV"));
    QObject::connect(exportAction, &QAction::triggered, this, &GlyphDesignerWindow::ExportToCSV);

    m_fileMenu->addActions(m_recentFileActions);
    
    m_fileMenu->addSeparator();

    QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
    QObject::connect(exitAction, &QAction::triggered, this, &GlyphDesignerWindow::close);

    //Create Edit Menu
    m_editMenu = menuBar()->addMenu(tr("Edit"));
    m_editMenu->addActions(m_sharedActions->GetEditActions());
    m_editMenu->addSeparator();

    QMenu* editingModeMenu = m_editMenu->addMenu(tr("Editing Mode"));

    QActionGroup* editingActionGroup = new QActionGroup(editingModeMenu);
    QAction* moveEditModeAction = new QAction(tr("Move"), editingModeMenu);
    moveEditModeAction->setCheckable(true);
    moveEditModeAction->setData(ANTzWidget::Move);
    editingActionGroup->addAction(moveEditModeAction);
    
    QAction* rotateEditModeAction = new QAction(tr("Rotate"), editingModeMenu);
    rotateEditModeAction->setCheckable(true);
    rotateEditModeAction->setData(ANTzWidget::Rotate);
    editingActionGroup->addAction(rotateEditModeAction);

    QAction* sizeEditModeAction = new QAction(tr("Size"), editingModeMenu);
    sizeEditModeAction->setCheckable(true);
    sizeEditModeAction->setData(ANTzWidget::Size);
    editingActionGroup->addAction(sizeEditModeAction);

    moveEditModeAction->setChecked(true);
    editingModeMenu->addActions(editingActionGroup->actions());
    //QObject::connect(editingActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(EditingModeChanged(QAction*)));
    QObject::connect(editingActionGroup, &QActionGroup::triggered, this, &GlyphDesignerWindow::EditingModeChanged);

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("View"));
    m_fullScreenAction = CreateMenuAction(m_viewMenu, tr("Full Screen"), QKeySequence::FullScreen);
    QObject::connect(m_fullScreenAction, &QAction::triggered, this, &GlyphDesignerWindow::SwitchBetweenFullAndNormalScreen);

    m_viewMenu->addSeparator();

    //Create Glyph Menu
    m_glyphMenu = menuBar()->addMenu(tr("Glyph"));

    QAction* newGlyphTreeAction = m_glyphMenu->addAction(tr("Create New Glyph Tree"));
    QObject::connect(newGlyphTreeAction, &QAction::triggered, this, &GlyphDesignerWindow::CreateNewGlyphTree);

    m_glyphMenu->addSeparator();
    m_glyphMenu->addActions(m_sharedActions->GetGlyphActions());

    m_helpMenu = menuBar()->addMenu(tr("Help"));
    QAction* aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
    QObject::connect(aboutBoxAction, &QAction::triggered, this, &GlyphDesignerWindow::ShowAboutBox);
}

void GlyphDesignerWindow::CreateDockWidgets() {

    //Add Tree View to dock widget on left side
    QDockWidget* leftDockWidget = new QDockWidget("Glyph Tree", this);
    m_treeView = new GlyphTreeView(leftDockWidget);
    m_treeView->setModel(m_glyphTreeModel);
    m_treeView->setSelectionModel(m_selectionModel);
    m_treeView->addActions(m_sharedActions->GetGlyphActions());
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    m_treeView->addAction(separator);
    m_treeView->addActions(m_sharedActions->GetEditActions());
    m_treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    
    leftDockWidget->setWidget(m_treeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
    m_viewMenu->addAction(leftDockWidget->toggleViewAction());

    QDockWidget* rightDockWidget = new QDockWidget("Properties", this);
    ModalGlyphWidget* modalGlyphWidget = new ModalGlyphWidget(m_glyphTreeModel, m_selectionModel, rightDockWidget);
    QObject::connect(modalGlyphWidget, SIGNAL(AddChildrenButtonClicked()), m_sharedActions, SLOT(AddChildren()));
    rightDockWidget->setWidget(modalGlyphWidget);
    addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
    m_viewMenu->addAction(rightDockWidget->toggleViewAction());
    QObject::connect(m_3dView, &ANTzWidget::ObjectEdited, modalGlyphWidget, &ModalGlyphWidget::SetWidgetFromGlyph);
}

void GlyphDesignerWindow::CreateNewGlyphTree() {
    
    if (AskUserToSave()) {

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
                SingleGlyphWidget::ChildOptions childOptions = SingleGlyphWidget::Invisible;
                if (i != numberOfBranches - 1) {
                    childOptions = SingleGlyphWidget::ShowOnBottom | SingleGlyphWidget::EnabledSpinBox;
                }
                SingleGlyphWidget* glyphWidget = new SingleGlyphWidget(childOptions, page);
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

                std::vector<boost::shared_ptr<SynGlyphX::GlyphProperties>> glyphs;
                std::vector<unsigned int> numberOfChildren;

                for (int i = 0; i < numberOfBranches; ++i) {
                    boost::shared_ptr<SynGlyphX::GlyphProperties> glyph(new SynGlyphX::GlyphProperties());
                    glyphWidgets[i]->SetGlyphFromWidget(glyph);
                    glyphs.push_back(glyph);
                    numberOfChildren.push_back(glyphWidgets[i]->GetNumberOfChildren());
                }

                boost::shared_ptr<SynGlyphX::Glyph> newGlyph(new SynGlyphX::Glyph(glyphs, numberOfChildren));

                m_glyphTreeModel->CreateNewTree(newGlyph);
                m_3dView->ResetCamera();
            }
        }
    }
}

void GlyphDesignerWindow::ExportToCSV() {

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

        if (m_glyphTreeModel->SaveToCSV(saveFile.toStdString(), m_selectionModel->selectedIndexes())) {
            statusBar()->showMessage("File successfully saved", 3000);
        }
        else {
            QString title = "Save File Failed";
            QMessageBox::warning(this, title, "Failed to save file");
        }
    }
}

void GlyphDesignerWindow::LoadRecentFile(const QString& filename) {

    if (AskUserToSave()) {
        LoadTemplate(filename);
    }
}

void GlyphDesignerWindow::OpenTemplate() {

    if (AskUserToSave()) {
        QString openFile = QFileDialog::getOpenFileName(this, tr("Open Template"), "", tr("SynGlyphX Glyph Template Files (*.sgt *.csv)"));
        LoadTemplate(openFile);
    }
}

void GlyphDesignerWindow::LoadTemplate(const QString& filename) {

    if (!filename.isEmpty()) {

        SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);
        bool fileLoaded = m_glyphTreeModel->LoadFromFile(filename.toStdString());
        SynGlyphX::Application::restoreOverrideCursor();

        if (fileLoaded) {
            SetCurrentFile(filename);
            statusBar()->showMessage("Template successfully opened", 3000);
            m_3dView->ResetCamera();
        }
        else {
            QString title = "Loading Template Failed";
            QMessageBox::warning(this, title, "Failed to load template");
        }
    }
}

bool GlyphDesignerWindow::SaveTemplate() {

    if (m_currentFilename.isEmpty()) {
        return SaveAsTemplate();
    }
    else {
        return SaveTemplateFile(m_currentFilename);
    }
}

bool GlyphDesignerWindow::SaveAsTemplate() {

    QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Glyph Tree As Template"), "", tr("SynGlyphX Glyph Template Files (*.sgt)"));
    return SaveTemplateFile(saveFile);
}
    
bool GlyphDesignerWindow::SaveTemplateFile(const QString& filename) {

    if (!filename.isEmpty()) {

        SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);
        bool fileSaved = m_glyphTreeModel->SaveToCSV(filename.toStdString(), m_selectionModel->selectedIndexes());
        SynGlyphX::Application::restoreOverrideCursor();

        if (fileSaved) {
            SetCurrentFile(filename);
            statusBar()->showMessage("Template successfully saved", 3000);
            return true;
        }
        else {
            QString title = "Saving Template Failed";
            QMessageBox::warning(this, title, "Failed to save template");
        }
    }

    return false;
}

void GlyphDesignerWindow::ShowAboutBox() {

    QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
    QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
}

void GlyphDesignerWindow::EditingModeChanged(QAction* action) {

    m_3dView->SetEditingMode(static_cast<ANTzWidget::EditingMode>(action->data().toInt()));
}

void GlyphDesignerWindow::closeEvent(QCloseEvent* event) {

    if (AskUserToSave()) {
        SynGlyphX::MainWindow::closeEvent(event);
    }
    else {
        event->ignore();
    }
}

bool GlyphDesignerWindow::AskUserToSave() {

    if (isWindowModified()) {

        QMessageBox::StandardButton result = QMessageBox::warning(this, tr("Save Changes"), tr("The template has been modified.  Do you wish to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (result == QMessageBox::Save) {
            return SaveTemplate();
        }
        else if (result == QMessageBox::Cancel) {
            return false;
        }
    }

    return true;
}

void GlyphDesignerWindow::SwitchBetweenFullAndNormalScreen() {

    if (isFullScreen()) {
        showNormal();
        m_fullScreenAction->setText(tr("Full Screen"));
    }
    else {
        showFullScreen();
        m_fullScreenAction->setText(tr("Return to window"));
    }
}