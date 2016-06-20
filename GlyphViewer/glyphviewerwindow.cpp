#include "glyphviewerwindow.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStackedWidget>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
#include <QtWidgets/QDockWidget>
#include <QtCore/QDateTime>
#include <QtWidgets/QFileDialog>
#include "glyphbuilderapplication.h"
#include "datatransformmapping.h"
#include "downloadoptionsdialog.h"
#include "data/npmapfile.h"
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
#include "downloadexception.h"
#include "loadingscreenwidget.h"
#include "remapdialog.h"
#include "baseimage.h"

GlyphViewerWindow::GlyphViewerWindow(QWidget *parent)
	: SynGlyphX::MainWindow(4, parent),
	m_glyph3DView(nullptr),
	m_showErrorFromTransform(true),
	m_dataEngineConnection(nullptr)
{
	m_dataEngineConnection = std::make_shared<DataEngine::DataEngineConnection>();
	m_mappingModel = new DataMappingLoadingFilterModel(this);
	m_mappingModel->SetDataEngineConnection(m_dataEngineConnection);
	m_sourceDataCache = std::make_shared<SourceDataCache>();
	m_glyphForestModel = new SynGlyphXANTz::GlyphForestModel(this);

	m_glyphForestSelectionModel = new SynGlyphX::ItemFocusSelectionModel(m_glyphForestModel, this);
	m_filteringManager = new FilteringManager(m_mappingModel, m_sourceDataCache, m_glyphForestSelectionModel, this);

	m_columnsModel = new SourceDataInfoModel(m_mappingModel->GetDataMapping(), m_sourceDataCache, this);
	m_columnsModel->SetSelectable(false, false, true);

	m_showHideToolbar = addToolBar(tr("Show/Hide Toolbar"));
	m_showHideToolbar->setFloatable(true);
	m_showHideToolbar->setMovable(true);

	CreateMenus();
	CreateDockWidgets();

	QStackedWidget* antzWidgetContainer = new QStackedWidget(this);
	antzWidgetContainer->setContentsMargins(0, 0, 0, 0);
	antzWidgetContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setCentralWidget(antzWidgetContainer);

	CreateLoadingScreen();
	
	try {
		
		CreateANTzWidget();
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("3D view error"), tr("3D view failed to create: ") + e.what());
		throw;
	}

	try {

		m_cacheManager.SetBaseCacheDirectory(GlyphViewerOptions::GetDefaultCacheDirectory().toStdWString());
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, tr("Cache Directory Error"), tr("Cache Directory Error: ") + e.what());
		throw;
	}

	m_linkedWidgetsManager = new LinkedWidgetsManager(m_glyph3DView, this);
	m_filteringWidget->SetupLinkedWidgets(*m_linkedWidgetsManager);
	m_pseudoTimeFilterWidget->SetupLinkedWidgets(*m_linkedWidgetsManager);

	CreateInteractionToolbar();

	//SynGlyphX::Transformer::SetDefaultImagesDirectory(SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation());
	//SynGlyphXANTz::ANTzCSVWriter::GetInstance().SetNOURLLocation(L"");

	try {

		if (!m_dataEngineConnection->hasJVM()){
			
			m_dataEngineConnection->createJVM();
			de_version = m_dataEngineConnection->VersionNumber();
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("JVM Error"), tr(e.what()));
		return;
	}

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		antzWidgetContainer->setCurrentIndex(0);
	}

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

GlyphViewerWindow::~GlyphViewerWindow()
{

}

void GlyphViewerWindow::closeJVM(){
	
	m_dataEngineConnection->destroyJVM();
}

void GlyphViewerWindow::CreateLoadingScreen() {

	QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		LoadingScreenWidget* loadingScreen = new LoadingScreenWidget(this, antzWidgetContainer);
		antzWidgetContainer->addWidget(loadingScreen);
	}
	else {

		QWidget* dummyWidget = new QWidget(antzWidgetContainer);
		dummyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		antzWidgetContainer->addWidget(dummyWidget);
	}
}

void GlyphViewerWindow::CreateANTzWidget() {
	
	QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());
	if (m_glyph3DView != nullptr) {
		
		antzWidgetContainer->removeWidget(m_glyph3DView);
		delete m_glyph3DView;
		m_glyph3DView = nullptr;
	}

	m_glyph3DView = new Glyph3DView(m_glyphForestModel, m_glyphForestSelectionModel, this);
	m_glyph3DView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_glyph3DView->addActions(m_treeView->GetSharedActions());
	m_glyph3DView->addAction(SynGlyphX::SharedActionList::CreateSeparator(m_glyph3DView));
	m_glyph3DView->addAction(m_showTagsAction);
	m_glyph3DView->addAction(m_hideTagsAction);
	m_glyph3DView->addAction(m_hideAllTagsAction);
	m_glyph3DView->addAction(SynGlyphX::SharedActionList::CreateSeparator(m_glyph3DView));
	m_glyph3DView->addAction(m_clearSelectionAction);

	antzWidgetContainer->addWidget(m_glyph3DView);
	antzWidgetContainer->setCurrentWidget(m_glyph3DView);

	QObject::connect(m_resetCameraToDefaultPosition, &QAction::triggered, m_glyph3DView, &Glyph3DView::ResetCamera);
	QObject::connect(m_showAnimation, &QAction::toggled, m_glyph3DView, &SynGlyphXANTz::ANTzForestWidget::ShowAnimatedRotations);
	QObject::connect(m_showTagsAction, &QAction::triggered, this, [this]{ m_glyph3DView->SetShowTagsOfSelectedObjects(true); });
	QObject::connect(m_hideTagsAction, &QAction::triggered, this, [this]{ m_glyph3DView->SetShowTagsOfSelectedObjects(false); });
	QObject::connect(m_hideAllTagsAction, &QAction::triggered, m_glyph3DView, &SynGlyphXANTz::ANTzForestWidget::ClearAllTags);
	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, m_glyph3DView, &SynGlyphXANTz::ANTzForestWidget::SetFilteredResults);
	QObject::connect(m_glyph3DView, &Glyph3DView::StereoSetup, this, &GlyphViewerWindow::OnStereoSetup);
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

	m_showTagsAction = CreateMenuAction(m_viewMenu, tr("Show Tags For Selected Objects"), Qt::Key_I);

	m_hideTagsAction = CreateMenuAction(m_viewMenu, tr("Hide Tags For Selected Objects"), Qt::Key_O);

	m_hideAllTagsAction = CreateMenuAction(m_viewMenu, tr("Hide All Visible Tags"));
	m_loadedVisualizationDependentActions.push_back(m_hideAllTagsAction);

	m_viewMenu->addSeparator();

	m_clearSelectionAction = CreateMenuAction(m_viewMenu, tr("Clear Selection"));
	m_clearSelectionAction->setEnabled(false);
	QObject::connect(m_clearSelectionAction, &QAction::triggered, m_glyphForestSelectionModel, &SynGlyphX::ItemFocusSelectionModel::ClearAll);

	m_viewMenu->addSeparator();

	m_resetCameraToDefaultPosition = CreateMenuAction(m_viewMenu, tr("Reset Camera To Starting Position"));
	m_loadedVisualizationDependentActions.push_back(m_resetCameraToDefaultPosition);

	m_viewMenu->addSeparator();

	m_toolbarsSubMenu = m_viewMenu->addMenu("Toolbar");
	m_toolbarsSubMenu->addAction(m_showHideToolbar->toggleViewAction());

	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	m_remapRootPositionMappingsAction = m_toolsMenu->addAction(tr("Change X, Y, Z Position Axes"));
	QObject::connect(m_remapRootPositionMappingsAction, &QAction::triggered, this, &GlyphViewerWindow::RemapRootPositionMappings);
	m_loadedVisualizationDependentActions.push_back(m_remapRootPositionMappingsAction);

	m_toolsMenu->addSeparator();

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

	m_legendsWidget = new LegendsDisplayWidget(this);
	m_legendsDockWidget = new QDockWidget(tr("Legends"), this);
	m_legendsDockWidget->setAllowedAreas(Qt::NoDockWidgetArea);
	m_legendsDockWidget->setWidget(m_legendsWidget);
	m_legendsDockWidget->setFloating(true);
	m_showHideToolbar->setIconSize(QSize(42, 32));
	QIcon icon;
	icon.addFile(":SGXGUI/Resources/Icons/icon-legend.png", QSize(), QIcon::Normal, QIcon::Off);
	icon.addFile(":SGXGUI/Resources/Icons/icon-legend-a.png", QSize(), QIcon::Normal, QIcon::On);
	QAction* act = m_legendsDockWidget->toggleViewAction();
	//act->setIconVisibleInMenu(false);
	act->setIcon(icon);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);
	m_legendsDockWidget->move(100, 100);
	m_legendsDockWidget->resize(400, 280);
	m_legendsDockWidget->hide();

	m_glyphListDockWidget = new QDockWidget(tr("Glyph List"), this);
	m_treeView = new GlyphTreeListView(m_glyphListDockWidget);
	m_treeView->setModel(m_glyphForestModel);
	m_treeView->SetItemFocusSelectionModel(m_glyphForestSelectionModel);

	m_glyphListDockWidget->setWidget(m_treeView);
	addDockWidget(Qt::LeftDockWidgetArea, m_glyphListDockWidget);
	act = m_glyphListDockWidget->toggleViewAction();
	icon.addFile(":SGXGUI/Resources/Icons/icon-list.png", QSize(), QIcon::Normal, QIcon::Off);
	icon.addFile(":SGXGUI/Resources/Icons/icon-list-a.png", QSize(), QIcon::Normal, QIcon::On);
	act->setIcon(icon);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);
	m_glyphListDockWidget->hide();

	m_glyphPropertiesWidgetContainer = new GlyphPropertiesWidgetsContainer(m_glyphForestModel, m_glyphForestSelectionModel, this);

	QDockWidget* textPropertiesDockWidget = new QDockWidget(tr("Text Properties"), this);
	textPropertiesDockWidget->setWidget(m_glyphPropertiesWidgetContainer->GetTextProperitesWidget());
	addDockWidget(Qt::LeftDockWidgetArea, textPropertiesDockWidget);
	act = textPropertiesDockWidget->toggleViewAction();
	icon.addFile(":SGXGUI/Resources/Icons/icon-text.png", QSize(), QIcon::Normal, QIcon::Off);
	icon.addFile(":SGXGUI/Resources/Icons/icon-text-a.png", QSize(), QIcon::Normal, QIcon::On);
	act->setIcon(icon);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);
	m_showHideToolbar->addAction(textPropertiesDockWidget->toggleViewAction());
	textPropertiesDockWidget->hide();

	tabifyDockWidget(m_glyphListDockWidget, textPropertiesDockWidget);

	QDockWidget* rightDockWidget = new QDockWidget(tr("Filtering"), this);
	m_filteringWidget = new FilteringWidget(m_columnsModel, m_filteringManager, rightDockWidget);
	rightDockWidget->setWidget(m_filteringWidget);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
	act = rightDockWidget->toggleViewAction();
	icon.addFile(":SGXGUI/Resources/Icons/icon-filter.png", QSize(), QIcon::Normal, QIcon::Off);
	icon.addFile(":SGXGUI/Resources/Icons/icon-filter-a.png", QSize(), QIcon::Normal, QIcon::On);
	act->setIcon(icon);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);

	QObject::connect(m_filteringWidget, &FilteringWidget::LoadSubsetVisualization, this, [this](QString filename){ LoadNewVisualization(filename); }, Qt::QueuedConnection);

	QDockWidget* bottomDockWidget = new QDockWidget(tr("Time Animated Filter"), this);
	m_pseudoTimeFilterWidget = new PseudoTimeFilterWidget(m_columnsModel, m_filteringManager, bottomDockWidget);
	bottomDockWidget->setWidget(m_pseudoTimeFilterWidget);
	addDockWidget(Qt::BottomDockWidgetArea, bottomDockWidget);
	act = bottomDockWidget->toggleViewAction();
	icon.addFile(":SGXGUI/Resources/Icons/icon-filter-time.png", QSize(), QIcon::Normal, QIcon::Off);
	icon.addFile(":SGXGUI/Resources/Icons/icon-filter-time-a.png", QSize(), QIcon::Normal, QIcon::On);
	act->setIcon(icon);
	m_viewMenu->addAction(act);
	m_showHideToolbar->addAction(act);

	QObject::connect(m_columnsModel, &SourceDataInfoModel::modelReset, m_pseudoTimeFilterWidget, &PseudoTimeFilterWidget::ResetForNewVisualization);
	bottomDockWidget->hide();}

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
			LoadVisualization(m_currentFilename, m_mappingModel->GetLoadingFilters());
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
	m_glyph3DView->SetBackgroundColor(SynGlyphX::GlyphColor::s_black);
	if (m_legendsDockWidget->isFloating()) {

		m_legendsDockWidget->hide();
	}

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());
		antzWidgetContainer->setCurrentIndex(0);
	}
}

void GlyphViewerWindow::ClearAllData() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	m_glyphForestSelectionModel->ClearAll();
	m_sourceDataCache->Close();
	m_glyphForestModel->Clear();
	m_mappingModel->ClearAndReset();
	m_legendsWidget->ClearLegends();
	m_filteringWidget->OnNewVisualization();
	SynGlyphX::Application::restoreOverrideCursor();
}

void GlyphViewerWindow::LoadVisualization(const QString& filename, const DataMappingLoadingFilterModel::Table2LoadingFiltersMap& filters) {

	QFileInfo fileInfo(filename);
	QString extension = fileInfo.suffix().toLower();
	if ((extension != "sdt") && (extension != "sav")) {

		throw std::runtime_error("File is not a recognized format");
	}

	if (!fileInfo.exists()) {

		throw std::runtime_error("File does not exist");
	}

	if (m_glyphForestModel->rowCount() > 0) {

		ClearAllData();
		m_glyph3DView->update();
	}

	if (extension == "sdt") {

		LoadDataTransform(filename, filters);
	}
	else if (extension == "sav") {

		LoadANTzCompatibilityVisualization(filename);
	}

	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		QStackedWidget* antzWidgetContainer = dynamic_cast<QStackedWidget*>(centralWidget());
		antzWidgetContainer->setCurrentIndex(1);
	}

	if (m_legendsWidget->HasLegends()) {

		m_legendsDockWidget->show();
	}
	else if (m_legendsDockWidget->isFloating()) {

		m_legendsDockWidget->hide();
	}
}

bool GlyphViewerWindow::LoadNewVisualization(const QString& filename, const DataMappingLoadingFilterModel::Table2LoadingFiltersMap& filters) {

	if (filename == m_currentFilename) {
		
		return true;
	}

	try {

		LoadVisualization(filename, filters);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Open Visualization"), tr("Failed to open visualization.  Error: ") + e.what(), QMessageBox::Ok);
		return false;
	}

	SetCurrentFile(filename);
	EnableLoadedVisualizationDependentActions(true);
	m_columnsModel->Reset();
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

	if (mapping->GetDatasources().empty()) {

		throw std::runtime_error("Visualization has no datasources.");
	}

	if (mapping->GetGlyphGraphs().empty()) {

		throw std::runtime_error("Visualization has no glyph templates.");
	}

	if (!mapping->DoesAtLeastOneGlyphGraphHaveBindingsOnPosition()) {

		throw std::runtime_error("Visualization has no glyph templates with bindings on Position X, Position Y, or Position Z.");
	}

	bool wasDataTransformUpdated = false;

	std::vector<boost::uuids::uuid> missingFileDatasources = mapping->GetFileDatasourcesWithInvalidFiles(true);
	std::vector<unsigned int> missingLegends = mapping->GetLegendsWithInvalidFiles();
	std::vector<unsigned int> localBaseImageIndexes = mapping->GetFileBaseObjectsWithInvalidFiles();

	if (!localBaseImageIndexes.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateImageFiles(localBaseImageIndexes, filename, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::runtime_error("Visualization has missing base images that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	if (!missingLegends.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateLegendFiles(missingLegends, filename, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::runtime_error("Visualization has missing legends that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	if (!missingFileDatasources.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeDatasourceFileDialog::UpdateDatasourceFiles(missingFileDatasources, filename, mapping, m_dataEngineConnection, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		if (!wasDataTransformUpdated) {

			throw std::runtime_error("Visualization has missing data sources that need to be updated to their correct locations before the visualization can be loaded");
		}
	}

	/*if (wasDataTransformUpdated) {

		mapping->WriteToFile(filename.toStdString());
	}*/

	mapping->WriteToFile(filename.toStdString());
}

void GlyphViewerWindow::LoadDataTransform(const QString& filename, const DataMappingLoadingFilterModel::Table2LoadingFiltersMap& filters) {
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

		if (!filename.contains(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation(), Qt::CaseInsensitive)) {

			ValidateDataMappingFile(filename);
		}

		m_mappingModel->LoadDataTransformFile(filename, filters);
		std::string dcd = GlyphViewerOptions::GetDefaultCacheDirectory().toStdString();
		std::string cacheDirectoryPath = dcd + ("/cache_" + boost::uuids::to_string(m_mappingModel->GetDataMapping()->GetID()));

		//SynGlyphXANTz::GlyphViewerANTzTransformer transformer(QString::fromStdString(cacheDirectoryPath.string()));
		//transformer.Transform(*m_mappingModel->GetDataMapping());
		
		DataEngine::GlyphEngine ge;
		std::string dirPath = cacheDirectoryPath + "/";
		std::string baseImageDir = SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation().toStdString();
		ge.initiate(m_dataEngineConnection->getEnv(), filename.toStdString(), dirPath, baseImageDir, "", "GlyphViewer");
		if (ge.IsUpdateNeeded()){
			DownloadBaseImages(ge);
			ge.generateGlyphs();
		}
		std::vector<std::string> images = ge.getBaseImages();
		
		QStringList cacheFiles;
		QString localOutputDir = QString::fromStdString(dirPath + "antz/");
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
		
		m_sourceDataCache->Setup(cacheFiles[2]);

		//This must be done before LoadFilesIntoModel is called
		m_filteringWidget->OnNewVisualization();

		LoadFilesIntoModel(outputfiles, qList);
		m_glyphForestModel->SetTagNotToBeShownIn3d(QString::fromStdWString(m_mappingModel->GetDataMapping()->GetDefaults().GetDefaultTagValue()));
		m_glyph3DView->SetBackgroundColor(m_mappingModel->GetDataMapping()->GetSceneProperties().GetBackgroundColor());
		m_legendsWidget->SetLegends(m_mappingModel->GetDataMapping()->GetLegends());

		SynGlyphX::Application::restoreOverrideCursor();
	}
	catch (const std::exception& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		throw;
	}
}

void GlyphViewerWindow::LoadFilesIntoModel(const SynGlyphXANTz::ANTzCSVWriter::FilenameList& filesToLoad, const QStringList& baseImageFilenames) {

	m_glyphForestModel->LoadANTzVisualization(filesToLoad, baseImageFilenames);

	SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr rootGlyph = m_mappingModel->GetDataMapping()->GetGlyphGraphs().begin()->second;
	std::array<QString, 3> rootPositionFields;
	for (unsigned int i = 0; i < 3; ++i) {

		const SynGlyphX::InputBinding& posInputBinding = rootGlyph->GetRoot()->second.GetPosition()[i].GetBinding();
		SynGlyphX::HashID id = posInputBinding.GetInputFieldID();
		if (id != 0) {

			rootPositionFields[i] = QString::fromStdWString(rootGlyph->GetInputFields().at(id).GetField());
		}
	}
	m_glyphForestModel->SetRootPosXYZMappedFields(rootPositionFields);
}

void GlyphViewerWindow::ChangeMapDownloadSettings() {

	DownloadOptionsDialog dialog(this);
	dialog.exec();
}

void GlyphViewerWindow::ShowOpenGLSettings() {

	const auto& format = m_glyph3DView->context()->format();
	QString settings = tr("OpenGL Version = ") + QString::number(format.majorVersion()) + "." + QString::number(format.minorVersion()) + '\n';
		
	settings += tr("Stereo Support") + " = ";
	if (m_glyph3DView->IsStereoSupported()) {

		settings += tr("enabled");
	}
	else {

		settings += tr("disabled");
	}
	QMessageBox::information(this, tr("OpenGL Settings"), settings);
}

void GlyphViewerWindow::ChangeStereoMode() {

	if (m_glyph3DView->IsStereoSupported()) {

		try {

			m_glyph3DView->SetStereoMode(!m_glyph3DView->IsInStereoMode());
		}
		catch (const std::exception& e) {

			QMessageBox::information(this, tr("Error"), tr("Stereo Error: ") + e.what());
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

	m_clearSelectionAction->setEnabled(false);
	m_showTagsAction->setEnabled(false);
	m_hideTagsAction->setEnabled(false);
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

		if (oldOptions.GetShowHUDAxisObject() != newOptions.GetShowHUDAxisObject()) {

			m_glyph3DView->SetShowHUDAxisInfoObject(newOptions.GetShowHUDAxisObject());
		}

		if (oldOptions.GetHUDAxisObjectLocation() != newOptions.GetHUDAxisObjectLocation()) {

			m_glyph3DView->SetAxisInfoObjectLocation(newOptions.GetHUDAxisObjectLocation());
		}

		if (oldOptions.GetShowSceneAxisObject() != newOptions.GetShowSceneAxisObject()) {

			m_glyph3DView->SetShowSceneAxisInfoObject(newOptions.GetShowSceneAxisObject());
		}

#ifdef USE_ZSPACE
		if (oldOptions.GetZSpaceOptions() != newOptions.GetZSpaceOptions()) {

			m_glyph3DView->SetZSpaceOptions(newOptions.GetZSpaceOptions());
		}
#endif

		if (oldOptions.GetHideUnselectedGlyphTrees() != newOptions.GetHideUnselectedGlyphTrees()) {

			m_linkedWidgetsManager->SetFilterView(newOptions.GetHideUnselectedGlyphTrees());
		}

		if (oldOptions.GetLoadSubsetVisualization() != newOptions.GetLoadSubsetVisualization()) {

			m_filteringWidget->SetLoadSubsetVisualization(newOptions.GetLoadSubsetVisualization());
		}

		if (oldOptions.GetLoadSubsetVisualizationInNewInstance() != newOptions.GetLoadSubsetVisualizationInNewInstance()) {

			m_filteringWidget->SetLoadSubsetVisualizationInNewInstance(newOptions.GetLoadSubsetVisualizationInNewInstance());
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

	options.ReadFromSettings();

	ChangeOptions(CollectOptions(), options);
}

void GlyphViewerWindow::WriteSettings() {

	SynGlyphX::MainWindow::WriteSettings();

	QSettings settings;

	settings.beginGroup("Display");
	settings.setValue("ShowAnimation", m_showAnimation->isChecked());
	settings.endGroup();

	GlyphViewerOptions options = CollectOptions();
	options.WriteToSettings();
}

GlyphViewerOptions GlyphViewerWindow::CollectOptions() {

	GlyphViewerOptions options;

	options.SetCacheDirectory(QString::fromStdWString(m_cacheManager.GetBaseCacheDirectory()));
	
	options.SetShowHUDAxisObject(m_glyph3DView->GetShowHUDAxisInfoObject());
	options.SetHUDAxisObjectLocation(m_glyph3DView->GetAxisInfoObjectLocation());
	options.SetShowSceneAxisObject(m_glyph3DView->GetShowSceneAxisInfoObject());
#ifdef USE_ZSPACE
	options.SetZSpaceOptions(m_glyph3DView->GetZSpaceOptions());
#endif

	options.SetHideUnselectedGlyphTrees(m_linkedWidgetsManager->GetFilterView());
	options.SetLoadSubsetVisualization(m_filteringWidget->GetLoadSubsetVisualization());
	options.SetLoadSubsetVisualizationInNewInstance(m_filteringWidget->GetLoadSubsetVisualizationInNewInstance());

	options.SetShowMessageWhenImagesDidNotDownload(m_showErrorFromTransform);

	return options;
}

void GlyphViewerWindow::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	bool selectionIsNotEmpty = !m_glyphForestSelectionModel->selectedIndexes().isEmpty();
	m_clearSelectionAction->setEnabled(selectionIsNotEmpty);
	m_showTagsAction->setEnabled(selectionIsNotEmpty);
	m_hideTagsAction->setEnabled(selectionIsNotEmpty);
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
	for (const auto& datasource : m_mappingModel->GetDataMapping()->GetDatasources()) {

		if (datasource.second->GetSourceType() == SynGlyphX::Datasource::SourceType::File) {

			SynGlyphX::FileDatasource::ConstSharedPtr fileDatasource = std::dynamic_pointer_cast<const SynGlyphX::FileDatasource>(datasource.second);
			projectFileDirs.insert(QDir::toNativeSeparators(QString::fromStdWString(fileDatasource->GetFilename())));
		}
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
		ge.initiate(m_dataEngineConnection->getEnv(), m_currentFilename.toStdString(), csvDirectory.toStdString() + "/", baseImageDir, baseFilename, "DataMapper");
		DownloadBaseImages(ge);
		ge.generateGlyphs();

		m_portableVisualizationExport.CopyLogo(QDir::toNativeSeparators(csvDirectory + "/usr/images/"));

		SynGlyphX::Application::restoreOverrideCursor();
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

void GlyphViewerWindow::DownloadBaseImages(DataEngine::GlyphEngine& ge) {

	try {

		ge.getDownloadedBaseImage(m_mappingModel->GetDataMapping().get()->GetBaseObjects());
	}
	catch (const DownloadException& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::information(this, "Download Image Error", tr("Base image failed to download so the world map was used instead.\n\nError: ") + tr(e.what()), QMessageBox::Ok);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	}
	catch (const std::exception& e) {

		throw;
	}
}

void GlyphViewerWindow::closeEvent(QCloseEvent* event) {

	m_filteringWidget->CloseSourceDataWidgets();

	SynGlyphX::MainWindow::closeEvent(event);
}

void GlyphViewerWindow::RemapRootPositionMappings() {

	QFileInfo currentFilenameInfo(m_currentFilename);

	RemapDialog remapDialog(m_mappingModel->GetDataMapping(), m_dataEngineConnection, this);
	remapDialog.SetSaveFilename(currentFilenameInfo.absolutePath() + "/" + currentFilenameInfo.completeBaseName() + "_remap.sdt");

	if (remapDialog.exec() == QDialog::Accepted) {

		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping = remapDialog.GetNewMapping();
		QString remapFilename = remapDialog.GetSaveFilename();
		dataTransformMapping->WriteToFile(remapFilename.toStdString());
		SynGlyphX::Application::restoreOverrideCursor();
		
		LoadNewVisualization(remapFilename);
	}
}

void GlyphViewerWindow::CreateInteractionToolbar() {

	m_interactionToolbar = addToolBar(tr("Interaction"));
	m_interactionToolbar->setFloatable(true);
	m_interactionToolbar->setMovable(true);

	m_interactionToolbar->addAction(m_remapRootPositionMappingsAction);

	m_interactionToolbar->addSeparator();

	QCheckBox* cb = new QCheckBox(tr("Hide Filtered"), this);

	m_interactionToolbar->addWidget(cb);
	m_linkedWidgetsManager->AddFilterViewCheckbox(cb);

	m_toolbarsSubMenu->addAction(m_interactionToolbar->toggleViewAction());
}

void GlyphViewerWindow::OnStereoSetup(bool stereoEnabled) {

	m_stereoAction->setChecked(stereoEnabled);
}
