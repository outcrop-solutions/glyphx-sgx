#include "glyphdesignerwindow.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtGui/QCloseEvent>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include "glyphpropertieswidget.h"
#include "antzcsvwriter.h"
#include "application.h"
#include "modalglyphwidget.h"
#include "newglyphtreewizard.h"

GlyphDesignerWindow::GlyphDesignerWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent),
    m_treeView(nullptr),
	m_glyphTreeModel(nullptr),
	m_3dView(nullptr),
	m_isFileLoadingOrDefaultGlyphSet(false)
{
	m_glyphTreeModel = new SynGlyphXANTz::MinMaxGlyphTreeModel(this);
	m_glyphTreeModel->RepaceModelWithDefaultGlyphTree();
	m_sharedSelectionModel = new QItemSelectionModel(m_glyphTreeModel, this);

	m_3dView = new SynGlyphXANTz::ANTzSingleGlyphTreeWidget(SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType::Max, this);
	m_3dView->SetAllowMultiSelection(true);
	m_3dView->SetModel(m_glyphTreeModel, m_sharedSelectionModel);
    setCentralWidget(m_3dView);

	CreateMenus();
	CreateDockWidgets();

	m_3dView->addActions(m_treeView->GetGlyphActions());
	m_3dView->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	m_3dView->addActions(m_treeView->GetEditActions());
	m_3dView->setContextMenuPolicy(Qt::ActionsContextMenu);

	QObject::connect(m_glyphTreeModel, &SynGlyphXANTz::MinMaxGlyphTreeModel::modelReset, this, &GlyphDesignerWindow::OnModelChanged);
	QObject::connect(m_glyphTreeModel, &SynGlyphXANTz::MinMaxGlyphTreeModel::rowsInserted, this, &GlyphDesignerWindow::OnModelChanged);
	QObject::connect(m_glyphTreeModel, &SynGlyphXANTz::MinMaxGlyphTreeModel::rowsRemoved, this, &GlyphDesignerWindow::OnModelChanged);
	QObject::connect(m_glyphTreeModel, &SynGlyphXANTz::MinMaxGlyphTreeModel::rowsMoved, this, &GlyphDesignerWindow::OnModelChanged);
	QObject::connect(m_glyphTreeModel, &SynGlyphXANTz::MinMaxGlyphTreeModel::dataChanged, this, &GlyphDesignerWindow::OnModelChanged);
	
	SelectRootGlyphInModel();

	QStringList commandLineArguments = SynGlyphX::Application::arguments();
	if (commandLineArguments.size() > 1) {

		QDir templateToLoad(commandLineArguments[1]);
		LoadTemplate(QDir::toNativeSeparators(templateToLoad.canonicalPath()));
	}

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

	QAction* exportDataMapperAction = m_fileMenu->addAction(tr("Export to CSV"));
	QObject::connect(exportDataMapperAction, &QAction::triggered, this, &GlyphDesignerWindow::ExportToCSV);

    //QAction* exportAction = m_fileMenu->addAction(tr("Export to CSV"));
    //QObject::connect(exportAction, &QAction::triggered, this, &GlyphDesignerWindow::ExportToCSV);

    m_fileMenu->addActions(m_recentFileActions);
    
    m_fileMenu->addSeparator();

    QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
    QObject::connect(exitAction, &QAction::triggered, this, &GlyphDesignerWindow::close);

    //Create Edit Menu
    m_editMenu = menuBar()->addMenu(tr("Edit"));

    QMenu* editingModeMenu = m_editMenu->addMenu(tr("Editing Mode"));

    QActionGroup* editingActionGroup = new QActionGroup(editingModeMenu);
    QAction* moveEditModeAction = new QAction(tr("Move"), editingModeMenu);
    moveEditModeAction->setCheckable(true);
	moveEditModeAction->setData(SynGlyphXANTz::ANTzSingleGlyphTreeWidget::Move);
    editingActionGroup->addAction(moveEditModeAction);
    
    QAction* rotateEditModeAction = new QAction(tr("Rotate"), editingModeMenu);
    rotateEditModeAction->setCheckable(true);
	rotateEditModeAction->setData(SynGlyphXANTz::ANTzSingleGlyphTreeWidget::Rotate);
    editingActionGroup->addAction(rotateEditModeAction);

    QAction* sizeEditModeAction = new QAction(tr("Size"), editingModeMenu);
    sizeEditModeAction->setCheckable(true);
	sizeEditModeAction->setData(SynGlyphXANTz::ANTzSingleGlyphTreeWidget::Size);
    editingActionGroup->addAction(sizeEditModeAction);

    moveEditModeAction->setChecked(true);
    editingModeMenu->addActions(editingActionGroup->actions());
    //QObject::connect(editingActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(EditingModeChanged(QAction*)));
    QObject::connect(editingActionGroup, &QActionGroup::triggered, this, &GlyphDesignerWindow::EditingModeChanged);

	m_editMenu->addSeparator();

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("View"));

	m_showAnimation = m_viewMenu->addAction(tr("Show Animation"));
	m_showAnimation->setCheckable(true);
	m_showAnimation->setChecked(true);
	QObject::connect(m_showAnimation, &QAction::toggled, m_3dView, &SynGlyphXANTz::ANTzSingleGlyphTreeWidget::EnableAnimation);

	m_viewMenu->addSeparator();

    CreateFullScreenAction(m_viewMenu);

    m_viewMenu->addSeparator();

    //Create Glyph Menu
    m_glyphMenu = menuBar()->addMenu(tr("Glyph"));

    QAction* newGlyphTreeAction = m_glyphMenu->addAction(tr("Create New Glyph Tree"));
    QObject::connect(newGlyphTreeAction, &QAction::triggered, this, &GlyphDesignerWindow::CreateNewGlyphTree);

    m_glyphMenu->addSeparator();

    CreateHelpMenu();
}

void GlyphDesignerWindow::CreateDockWidgets() {

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    //Add Tree View to dock widget on left side
    QDockWidget* leftDockWidget = new QDockWidget("Glyph Tree", this);
	m_treeView = new GlyphTreeView(m_glyphTreeModel, SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType::Max, leftDockWidget);
	m_treeView->setSelectionModel(m_sharedSelectionModel);
	m_editMenu->addActions(m_treeView->GetEditActions());
	m_glyphMenu->addActions(m_treeView->GetGlyphActions());
    
    leftDockWidget->setWidget(m_treeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
    m_viewMenu->addAction(leftDockWidget->toggleViewAction());

    QDockWidget* rightDockWidget = new QDockWidget("Properties", this);
	ModalGlyphWidget* modalGlyphWidget = new ModalGlyphWidget(SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType::Max, rightDockWidget);
	modalGlyphWidget->SetModel(m_glyphTreeModel, m_sharedSelectionModel);
	QObject::connect(modalGlyphWidget, &ModalGlyphWidget::AddChildrenButtonClicked, m_treeView, &GlyphTreeView::AddChildren);
    rightDockWidget->setWidget(modalGlyphWidget);
    addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
    m_viewMenu->addAction(rightDockWidget->toggleViewAction());
}

void GlyphDesignerWindow::CreateNewGlyphTree() {
    
    if (AskUserToSave()) {

		SynGlyphX::DataMappingGlyphGraph::SharedPtr newGlyphTree = SynGlyphX::NewGlyphTreeWizard::RunNewGlyphTreeWizard(this);
		if (newGlyphTree) {

			m_glyphTreeModel->SetMinMaxGlyphTree(newGlyphTree);
			SelectRootGlyphInModel();

		}
    }
}

void GlyphDesignerWindow::ExportToCSV() {

    QString saveFile = GetFileNameSaveDialog("SaveDir", tr("Save Glyph Tree To CSV"), "", tr("CSV Files (*.csv)"));
    if (!saveFile.isEmpty()) {

        if (m_glyphTreeModel->SaveToCSV(saveFile)) {

            statusBar()->showMessage("File successfully saved", 3000);
        }
        else {

            QString title = "Save File Failed";
            QMessageBox::warning(this, title, "Failed to save file");
        }
    }
}

bool GlyphDesignerWindow::LoadRecentFile(const QString& filename) {

    if (AskUserToSave()) {
        
		return LoadTemplate(filename);
    }

	return true;
}

void GlyphDesignerWindow::OpenTemplate() {

    if (AskUserToSave()) {
        
		QString openFile = GetFileNameOpenDialog("GlyphTemplateDir", tr("Open Template"), m_glyphTemplatesDirectory, tr("SynGlyphX Glyph Template Files (*.sgt *.csv)"));
		LoadTemplate(openFile);
    }
}

bool GlyphDesignerWindow::LoadTemplate(const QString& filename) {

	if (filename.isEmpty()) {

		return false;
	}

	QFileInfo fileInfo(filename);
	QString extension = fileInfo.suffix().toLower();
	if ((extension != "csv") && (extension != "sgt")) {

		QMessageBox::warning(this, tr("Loading Template Failed"), tr("File is not a recognized format"));
		return false;
	}

	if (!fileInfo.exists()) {

		QMessageBox::warning(this, tr("Loading Template Failed"), tr("File does not exist"));
		return false;
	}

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	m_isFileLoadingOrDefaultGlyphSet = true;
	bool fileLoaded = m_glyphTreeModel->LoadFromFile(filename);
	SynGlyphX::Application::restoreOverrideCursor();

	if (fileLoaded) {

		SetCurrentFile(filename);
		statusBar()->showMessage("Template successfully opened", 3000);
		SelectRootGlyphInModel();
	}
	else {

		m_isFileLoadingOrDefaultGlyphSet = false;
		QMessageBox::warning(this, tr("Loading Template Failed"), tr("Failed to load template"));
	}

	return fileLoaded;
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

	QString saveFile = GetFileNameSaveDialog("SaveDir", tr("Export to Data Mapper"), "", tr("SynGlyphX Glyph Template Files (*.sgt)"));
    return SaveTemplateFile(saveFile);
}
    
bool GlyphDesignerWindow::SaveTemplateFile(const QString& filename) {
	
    if (!filename.isEmpty()) {

		try {

			SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
			m_glyphTreeModel->SaveToTemplateFile(filename);
			SynGlyphX::Application::restoreOverrideCursor();

            SetCurrentFile(filename);
            statusBar()->showMessage("Template successfully saved", 3000);
            return true;
        }
        catch(const std::exception& e) {
            
			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::warning(this, tr("Saving Template Failed"), tr("Failed to save template: ") + e.what());
        }
    }

    return false;
}

void GlyphDesignerWindow::EditingModeChanged(QAction* action) {

	m_3dView->SetEditingMode(static_cast<SynGlyphXANTz::ANTzSingleGlyphTreeWidget::EditingMode>(action->data().toInt()));
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

void GlyphDesignerWindow::OnModelChanged() {

	if (m_isFileLoadingOrDefaultGlyphSet) {

		setWindowModified(false);
		m_isFileLoadingOrDefaultGlyphSet = false;
	}
	else {

		setWindowModified(true);
	}
}

void GlyphDesignerWindow::SelectRootGlyphInModel() {

	m_sharedSelectionModel->select(m_glyphTreeModel->index(0), QItemSelectionModel::ClearAndSelect);
}

void GlyphDesignerWindow::ReadSettings(){

	SynGlyphX::MainWindow::ReadSettings();

	QSettings settings;
	settings.beginGroup("Display");
	if (!settings.value("show", true).toBool()) {

		m_showAnimation->toggle();
	}
	settings.endGroup();
}

void GlyphDesignerWindow::WriteSettings() {

	SynGlyphX::MainWindow::WriteSettings();
	QSettings settings;
	settings.beginGroup("Display");
	settings.setValue("show", m_showAnimation->isChecked());
	settings.endGroup();
}