#include "glyphviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStackedWidget>
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
	: SynGlyphX::MainWindow(parent),
	m_antzWidget(nullptr)
{
	m_sourceDataCache = std::make_shared<SynGlyphX::SourceDataCache>();
	m_glyphForestModel = new GlyphForestModel(this);

	m_glyphForestSelectionModel = new QItemSelectionModel(m_glyphForestModel, this);
	CreateMenus();
	CreateDockWidgets();

	QStackedWidget* antzWidgetContainer = new QStackedWidget(this);
	antzWidgetContainer->setContentsMargins(0, 0, 0, 0);
	antzWidgetContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QWidget* dummyWidget = new QWidget(antzWidgetContainer);
	dummyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	antzWidgetContainer->addWidget(dummyWidget);
	setCentralWidget(antzWidgetContainer);
	
	try {
		
		CreateANTzWidget(ANTzViewerWidget::GetStereoFormat());
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("3D view error"), tr("3D view failed to create: ") + e.what());
		throw;
	}

	m_isStereoSupported = m_antzWidget->IsInStereoMode();

	try {

		m_cacheManager.SetBaseCacheDirectory(GlyphViewerOptions::GetDefaultCacheDirectory().toStdWString());
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("Cache Directory Error"), tr("Cache Directory Error: ") + e.what());
		throw;
	}

	//ReadOptions();

	//QObject::connect(m_antzWidget, &ANTzViewerWidget::NewStatusMessage, statusBar(), &QStatusBar::showMessage);
	QObject::connect(m_glyphForestSelectionModel, &QItemSelectionModel::selectionChanged, this, [this]{  statusBar()->showMessage(tr("Selection Signal"), 4000); });

	m_stereoAction->setChecked(m_antzWidget->IsInStereoMode());

	QStringList commandLineArguments = SynGlyphX::Application::arguments();
	if (commandLineArguments.size() > 1) {

		QDir visualizationToLoad(commandLineArguments[1]);

		try {

			LoadVisualization(QDir::toNativeSeparators(visualizationToLoad.canonicalPath()));
		}
		catch (const std::exception& e) {

			QMessageBox::critical(this, tr("Failed To Open Visualization"), tr("Failed to open visualization from command line.  Error: ") + e.what(), QMessageBox::Ok);
		}
	}

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

GlyphViewerWindow::~GlyphViewerWindow()
{

}

void GlyphViewerWindow::CreateANTzWidget(const QGLFormat& format) {
	
	QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());
	if (m_antzWidget != nullptr) {
		
		antzWidgetContainer->removeWidget(m_antzWidget);
		delete m_antzWidget;
		m_antzWidget = nullptr;
	}

	m_antzWidget = new ANTzViewerWidget(format, m_glyphForestModel, m_glyphForestSelectionModel, this);
	m_antzWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	antzWidgetContainer->addWidget(m_antzWidget);
	antzWidgetContainer->setCurrentWidget(m_antzWidget);

	QObject::connect(m_showAnimation, &QAction::toggled, m_antzWidget, &ANTzViewerWidget::ShowAnimatedRotations);
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

	m_showAnimation = m_viewMenu->addAction(tr("Show Animation"));
	m_showAnimation->setCheckable(true);
	m_showAnimation->setChecked(true);

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

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

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
	QObject::connect(m_sourceDataSelectionWidget, &SourceDataSelectionWidget::OptionsChanged, this, &GlyphViewerWindow::OnSourceDataSelectionWidgetOptionsChanged);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
	m_viewMenu->addAction(rightDockWidget->toggleViewAction());

	QDockWidget* bottomDockWidget = new QDockWidget(tr("Psuedo-Time Filter"), this);
	m_pseudoTimeFilterWidget = new PseudoTimeFilterWidget(bottomDockWidget);
	addDockWidget(Qt::BottomDockWidgetArea, bottomDockWidget);
	m_viewMenu->addAction(bottomDockWidget->toggleViewAction());
}

void GlyphViewerWindow::OpenProject() {

	QString openFile = GetFileNameOpenDialog("VisualizationDir", tr("Open Visualization"), "", tr("SynGlyphX Visualization Files (*.sdt *.sav);;SynGlyphX Data Transform Files (*.sdt);;SynGlyphX ANTz Visualization Files (*.sav)"));
	if (!openFile.isEmpty()) {

		LoadNewVisualization(openFile);
	}
}

void GlyphViewerWindow::RefreshVisualization() {

	try {
		
		SynGlyphX::DataTransformMapping mapping;
		mapping.ReadFromFile(m_currentFilename.toStdString());
		if ((m_mapping != mapping) || (m_sourceDataCache->IsCacheOutOfDate(m_mapping.GetDatasourcesInUse()))) {

			ClearAllData();
			LoadVisualization(m_currentFilename);
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Refresh Visualization"), tr("Failed to refresh visualization.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	statusBar()->showMessage("Visualization successfully refreshed", 3000);
}

void GlyphViewerWindow::CloseVisualization() {

	ClearAllData();
	EnableLoadedVisualizationDependentActions(false);
	ClearCurrentFile();
}

void GlyphViewerWindow::ClearAllData() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	m_sourceDataCache->Close();
	m_glyphForestModel->Clear();
	m_glyphForestModel->SetParentGridToDefaultBaseImage();
	m_mapping.Clear();
	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::LoadVisualization(const QString& filename) {

	QFileInfo fileInfo(filename);
	QString extension = fileInfo.suffix().toLower();
	if ((extension != "sdt") && (extension != "sav")) {

		throw std::exception("File is not a recognized format");
	}

	if (!fileInfo.exists()) {

		throw std::exception("File does not exist");
	}

	if (!m_glyphForestSelectionModel->selectedIndexes().empty()) {

		m_glyphForestSelectionModel->clearSelection();
		m_antzWidget->updateGL();
	}

	if (extension == "sdt") {

		LoadDataTransform(filename);
	}
	else if (extension == "sav") {

		LoadANTzCompatibilityVisualization(filename);
	}
}

bool GlyphViewerWindow::LoadNewVisualization(const QString& filename) {

	if (filename == m_currentFilename) {
		
		return true;
	}

	try {

		LoadVisualization(filename);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Open Visualization"), tr("Failed to open visualization.  Error: ") + e.what(), QMessageBox::Ok);
		return false;
	}

	SetCurrentFile(filename);
	EnableLoadedVisualizationDependentActions(true);
	statusBar()->showMessage("Visualization successfully opened", 3000);
	return true;
}

void GlyphViewerWindow::LoadANTzCompatibilityVisualization(const QString& filename) {

	SynGlyphXANTz::ANTzVisualizationFileListing fileListing;
	fileListing.ReadFromFile(filename.toStdString());
	QStringList csvFiles;


	//LoadFilesIntoModel(csvFiles, QString::fromStdWString(fileListing.GetBaseImageFilename()));
}

bool GlyphViewerWindow::LoadRecentFile(const QString& filename) {

	return LoadNewVisualization(filename);
}

void GlyphViewerWindow::LoadDataTransform(const QString& filename) {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	try {

		m_mapping.ReadFromFile(filename.toStdString());

		if (!m_mapping.GetDatasources().HasDatasources()) {

			throw std::exception("Visualization has no datasources.");
		}

		if (m_mapping.GetGlyphGraphs().empty()) {

			throw std::exception("Visualization has no glyph templates.");
		}

		if (!m_mapping.DoesAtLeastOneGlyphGraphHaveBindingsOnPosition()) {

			throw std::exception("Visualization has no glyph templates with bindings on Position X, Position Y, or Position Z.");
		}

		bool wereDatasourcesUpdated = false;

		SynGlyphX::DatasourceMaps datasourcesInUse = m_mapping.GetDatasourcesInUse();
		std::vector<SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator> fileDatasourcesToBeUpdated;
		for (SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator datasource = datasourcesInUse.GetFileDatasources().begin(); datasource != datasourcesInUse.GetFileDatasources().end(); ++datasource) {

			if (!datasource->second.CanDatasourceBeFound()) {

				fileDatasourcesToBeUpdated.push_back(datasource);
			}
		}

		if (!fileDatasourcesToBeUpdated.empty()) {

			SynGlyphX::Application::restoreOverrideCursor();
			for (int i = 0; i < fileDatasourcesToBeUpdated.size(); ++i) {

				QString acceptButtonText = tr("Next");
				if (i == fileDatasourcesToBeUpdated.size() - 1) {
					
					acceptButtonText = tr("Ok");
				}

				ChangeDatasourceFileDialog dialog(fileDatasourcesToBeUpdated[i]->second, acceptButtonText, this);
				if (dialog.exec() == QDialog::Accepted) {

					m_mapping.UpdateDatasourceName(fileDatasourcesToBeUpdated[i]->first, dialog.GetNewDatasourceFile().toStdWString());
					wereDatasourcesUpdated = true;
				}
				else {
					
					throw std::exception("One or more datasources weren't found.");
				}
			}
			SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

			if (wereDatasourcesUpdated) {

				m_mapping.WriteToFile(filename.toStdString());
			}
		}

		SynGlyphXANTz::GlyphViewerANTzTransformer transformer(QString::fromStdWString(m_cacheManager.GetCacheDirectory(m_mapping.GetID())));
		transformer.Transform(m_mapping);

		m_sourceDataCache->Setup(transformer.GetSourceDataCacheLocation());
		LoadFilesIntoModel(transformer.GetCSVFilenames(), transformer.GetBaseImageFilenames());
		m_glyphForestModel->SetTagNotToBeShownIn3d(QString::fromStdWString(m_mapping.GetDefaults().GetDefaultTagValue()));
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
	if (m_isStereoSupported) {
		settings += tr("enabled");
	}
	else {
		settings += tr("disabled");
	}
	QMessageBox::information(this, tr("OpenGL Settings"), settings);
}

void GlyphViewerWindow::ChangeStereoMode() {

	if (m_isStereoSupported) {

		if (m_antzWidget->IsInStereoMode()) {

			CreateANTzWidget(ANTzViewerWidget::GetNonStereoFormat());
		}
		else {

			CreateANTzWidget(ANTzViewerWidget::GetStereoFormat());
		}
	}
	else {

		m_stereoAction->setChecked(false);
		QMessageBox::information(this, tr("Stereo not supported"), tr("Stereo is not supported. Check your driver settings to see if stereo is enabled and available for your hardware.  Contact the manufacturer of your GPU for assitance."));
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

	m_stereoAction->setEnabled(!enable);
}

void GlyphViewerWindow::ChangeOptions() {

	OptionsWidget* optionsWidget = new OptionsWidget(m_options, (m_glyphForestModel->rowCount() == 0), this);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, optionsWidget, this);
	dialog.setWindowTitle(tr("Glyph Viewer Options"));

	if (dialog.exec() == QDialog::Accepted) {

		try {

			ChangeOptions(optionsWidget->GetOptions());
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("Options Error"), tr("Options was not updated: ") + e.what());
		}
	}
}

void GlyphViewerWindow::ChangeOptions(const GlyphViewerOptions& options) { //, bool writeOptions) {

	if (options != m_options) {

		if (m_options.GetCacheDirectory() != options.GetCacheDirectory()) {

			QString newCacheDirectory(options.GetCacheDirectory());
			QDir cacheDir(newCacheDirectory);
			if (!cacheDir.exists()) {

				if (!cacheDir.mkpath(newCacheDirectory)) {

					throw std::invalid_argument("Unable to create " + newCacheDirectory.toStdString());
				}
			}

			m_cacheManager.SetBaseCacheDirectory(newCacheDirectory.toStdWString());
		}

		if (m_options.GetHideUnselectedGlyphTrees() != options.GetHideUnselectedGlyphTrees()) {

			m_antzWidget->SetHideUnselectedGlyphTrees(options.GetHideUnselectedGlyphTrees());
			m_sourceDataSelectionWidget->SetHideUnselectedTreesCheckbox(options.GetHideUnselectedGlyphTrees());
		}

		if (m_options.GetZSpaceOptions() != options.GetZSpaceOptions()) {

			m_antzWidget->SetZSpaceOptions(options.GetZSpaceOptions());
		}

		m_options = options;

		//if (writeOptions) {
		//
		//	WriteOptions();
		//}
	}
}

void GlyphViewerWindow::ReadSettings() {

	SynGlyphX::MainWindow::ReadSettings();

	GlyphViewerOptions options;

	QSettings settings;

	settings.beginGroup("Display");
	if (!settings.value("ShowAnimation", true).toBool()) {

		m_showAnimation->toggle();
	}
	settings.endGroup();

	settings.beginGroup("Options");

	QString cacheDirectory = QDir::toNativeSeparators(settings.value("cacheDirectory", GlyphViewerOptions::GetDefaultCacheDirectory()).toString());
	if (cacheDirectory.isEmpty()) {

		cacheDirectory = GlyphViewerOptions::GetDefaultCacheDirectory();
	}
	options.SetCacheDirectory(cacheDirectory);
	options.SetHideUnselectedGlyphTrees(settings.value("hideUnselectedGlyphs", false).toBool());
	settings.endGroup();

	settings.beginGroup("zSpace");
	ZSpaceOptions zSpaceOptions;
	zSpaceOptions.SetStylusColor(settings.value("stylusColor", QColor(Qt::green)).value<QColor>());
	zSpaceOptions.SetStylusLength(settings.value("stylusLength", 0.15f).toFloat());
	settings.endGroup();

	options.SetZSpaceOptions(zSpaceOptions);

	ChangeOptions(options); // , false);
}

void GlyphViewerWindow::WriteSettings() {

	SynGlyphX::MainWindow::WriteSettings();

	QSettings settings;

	settings.beginGroup("Display");
	settings.setValue("ShowAnimation", m_showAnimation->isChecked());
	settings.endGroup();

	settings.beginGroup("Options");

	if (m_options.GetCacheDirectory() != GlyphViewerOptions::GetDefaultCacheDirectory()) {

		settings.setValue("cacheDirectory", m_options.GetCacheDirectory());
	}
	else {

		settings.setValue("cacheDirectory", "");
	}
	settings.setValue("hideUnselectedGlyphs", m_options.GetHideUnselectedGlyphTrees());
	settings.endGroup();

	settings.beginGroup("zSpace");
	settings.setValue("stylusColor", m_options.GetZSpaceOptions().GetStylusColor());
	settings.setValue("stylusLength", m_options.GetZSpaceOptions().GetStylusLength());
	settings.endGroup();
}

void GlyphViewerWindow::OnSourceDataSelectionWidgetOptionsChanged() {

	GlyphViewerOptions options = m_options;
	options.SetHideUnselectedGlyphTrees(m_sourceDataSelectionWidget->GetHideUnselectedTreesCheckbox());
	ChangeOptions(options);
}