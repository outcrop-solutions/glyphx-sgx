#include "glyphviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
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
#include "singlewidgetdialog.h"
#include "optionswidget.h"

GlyphViewerWindow::GlyphViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(parent)
{
	m_sourceDataCache = std::make_shared<SynGlyphX::SourceDataCache>();
	m_glyphForestModel = new GlyphForestModel(this);

	m_glyphForestSelectionModel = new QItemSelectionModel(m_glyphForestModel, this);
	ReadOptions();	
	CreateMenus();
	CreateDockWidgets();
	
	try {
		m_antzWidget = new ANTzViewerWidget(m_glyphForestModel, m_glyphForestSelectionModel, this);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("3D view error"), tr("3D view failed to create: ") + e.what());
		throw;
	}

	setCentralWidget(m_antzWidget);

#ifdef DEBUG
	QObject::connect(m_antzWidget, &ANTzViewerWidget::NewStatusMessage, statusBar(), &QStatusBar::showMessage);
#endif

	m_stereoAction->setChecked(m_antzWidget->IsInStereoMode());

	QStringList commandLineArguments = SynGlyphX::Application::arguments();
	if (commandLineArguments.size() > 1) {

		QDir visualizationToLoad(commandLineArguments[1]);
		LoadVisualization(QDir::toNativeSeparators(visualizationToLoad.canonicalPath()));
	}

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

	QAction* optionsAction = m_toolsMenu->addAction(tr("Options"));
	QObject::connect(optionsAction, &QAction::triggered, this, static_cast<void (GlyphViewerWindow::*)()>(&GlyphViewerWindow::ChangeOptions));

	m_toolsMenu->addSeparator();

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeMapDownloadSettings);

	CreateHelpMenu();

	QAction* openGLSettingsAction = new QAction(tr("OpenGL Settings"), m_helpMenu);
	QObject::connect(openGLSettingsAction, &QAction::triggered, this, &GlyphViewerWindow::ShowOpenGLSettings);

	m_helpMenu->insertAction(m_aboutBoxAction, openGLSettingsAction);
	m_helpMenu->insertSeparator(m_aboutBoxAction);

	EnableLoadedVisualizationDependentActions(false);
}

void GlyphViewerWindow::CreateDockWidgets() {

	//Add Tree View to dock widget on left side
	QDockWidget* leftDockWidget = new QDockWidget(tr("Glyph List"), this);
	m_treeView = new GlyphTreeListView(leftDockWidget);
	m_treeView->setModel(m_glyphForestModel);
	m_treeView->setSelectionModel(m_glyphForestSelectionModel);

	leftDockWidget->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
	m_viewMenu->addAction(leftDockWidget->toggleViewAction());

	QDockWidget* rightDockWidget = new QDockWidget(tr("Source Data Selector"), this);
	m_sourceDataSelectionWidget = new SourceDataSelectionWidget(m_sourceDataCache, m_glyphForestModel, m_glyphForestSelectionModel, rightDockWidget);
	rightDockWidget->setWidget(m_sourceDataSelectionWidget);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
	m_viewMenu->addAction(rightDockWidget->toggleViewAction());
}

void GlyphViewerWindow::OpenProject() {

	QString openFile = GetFileNameOpenDialog("VisualizationDir", tr("Open Visualization"), "", tr("SynGlyphX Visualization Files (*.sdt *.sav);;SynGlyphX Data Transform Files (*.sdt);;SynGlyphX ANTz Visualization Files (*.sav)"));
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

	SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);
	m_sourceDataCache->Close();
	m_glyphForestModel->Clear();
	m_glyphForestModel->SetParentGridToDefaultBaseImage();
	EnableLoadedVisualizationDependentActions(false);
	ClearCurrentFile();
	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::LoadVisualization(const QString& filename) {

	QFileInfo fileInfo(filename);
	QString extension = fileInfo.suffix().toLower();
	if ((extension != "sdt") && (extension != "sav")) {

		QMessageBox::warning(this, tr("Loading Visualization Failed"), tr("File is not a recognized format"));
		return;
	}

	if (!fileInfo.exists()) {

		QMessageBox::warning(this, tr("Loading Visualization Failed"), tr("File does not exist"));
		return;
	}

	if (!m_treeView->selectionModel()->selectedIndexes().empty()) {

		m_treeView->selectionModel()->clearSelection();
		m_antzWidget->updateGL();
	}

	if (extension == "sdt") {

		LoadDataTransform(filename);
	}
	else if (extension == "sav") {

		LoadANTzCompatibilityVisualization(filename);
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

	SynGlyphXANTz::ANTzVisualizationFileListing fileListing;
	fileListing.ReadFromFile(filename.toStdString());
	QStringList csvFiles;


	//LoadFilesIntoModel(csvFiles, QString::fromStdWString(fileListing.GetBaseImageFilename()));
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

		SynGlyphXANTz::GlyphViewerANTzTransformer transformer(QString::fromStdWString(m_cacheManager.GetCacheDirectory(mapping.GetID())));
		transformer.Transform(mapping);

		m_sourceDataCache->Setup(transformer.GetSourceDataCacheLocation());
		LoadFilesIntoModel(transformer.GetCSVFilenames(), transformer.GetBaseImageFilenames());
	}
	catch (const std::exception& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		throw;
	}

	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::LoadFilesIntoModel(const QStringList& csvFiles, const QStringList& baseImageFilenames) {

	m_glyphForestModel->LoadANTzVisualization(csvFiles, baseImageFilenames);
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

		SynGlyphXANTz::ANTzVisualizationFileListing antzVisualization(importDialog.GetANTzNodeFilename().toStdWString(),
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

void GlyphViewerWindow::ChangeOptions() {

	OptionsWidget* optionsWidget = new OptionsWidget(m_options, (m_glyphForestModel->rowCount() == 0), this);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, optionsWidget, this);
	dialog.setWindowTitle(tr("Glyph Viewer Options"));

	if (dialog.exec() == QDialog::Accepted) {

		try {

			ChangeOptions(optionsWidget->GetOptions());
			WriteOptions();
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("Options Error"), tr("Options was not updated: ") + e.what());
		}
	}
}

void GlyphViewerWindow::ChangeOptions(const GlyphViewerOptions& options) {

	if (options != m_options) {

		QString newCacheDirectory(options.GetCacheDirectory());
		QDir cacheDir(newCacheDirectory);
		if (!cacheDir.exists()) {

			if (!cacheDir.mkpath(newCacheDirectory)) {

				throw std::invalid_argument("Unable to create " + newCacheDirectory.toStdString());
			}
		}

		m_options = options;
		m_cacheManager.SetBaseCacheDirectory(m_options.GetCacheDirectory().toStdWString());
	}
}

void GlyphViewerWindow::ReadOptions() {

	GlyphViewerOptions options;

	QSettings settings;
	settings.beginGroup("Options");

	QString cacheDirectory = QDir::toNativeSeparators(settings.value("cacheDirectory", SynGlyphX::Application::GetCommonDataLocation() + QDir::separator() + "gv_cache").toString());
	options.SetCacheDirectory(cacheDirectory);
	settings.endGroup();

	ChangeOptions(options);
}

void GlyphViewerWindow::WriteOptions() {

	QSettings settings;
	settings.beginGroup("Options");

	settings.setValue("cacheDirectory", m_options.GetCacheDirectory());
	settings.endGroup();
}