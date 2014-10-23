#include "glyphviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDockWidget>
#include "application.h"
#include "datatransformmapping.h"
#include "downloadoptionsdialog.h"
#include "data/npmapfile.h"
#include "sourcedatamanager.h"
#include "glyphviewerantztransformer.h"
#include "changedatasourcefiledialog.h"
#include "antzimportdialog.h"
#include "antzvisualizationfilelisting.h"

GlyphViewerWindow::GlyphViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(parent)
{
	m_glyphForestModel = new GlyphForestModel(this);
	QString cacheDir = QDir::toNativeSeparators(QDir::currentPath()) + QDir::separator() + "cache";
	m_cacheManager.SetBaseCacheDirectory(cacheDir.toStdWString());
	CreateMenus();
	CreateDockWidgets();
	
	try {
		m_antzWidget = new ANTzViewerWidget(m_glyphForestModel, m_treeView->selectionModel(), this);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("3D view error"), tr("3D view failed to create: ") + e.what());
		throw;
	}

	setCentralWidget(m_antzWidget);

	QObject::connect(m_antzWidget, &ANTzViewerWidget::NewStatusMessage, statusBar(), &QStatusBar::showMessage);

	m_stereoAction->setChecked(m_antzWidget->IsInStereoMode());

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

GlyphViewerWindow::~GlyphViewerWindow()
{

}

void GlyphViewerWindow::CreateMenus() {

	//Create File Menu
	m_fileMenu = menuBar()->addMenu(tr("File"));

	QAction* openProjectAction = CreateMenuAction(m_fileMenu, tr("Open Visualization"), QKeySequence::Open);
	QObject::connect(openProjectAction, &QAction::triggered, this, &GlyphViewerWindow::OpenProject);

	m_fileMenu->addSeparator();

	QAction* refreshVisualizationAction = CreateMenuAction(m_fileMenu, tr("Refresh Visualization"), QKeySequence::Refresh);
	QObject::connect(refreshVisualizationAction, &QAction::triggered, this, &GlyphViewerWindow::RefreshVisualization);
	m_loadedVisualizationDependentActions.push_back(refreshVisualizationAction);

	m_fileMenu->addSeparator();

	//Import from ANTz is on hold so until more development is done, the menu option will not be added to the File menu

	//QAction* importFromANTzAction = m_fileMenu->addAction(tr("Import From ANTz"));
	//QObject::connect(importFromANTzAction, &QAction::triggered, this, &GlyphViewerWindow::ImportFilesFromANTz);

	//m_fileMenu->addSeparator();

	QAction* closeVisualizationAction = CreateMenuAction(m_fileMenu, tr("Close Visualization"), QKeySequence::Close);
	QObject::connect(closeVisualizationAction, &QAction::triggered, this, &GlyphViewerWindow::CloseVisualization);
	m_loadedVisualizationDependentActions.push_back(closeVisualizationAction);

	m_fileMenu->addActions(m_recentFileActions);

	m_fileMenu->addSeparator();

	QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
	QObject::connect(exitAction, &QAction::triggered, this, &GlyphViewerWindow::close);

	//Create View Menu
	m_viewMenu = menuBar()->addMenu(tr("View"));

	m_stereoAction = CreateMenuAction(m_viewMenu, tr("Stereo"));
	m_stereoAction->setCheckable(true);
	QObject::connect(m_stereoAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeStereoMode);

	m_viewMenu->addSeparator();
	CreateFullScreenAction(m_viewMenu);

	m_viewMenu->addSeparator();

	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeMapDownloadSettings);

	m_helpMenu = menuBar()->addMenu(tr("Help"));

	QAction* openSettingsAction = m_helpMenu->addAction(tr("OpenGL Settings"));
	QObject::connect(openSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ShowOpenGLSettings);

	m_helpMenu->addSeparator();

	QAction* aboutBoxAction = m_helpMenu->addAction(tr("About ") + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
	QObject::connect(aboutBoxAction, &QAction::triggered, this, &GlyphViewerWindow::ShowAboutBox);

	EnableLoadedVisualizationDependentActions(false);
}

void GlyphViewerWindow::CreateDockWidgets() {

	//Add Tree View to dock widget on left side
	QDockWidget* leftDockWidget = new QDockWidget(tr("Glyph List"), this);
	m_treeView = new GlyphTreeListView(leftDockWidget);
	m_treeView->setModel(m_glyphForestModel);

	leftDockWidget->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
	m_viewMenu->addAction(leftDockWidget->toggleViewAction());
}

void GlyphViewerWindow::ShowAboutBox() {

	QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
	QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
}

void GlyphViewerWindow::OpenProject() {

	QString openFile = QFileDialog::getOpenFileName(this, tr("Open Visualization"), "", tr("SynGlyphX Visualization Files (*.sdt *.sav);;SynGlyphX Data Transform Files (*.sdt);;SynGlyphX ANTz Visualization Files (*.sav)"));
	if (!openFile.isEmpty()) {

		LoadNewVisualization(openFile);
	}
}

void GlyphViewerWindow::RefreshVisualization() {

	try {

		LoadVisualization(m_currentFilename);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Refresh Visualization"), tr("Failed to refresh visualization.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	statusBar()->showMessage("Visualization successfully refreshed", 3000);
}

void GlyphViewerWindow::CloseVisualization() {

	QStringList empty;
	m_glyphForestModel->LoadANTzFiles(empty);
	m_glyphForestModel->UseDefaultBaseImage();
	EnableLoadedVisualizationDependentActions(false);
	ClearCurrentFile();
}

void GlyphViewerWindow::LoadVisualization(const QString& filename) {

	if (!m_treeView->selectionModel()->selectedIndexes().empty()) {

		m_treeView->selectionModel()->clearSelection();
		m_antzWidget->updateGL();
	}

	QString extension = filename.right(4).toLower();
	if (extension == ".sdt") {

		LoadDataTransform(filename);
	}
	else if (extension == ".sav") {

		LoadANTzCompatibilityVisualization(filename);
	}
	else {

		throw std::exception("File is not a visualization.");
	}
}

void GlyphViewerWindow::LoadNewVisualization(const QString& filename) {

	if (filename == m_currentFilename) {
		return;
	}

	try {

		LoadVisualization(filename);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Open Visualization"), tr("Failed to open visualization.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	SetCurrentFile(filename);
	EnableLoadedVisualizationDependentActions(true);
	statusBar()->showMessage("Visualization successfully opened", 3000);
}

void GlyphViewerWindow::LoadANTzCompatibilityVisualization(const QString& filename) {

	SynGlyphX::ANTzVisualizationFileListing fileListing;
	fileListing.ReadFromFile(filename.toStdString());
	QStringList csvFiles;


	LoadFilesIntoModel(csvFiles, QString::fromStdWString(fileListing.GetBaseImageFilename()));
}

void GlyphViewerWindow::LoadRecentFile(const QString& filename) {

	LoadNewVisualization(filename);
}

void GlyphViewerWindow::LoadDataTransform(const QString& filename) {

	SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);

	try {

		SynGlyphX::DataTransformMapping mapping;
		mapping.ReadFromFile(filename.toStdString());

		bool wereDatasourcesUpdated = false;

		std::vector<SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator> fileDatasourcesToBeUpdated;
		for (SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator datasource = mapping.GetDatasources().GetFileDatasources().begin(); datasource != mapping.GetDatasources().GetFileDatasources().end(); ++datasource) {

			if (!datasource->second.CanDatasourceBeFound()) {

				fileDatasourcesToBeUpdated.push_back(datasource);
			}
		}

		SynGlyphX::Application::restoreOverrideCursor();
		for (int i = 0; i < fileDatasourcesToBeUpdated.size(); ++i) {

			QString acceptButtonText = tr("Next");
			if (i == fileDatasourcesToBeUpdated.size() - 1) {
				acceptButtonText = tr("Ok");
			}

			ChangeDatasourceFileDialog dialog(fileDatasourcesToBeUpdated[i]->second, acceptButtonText, this);
			if (dialog.exec() == QDialog::Accepted) {

				mapping.UpdateDatasourceName(fileDatasourcesToBeUpdated[i]->first, dialog.GetNewDatasourceFile().toStdWString());
				wereDatasourcesUpdated = true;
			}
			else {
				throw std::exception("One or more datasources weren't found.");
			}
		}
		SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);

		if (wereDatasourcesUpdated) {

			mapping.WriteToFile(filename.toStdString());
		}

		GlyphViewerANTzTransformer transformer(QString::fromStdWString(m_cacheManager.GetCacheDirectory(mapping.GetID())));
		transformer.Transform(mapping);

		LoadFilesIntoModel(transformer.GetCSVFilenames(), transformer.GetBaseImageFilename());
	}
	catch (const std::exception& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		throw;
	}

	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::LoadFilesIntoModel(const QStringList& csvFiles, const QString& baseImageFilename) {

	m_glyphForestModel->LoadANTzFiles(csvFiles);

	//Base image must come after loading of CSV files
	if (baseImageFilename.isEmpty()) {

		m_glyphForestModel->UseDefaultBaseImage();
	}
	else {

		m_glyphForestModel->UseLocalBaseImage(baseImageFilename);
	}
}

void GlyphViewerWindow::ChangeMapDownloadSettings() {

	DownloadOptionsDialog dialog(this);
	dialog.exec();
}

void GlyphViewerWindow::ShowOpenGLSettings() {

	const QGLFormat& format = m_antzWidget->context()->format();
	QString settings = tr("OpenGL Version = ") + QString::number(format.majorVersion()) + "." + QString::number(format.minorVersion()) + '\n';
		
	settings += tr("Stereo Support") + " = ";
	if (m_antzWidget->IsStereoSupported()) {
		settings += tr("enabled");
	}
	else {
		settings += tr("disabled");
	}
	QMessageBox::information(this, tr("OpenGL Settings"), settings);
}

void GlyphViewerWindow::ChangeStereoMode() {

	if (m_antzWidget->IsStereoSupported()) {

		m_antzWidget->SetStereo(!m_antzWidget->IsInStereoMode());
	}
	else {

		m_stereoAction->setChecked(false);
		QMessageBox::information(this, tr("Stereo not supported"), tr("Stereo is not supported. Check your driver settings to see if stereo is enabled or contact the manufacturer of your GPU for assitance"));
	}
}

void GlyphViewerWindow::ImportFilesFromANTz() {

	ANTzImportDialog importDialog(this);
	if (importDialog.exec() == QDialog::Accepted) {

		SynGlyphX::ANTzVisualizationFileListing antzVisualization(importDialog.GetANTzNodeFilename().toStdWString(),
			importDialog.GetANTzTagFilename().toStdWString(),
			importDialog.GetANTzChannelFilename().toStdWString(),
			importDialog.GetANTzChannelMapFilename().toStdWString(),
			importDialog.GetANTzTagFilename().toStdWString());

		antzVisualization.WriteToFile(importDialog.GetOutputFilename().toStdString());
	}
}

void GlyphViewerWindow::EnableLoadedVisualizationDependentActions(bool enable) {

	for (int i = 0; i < m_loadedVisualizationDependentActions.length(); ++i) {

		m_loadedVisualizationDependentActions[i]->setEnabled(enable);
	}
}