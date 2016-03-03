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
#include <QtCore/QDateTime>
#include "glyphbuilderapplication.h"
#include "datatransformmapping.h"
#include "downloadoptionsdialog.h"
#include "data/npmapfile.h"
#include "sourcedatamanager.h"
//#include "glyphviewerantztransformer.h"
#include "changedatasourcefiledialog.h"
#include "antzimportdialog.h"
#include "antzvisualizationfilelisting.h"
#include "singlewidgetdialog.h"
#include "optionswidget.h"
#include "userdefinedbaseimageproperties.h"
#include "changeimagefiledialog.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include "filesystem.h"
#include "defaultbaseimageproperties.h"

GlyphViewerWindow::GlyphViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(2, parent),
	m_antzWidget(nullptr),
	m_showErrorFromTransform(true)
{
	m_mappingModel = new SynGlyphX::DataMappingModel(this);
	m_sourceDataCache = std::make_shared<SynGlyphX::SourceDataCache>();
	m_glyphForestModel = new SynGlyphXANTz::GlyphForestModel(this);

	m_glyphForestSelectionModel = new SynGlyphX::ItemFocusSelectionModel(m_glyphForestModel, this);
	m_sourceDataSelectionModel = new SourceDataSelectionModel(m_mappingModel, m_sourceDataCache, m_glyphForestSelectionModel, this);

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
		
		CreateANTzWidget(SynGlyphXANTz::ANTzForestWidget::GetStereoFormat());
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

	m_linkedWidgetsManager = new LinkedWidgetsManager(m_antzWidget, this);
	m_sourceDataSelectionWidget->SetupLinkedWidgets(*m_linkedWidgetsManager);
	m_pseudoTimeFilterWidget->SetupLinkedWidgets(*m_linkedWidgetsManager);

	m_stereoAction->setChecked(m_antzWidget->IsInStereoMode());

	//SynGlyphX::Transformer::SetDefaultImagesDirectory(SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation());
	//SynGlyphXANTz::ANTzCSVWriter::GetInstance().SetNOURLLocation(L"");

	try {

		if (!dec.hasJVM()){
			dec.createJVM();
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("JVM Error"), tr(e.what()));
		return;
	}

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

void GlyphViewerWindow::closeJVM(){
	dec.destroyJVM();
}

void GlyphViewerWindow::CreateANTzWidget(const QGLFormat& format) {
	
	QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());
	if (m_antzWidget != nullptr) {
		
		antzWidgetContainer->removeWidget(m_antzWidget);
		delete m_antzWidget;
		m_antzWidget = nullptr;
	}

	m_antzWidget = new SynGlyphXANTz::ANTzForestWidget(format, m_glyphForestModel, m_glyphForestSelectionModel, this);
	m_antzWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_antzWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
	m_antzWidget->addActions(m_treeView->GetSharedActions());

	antzWidgetContainer->addWidget(m_antzWidget);
	antzWidgetContainer->setCurrentWidget(m_antzWidget);

	QObject::connect(m_showAnimation, &QAction::toggled, m_antzWidget, &SynGlyphXANTz::ANTzForestWidget::ShowAnimatedRotations);
	QObject::connect(m_showTagsAction, &QAction::toggled, m_antzWidget, &SynGlyphXANTz::ANTzForestWidget::SetShowTagsOfSelectedObjects);
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

	CreateExportToPortableVisualizationSubmenu();
	
	QObject::connect(m_glyphForestSelectionModel, &SynGlyphX::ItemFocusSelectionModel::selectionChanged, this, &GlyphViewerWindow::OnSelectionChanged);

	m_fileMenu->addActions(m_recentFileActions);

	m_fileMenu->addSeparator();

	QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
	QObject::connect(exitAction, &QAction::triggered, this, &GlyphViewerWindow::close);

	//Create View Menu
	CreateViewMenu();

	m_stereoAction = CreateMenuAction(m_viewMenu, tr("Stereo"));
	m_stereoAction->setCheckable(true);
	QObject::connect(m_stereoAction, &QAction::triggered, this, &GlyphViewerWindow::ChangeStereoMode);

	m_viewMenu->addSeparator();

	m_showAnimation = m_viewMenu->addAction(tr("Show Animation"));
	m_showAnimation->setCheckable(true);
	m_showAnimation->setChecked(true);

	m_viewMenu->addSeparator();

	m_showTagsAction = CreateMenuAction(m_viewMenu, tr("Show Tags"), Qt::Key_I);
	m_showTagsAction->setCheckable(true);
	m_showTagsAction->setChecked(false);
	m_loadedVisualizationDependentActions.push_back(m_showTagsAction);

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

	m_glyphListDockWidget = new QDockWidget(tr("Glyph List"), this);
	m_treeView = new GlyphTreeListView(m_glyphListDockWidget);
	m_treeView->setModel(m_glyphForestModel);
	m_treeView->SetItemFocusSelectionModel(m_glyphForestSelectionModel);

	m_glyphListDockWidget->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, m_glyphListDockWidget);
	m_viewMenu->addAction(m_glyphListDockWidget->toggleViewAction());

	m_glyphPropertiesWidgetContainer = new GlyphPropertiesWidgetsContainer(m_glyphForestModel, m_glyphForestSelectionModel, this);

	/*QDockWidget* visualPropertiesDockWidget = new QDockWidget(tr("Visual Properties"), this);
	visualPropertiesDockWidget->setWidget(m_glyphPropertiesWidgetContainer->GetVisualProperitesWidget());
	addDockWidget(Qt::LeftDockWidgetArea, visualPropertiesDockWidget);
	m_viewMenu->addAction(visualPropertiesDockWidget->toggleViewAction());*/

	QDockWidget* textPropertiesDockWidget = new QDockWidget(tr("Text Properties"), this);
	textPropertiesDockWidget->setWidget(m_glyphPropertiesWidgetContainer->GetTextProperitesWidget());
	addDockWidget(Qt::LeftDockWidgetArea, textPropertiesDockWidget);
	m_viewMenu->addAction(textPropertiesDockWidget->toggleViewAction());

	tabifyDockWidget(m_glyphListDockWidget, textPropertiesDockWidget);
	m_glyphListDockWidget->raise();

	QDockWidget* rightDockWidget = new QDockWidget(tr("Source Data Selector"), this);
	m_sourceDataSelectionWidget = new MultiTableElasticListsWidget(m_sourceDataCache, m_sourceDataSelectionModel, rightDockWidget);
	rightDockWidget->setWidget(m_sourceDataSelectionWidget);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
	m_viewMenu->addAction(rightDockWidget->toggleViewAction());

	QDockWidget* bottomDockWidget = new QDockWidget(tr("Time Animated Filter"), this);
	m_pseudoTimeFilterWidget = new PseudoTimeFilterWidget(m_mappingModel->GetDataMapping(), m_sourceDataCache, m_sourceDataSelectionModel, bottomDockWidget);
	bottomDockWidget->setWidget(m_pseudoTimeFilterWidget);
	addDockWidget(Qt::BottomDockWidgetArea, bottomDockWidget);
	m_viewMenu->addAction(bottomDockWidget->toggleViewAction());
}

void GlyphViewerWindow::OpenProject() {

	QString openFile = GetFileNameOpenDialog("VisualizationDir", tr("Open Visualization"), "", tr("SynGlyphX Visualization Files (*.sdt *.sav);;SynGlyphX Data Transform Files (*.sdt);;SynGlyphX ANTz Visualization Files (*.sav)"));
	if (!openFile.isEmpty()) {

		LoadNewVisualization(openFile);
	}
}

bool GlyphViewerWindow::DoesVisualizationNeedToBeRecreated(const SynGlyphX::DataTransformMapping& mapping) const {

	if ((m_mappingModel->GetDataMapping()->operator!=(mapping))) {

		return true;
	}

	if (m_sourceDataCache->IsCacheOutOfDate(m_mappingModel->GetDataMapping()->GetDatasourcesInUse())) {

		return true;
	}

	QDir cacheDir(QString::fromStdWString(m_cacheManager.GetCacheDirectory(m_mappingModel->GetDataMapping()->GetID())));
	QDateTime lastUpdateTimeForCache = cacheDir.entryInfoList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files).first().lastModified();
	for (const auto& baseObject : m_mappingModel->GetDataMapping()->GetBaseObjects()) {

		SynGlyphX::UserDefinedBaseImageProperties::ConstSharedPtr properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(baseObject.GetProperties());
		if (properties) {

			if (QFileInfo(QString::fromStdWString(properties->GetFilename())).lastModified() > lastUpdateTimeForCache) {

				return true;
			}
		}
	}

	return false;
}

void GlyphViewerWindow::RefreshVisualization() {

	try {
		
		SynGlyphX::DataTransformMapping mapping;
		mapping.ReadFromFile(m_currentFilename.toStdString());
		if (DoesVisualizationNeedToBeRecreated(mapping)) {

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

	m_pseudoTimeFilterWidget->Disable();
	ClearAllData();
	EnableLoadedVisualizationDependentActions(false);
	ClearCurrentFile();
	m_antzWidget->SetBackgroundColor(SynGlyphX::GlyphColor::s_black);

}

void GlyphViewerWindow::ClearAllData() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	m_glyphForestSelectionModel->ClearAll();
	m_sourceDataCache->Close();
	m_glyphForestModel->Clear();
	m_mappingModel->Clear();
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

	if (m_glyphForestModel->rowCount() > 0) {

		ClearAllData();
		m_antzWidget->updateGL();
	}

	if (extension == "sdt") {

		LoadDataTransform(filename);
	}
	else if (extension == "sav") {

		LoadANTzCompatibilityVisualization(filename);
	}

	m_glyphListDockWidget->raise();
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
	m_pseudoTimeFilterWidget->ResetForNewVisualization();
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

void GlyphViewerWindow::ValidateDataMappingFile(const QString& filename) {

	SynGlyphX::DataTransformMapping::SharedPtr mapping = std::make_shared<SynGlyphX::DataTransformMapping>();
	mapping->ReadFromFile(filename.toStdString());

	if (!mapping->GetDatasources().HasDatasources()) {

		throw std::exception("Visualization has no datasources.");
	}

	if (mapping->GetGlyphGraphs().empty()) {

		throw std::exception("Visualization has no glyph templates.");
	}

	if (!mapping->DoesAtLeastOneGlyphGraphHaveBindingsOnPosition()) {

		throw std::exception("Visualization has no glyph templates with bindings on Position X, Position Y, or Position Z.");
	}

	bool wasDataTransformUpdated = false;

	std::vector<boost::uuids::uuid> missingFileDatasources = mapping->GetFileDatasourcesWithInvalidFiles(true);

	std::vector<unsigned int> localBaseImageIndexes = mapping->GetFileBaseObjectsWithInvalidFiles();

	if (!localBaseImageIndexes.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateImageFiles(localBaseImageIndexes, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::exception("Visualization has missing base images that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	if (!missingFileDatasources.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeDatasourceFileDialog::UpdateDatasourceFiles(missingFileDatasources, filename, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::exception("Visualization has missing data sources that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	if (wasDataTransformUpdated) {

		mapping->WriteToFile(filename.toStdString());
	}
}

void GlyphViewerWindow::LoadDataTransform(const QString& filename) {
	/*
	try {

		if (!dec.hasJVM()){
			dec.createJVM();
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("JVM Error"), tr(e.what()));
		return;
	}*/

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	try {

		ValidateDataMappingFile(filename);

		m_mappingModel->LoadDataTransformFile(filename);
		std::string dcd = GlyphViewerOptions::GetDefaultCacheDirectory().toStdString();
		std::string cacheDirectoryPath = dcd + ("\\cache_" + boost::uuids::to_string(m_mappingModel->GetDataMapping()->GetID()));

		//SynGlyphXANTz::GlyphViewerANTzTransformer transformer(QString::fromStdString(cacheDirectoryPath.string()));
		//transformer.Transform(*m_mappingModel->GetDataMapping());
		
		DataEngine::GlyphEngine ge;
		std::string dirPath = cacheDirectoryPath + "\\";
		std::string baseImageDir = SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation().toStdString();
		ge.initiate(dec.getEnv(), filename.toStdString(), dirPath, baseImageDir, "", "GlyphViewer");
		ge.getDownloadedBaseImage(m_mappingModel->GetDataMapping().get()->GetBaseObjects());
		ge.generateGlyphs();
		std::vector<std::string> images = ge.getBaseImages();
		
		QStringList cacheFiles;
		QString localOutputDir = QString::fromStdString(dirPath + "antz\\");
		cacheFiles.push_back(localOutputDir + "antz.csv");
		cacheFiles.push_back(localOutputDir + "antztag.csv");
		cacheFiles.push_back(QString::fromStdString(dirPath + "sourcedata.db"));

		SynGlyphXANTz::ANTzCSVWriter::FilenameList outputfiles;
		outputfiles[SynGlyphXANTz::ANTzCSVWriter::s_nodeFilenameIndex] = cacheFiles[0].toStdString();
		outputfiles[SynGlyphXANTz::ANTzCSVWriter::s_tagFilenameIndex] = cacheFiles[1].toStdString();

		QStringList qList;
		for (int i = 0; i < images.size(); i++){
			qList << images.at(i).c_str();
		}
		//m_sourceDataCache->Setup(transformer.GetSourceDataCacheLocation());
		m_sourceDataCache->Setup(cacheFiles[2]);
		LoadFilesIntoModel(outputfiles, qList);
		m_glyphForestModel->SetTagNotToBeShownIn3d(QString::fromStdWString(m_mappingModel->GetDataMapping()->GetDefaults().GetDefaultTagValue()));
		m_antzWidget->SetBackgroundColor(m_mappingModel->GetDataMapping()->GetSceneProperties().GetBackgroundColor());

		SynGlyphX::Application::restoreOverrideCursor();
		const QString& transformerError = ge.getError();
		if ((!transformerError.isNull())) { //&& m_showErrorFromTransform
			QMessageBox::information(this, "Transformation Error", transformerError, QMessageBox::Ok);
		}
	}
	catch (const std::exception& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		throw;
	}
}

void GlyphViewerWindow::LoadFilesIntoModel(const SynGlyphXANTz::ANTzCSVWriter::FilenameList& filesToLoad, const QStringList& baseImageFilenames) {

	m_glyphForestModel->LoadANTzVisualization(filesToLoad, baseImageFilenames);
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

			CreateANTzWidget(SynGlyphXANTz::ANTzForestWidget::GetNonStereoFormat());
		}
		else {

			CreateANTzWidget(SynGlyphXANTz::ANTzForestWidget::GetStereoFormat());
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

	m_showTagsAction->setChecked(false);
}

void GlyphViewerWindow::ChangeOptions() {

	GlyphViewerOptions oldOptions = CollectOptions();
	OptionsWidget* optionsWidget = new OptionsWidget(oldOptions, (m_glyphForestModel->rowCount() == 0), this);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, optionsWidget, this);
	dialog.setWindowTitle(tr("Glyph Viewer Options"));

	if (dialog.exec() == QDialog::Accepted) {

		try {

			ChangeOptions(oldOptions, optionsWidget->GetOptions());
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("Options Error"), tr("Options was not updated: ") + e.what());
		}
	}
}

void GlyphViewerWindow::ChangeOptions(const GlyphViewerOptions& oldOptions, const GlyphViewerOptions& newOptions) {

	if (oldOptions != newOptions) {

		if (oldOptions.GetCacheDirectory() != newOptions.GetCacheDirectory()) {

			QString newCacheDirectory(newOptions.GetCacheDirectory());
			QDir cacheDir(newCacheDirectory);
			if (!cacheDir.exists()) {

				if (!cacheDir.mkpath(newCacheDirectory)) {

					throw std::invalid_argument("Unable to create " + newCacheDirectory.toStdString());
				}
			}

			m_cacheManager.SetBaseCacheDirectory(newCacheDirectory.toStdWString());
		}

		if (oldOptions.GetHideUnselectedGlyphTrees() != newOptions.GetHideUnselectedGlyphTrees()) {

			m_linkedWidgetsManager->SetFilterView(newOptions.GetHideUnselectedGlyphTrees());
		}

		if (oldOptions.GetZSpaceOptions() != newOptions.GetZSpaceOptions()) {

			m_antzWidget->SetZSpaceOptions(newOptions.GetZSpaceOptions());
		}

		if (oldOptions.GetShowMessageWhenImagesDidNotDownload() != newOptions.GetShowMessageWhenImagesDidNotDownload()) {

			m_showErrorFromTransform = newOptions.GetShowMessageWhenImagesDidNotDownload();
		}
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
	options.SetShowMessageWhenImagesDidNotDownload(settings.value("showFailedToDownloadImageMessage", true).toBool());
	settings.endGroup();

	settings.beginGroup("zSpace");
	SynGlyphX::ZSpaceOptions zSpaceOptions;
	zSpaceOptions.SetStylusColor(settings.value("stylusColor", QColor(Qt::green)).value<QColor>());
	zSpaceOptions.SetStylusLength(settings.value("stylusLength", 0.15f).toFloat());
	settings.endGroup();

	options.SetZSpaceOptions(zSpaceOptions);

	ChangeOptions(CollectOptions(), options);
}

void GlyphViewerWindow::WriteSettings() {

	SynGlyphX::MainWindow::WriteSettings();

	QSettings settings;

	settings.beginGroup("Display");
	settings.setValue("ShowAnimation", m_showAnimation->isChecked());
	settings.endGroup();

	GlyphViewerOptions options = CollectOptions();

	settings.beginGroup("Options");

	if (options.GetCacheDirectory() != GlyphViewerOptions::GetDefaultCacheDirectory()) {

		settings.setValue("cacheDirectory", options.GetCacheDirectory());
	}
	else {

		settings.setValue("cacheDirectory", "");
	}
	settings.setValue("hideUnselectedGlyphs", options.GetHideUnselectedGlyphTrees());
	settings.setValue("showFailedToDownloadImageMessage", options.GetShowMessageWhenImagesDidNotDownload());
	settings.endGroup();

	settings.beginGroup("zSpace");
	settings.setValue("stylusColor", options.GetZSpaceOptions().GetStylusColor());
	settings.setValue("stylusLength", options.GetZSpaceOptions().GetStylusLength());
	settings.endGroup();
}

GlyphViewerOptions GlyphViewerWindow::CollectOptions() {

	GlyphViewerOptions options;

	options.SetCacheDirectory(QString::fromStdWString(m_cacheManager.GetBaseCacheDirectory()));
	options.SetHideUnselectedGlyphTrees(m_linkedWidgetsManager->GetFilterView());
	options.SetZSpaceOptions(m_antzWidget->GetZSpaceOptions());
	options.SetShowMessageWhenImagesDidNotDownload(m_showErrorFromTransform);

	return options;
}

void GlyphViewerWindow::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	
}

void GlyphViewerWindow::CreateExportToPortableVisualizationSubmenu() {

	if (m_portableVisualizationExport.DoAnyPlatformsHaveSourceDirectories()) {

		m_fileMenu->addSeparator();
		QMenu* portableVisualizationMenu = m_fileMenu->addMenu(tr("Create Portable Visualization"));

		m_portableVisualizationExport.CreateSubmenu(portableVisualizationMenu);
		QObject::connect(&m_portableVisualizationExport, &SynGlyphX::PortableVisualizationExport::CreatePortableVisualization, this, &GlyphViewerWindow::CreatePortableVisualization);

		for (auto action : portableVisualizationMenu->actions()) {

			m_loadedVisualizationDependentActions.push_back(action);
		}
	}
}

void GlyphViewerWindow::CreatePortableVisualization(SynGlyphX::PortableVisualizationExport::Platform platform) {

	QSet<QString> projectFileDirs;
	projectFileDirs.insert(QDir::toNativeSeparators(m_currentFilename));
	for (const auto& fileDatasource : m_mappingModel->GetDataMapping()->GetDatasources().GetFileDatasources()) {

		projectFileDirs.insert(QDir::toNativeSeparators(QString::fromStdWString(fileDatasource.second.GetFilename())));
	}

	QString csvDirectory = QDir::toNativeSeparators(GetExistingEmptyDirectory(projectFileDirs, "PortableVisualizationExportDir", tr("Select Directory For Portable Visualization"), "", tr("Selected directory contains one or more files relevant to the project.")));
	if (csvDirectory.isEmpty()) {

		return;
	}


	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	try {

		m_portableVisualizationExport.CopyContentsOfSourceDirectory(platform, csvDirectory);
		DataEngine::GlyphEngine ge;
		std::string baseImageDir = SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation().toStdString();
		std::string baseFilename = (QString::fromStdWString(SynGlyphX::DefaultBaseImageProperties::GetBasefilename()).toStdString());

		//App says "DataMapper" because this is equivalent to create portable visualization in DataMapper
		ge.initiate(dec.getEnv(), m_currentFilename.toStdString(), csvDirectory.toStdString() + "\\", baseImageDir, baseFilename, "DataMapper");
		ge.getDownloadedBaseImage(m_mappingModel->GetDataMapping().get()->GetBaseObjects());
		ge.generateGlyphs();

		SynGlyphX::Application::restoreOverrideCursor();

		const QString& GlyphEngineError = ge.getError();

		if (!GlyphEngineError.isNull()) {

			QMessageBox::information(this, "Transformation Error", GlyphEngineError, QMessageBox::Ok);
		}
	}
	catch (const std::exception& e) {

		try {

			SynGlyphX::Filesystem::RemoveContentsOfDirectory(csvDirectory.toStdString());
			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::critical(this, tr("Create Portable Visualization Error"), e.what());
		}
		catch (...) {

			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::information(this, tr("Directory in use"), tr("Could not create portable visualization because files in this directory are currently in use."), QMessageBox::Ok);
		}
		return;
	}

	statusBar()->showMessage("Portable visualization successfully created", 6000);
}