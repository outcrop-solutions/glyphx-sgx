#include "datamapperwindow.h"
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDockWidget>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QFileDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "glyphbuilderapplication.h"
#include "downloadoptionsdialog.h"
#include "baseimagedialog.h"
#include "networkdownloader.h"
#include "filesystem.h"
//#include "antzexporttransformer.h"
#include "csvfilereader.h"
#include "csvtfilereaderwriter.h"
#include "singlewidgetdialog.h"
#include "newglyphtreewizard.h"
#include "newmappingdefaultswidget.h"
#include "singleglyphviewoptionswidget.h"
#include "changedatasourcefiledialog.h"
#include "changeimagefiledialog.h"
#include <fstream>
#include "glyphtemplatelibrarylistwidget.h"
#include "adddatabaseserverwizard.h"
#include "addfiledatasourcewizard.h"
#include "downloadexception.h"
#include "legenddialog.h"
#include "datatransformmodel.h"
#include "roledatafilterproxymodel.h"
#include "LinksListView.h"
#include "LinksDialog.h"
#include "QueryBuilderDialog.h"
#include "DMGlobal.h"
#include <QtWidgets/QUndoview>
#include <QtCore/QStandardPaths>
#include "FilterSetupWidget.h"
#include "ProjectEditDialog.h"
#include "ElasticListSetupDialog.h"
#include <QtWidgets/QVBoxLayout>
#include "datastatsmodel.h"
#include "inputfield.h"
#include "version.h"

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(0, parent),
    m_baseObjectsModel(nullptr),
	m_baseObjectsView(nullptr),
	m_glyphTreesView(nullptr),
	m_dataSourceStats(nullptr),
    m_dataSourcesView(nullptr),
	m_dataBindingWidget(nullptr),
	m_glyphRolesTableModel(nullptr),
    m_dataTransformModel(nullptr),
    m_minMaxGlyph3DWidget(nullptr),
	m_dataEngineConnection(nullptr)
{
	m_dataEngineConnection = std::make_shared<DataEngine::DataEngineConnection>();

	m_dataTransformModel = new SynGlyphX::DataTransformModel(this);
	QObject::connect(m_dataTransformModel, &SynGlyphX::DataTransformModel::dataChanged, this, [&, this](const QModelIndex& topLeft, const QModelIndex& bottomRight){ setWindowModified(true); });
	QObject::connect(m_dataTransformModel, &SynGlyphX::DataTransformModel::rowsInserted, this, [&, this](const QModelIndex& parent, int first, int last){ setWindowModified(true); });
	QObject::connect(m_dataTransformModel, &SynGlyphX::DataTransformModel::rowsRemoved, this, [&, this](const QModelIndex& parent, int first, int last){ setWindowModified(true); });
	
	m_glyphRolesTableModel = new GlyphRolesTableModel(m_dataTransformModel, this);

	CreateMenus();
    CreateDockWidgets();

	QObject::connect(m_glyphTreesView, &GlyphTreesView::SelectionChangedSourceModel, m_glyphRolesTableModel, &GlyphRolesTableModel::SetSelectedGlyphTreeIndexes);
	QObject::connect(m_glyphTreesView->GetClearSelectedInputBindingsAction(), &QAction::triggered, m_glyphRolesTableModel, &GlyphRolesTableModel::ClearInputBindings);

	CreateCenterWidget();

	ReadNewMappingDefaults();
	ClearAndInitializeDataMapping();

	m_linksDialog = new LinksDialog(m_dataTransformModel,  this);
	QObject::connect(m_linksDialog, &QDialog::accepted, this, &DataMapperWindow::OnLinkDialogAccepted);

	m_frontEndFiltersSetupDialog = new SynGlyphX::SingleWidgetDialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, new FilterSetupWidget(this), this);
	m_frontEndFiltersSetupDialog->setWindowTitle(tr("Filter Setup"));
	QObject::connect(m_frontEndFiltersSetupDialog, &QDialog::accepted, this, &DataMapperWindow::OnFrontEndFiltersDialogAccepted);

	//Setup data transform
	//SynGlyphXANTz::ANTzExportTransformer::SetLogoFilename(SynGlyphX::GlyphBuilderApplication::applicationDirPath() + QDir::separator() + "logo.png");
	//SynGlyphX::Transformer::SetDefaultImagesDirectory(SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation());

	QObject::connect(m_baseObjectsModel, &SynGlyphX::RoleDataFilterProxyModel::dataChanged, m_dataBindingWidget, &DataBindingTablesWidget::OnBaseObjectChanged);

	try {

		if (!m_dataEngineConnection->hasJVM()){

			m_dataEngineConnection->createJVM();
			m_dataTransformModel->SetDataEngineConnection(m_dataEngineConnection);
			m_dataSourceStats->SetDataEngineConnection(m_dataEngineConnection);
			m_dataEngineConnection->UserAccessControls()->SetAppVersionNumber(SynGlyphX::getAppVersionString());
			m_dataEngineConnection->SetGlyphEdPath(QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/Content/"));

		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("JVM Error"), tr(e.what()));
		throw;
	}

	DMGlobal::Init(new DMServices(this));

	QStringList commandLineArguments = SynGlyphX::Application::arguments();
	if (commandLineArguments.size() > 1) {

		QDir dataTransformToLoad(commandLineArguments[1]);
		LoadDataTransform(QDir::toNativeSeparators(dataTransformToLoad.canonicalPath()));
	}

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);

	loginDialog = new QDialog(this);
	loginDialog->setWindowFlags(((loginDialog->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowContextHelpButtonHint));
	QVBoxLayout* layout = new QVBoxLayout(loginDialog);
	loginWidget = new DataEngine::UserLoginDialog(m_dataEngineConnection, this);
	loginWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
	loginWidget->setMinimumWidth(400);
	loginWidget->setLineWidth(2);
	loginWidget->setMidLineWidth(3);
	loginWidget->setStyleSheet("background-color: white;");
	QObject::connect(loginWidget, &DataEngine::UserLoginDialog::LoginActivated, this, &DataMapperWindow::Login);
	loginWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	layout->addWidget(loginWidget);
	loginDialog->setLayout(layout);

	QObject::connect(loginDialog, &QDialog::rejected, this, &QWidget::close);

	if (IsUserLoggedIn()){
		Login();
	}
	else{
		QTimer::singleShot(0, loginDialog, SLOT(exec()));
	}

}

DataMapperWindow::~DataMapperWindow()
{
	
}

void DataMapperWindow::closeJVM(){
	
	m_dataEngineConnection->destroyJVM();
}

void DataMapperWindow::CreateCenterWidget() {
	
	m_minMaxGlyph3DWidget = new DataMapping3DWidget(m_dataTransformModel, this);
	m_minMaxGlyph3DWidget->SetModelRDFP(dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(m_glyphTreesView->model()), m_glyphTreesView->selectionModel());
	m_minMaxGlyph3DWidget->SetAllowMultiselect(true);

	QList<QAction*> actions;
	actions.append(m_glyphTreesView->GetGlyphActions().getActions());
	actions.push_back(SynGlyphX::SharedActionList::CreateSeparator(this));
	actions.append(m_glyphTreesView->GetEditActions().getActions());
	m_minMaxGlyph3DWidget->AddActionsToMinMaxViews(actions);

	QObject::connect(m_showAnimation, &QAction::toggled, m_minMaxGlyph3DWidget, &DataMapping3DWidget::EnableAnimation);
	
	setCentralWidget(m_minMaxGlyph3DWidget);
}

void DataMapperWindow::CreateMenus() {

    //Create File Menu
    m_fileMenu = menuBar()->addMenu(tr("File"));

    QAction* newProjectAction = CreateMenuAction(m_fileMenu, tr("New Mapping"), QKeySequence::New);
    QObject::connect(newProjectAction, &QAction::triggered, this, &DataMapperWindow::CreateNewProject);

    QAction* openProjectAction = CreateMenuAction(m_fileMenu, tr("Open Mapping"), QKeySequence::Open);
    QObject::connect(openProjectAction, &QAction::triggered, this, &DataMapperWindow::OpenProject);

    QAction* saveProjectAction = CreateMenuAction(m_fileMenu, tr("Save Mapping"), QKeySequence::Save);
    QObject::connect(saveProjectAction, &QAction::triggered, this, &DataMapperWindow::SaveProject);
	m_projectDependentActions.push_back(saveProjectAction);

    QAction* saveAsProjectAction = CreateMenuAction(m_fileMenu, tr("Save As Mapping"), QKeySequence::SaveAs);
    QObject::connect(saveAsProjectAction, &QAction::triggered, this, &DataMapperWindow::SaveAsProject);
	m_projectDependentActions.push_back(saveAsProjectAction);

	m_fileMenu->addActions(m_recentFileActions);

    m_fileMenu->addSeparator();

    QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
    QObject::connect(exitAction, &QAction::triggered, this, &DataMapperWindow::close);

	CreateEditMenu();

    //Create Glyph Menu
    m_glyphMenu = menuBar()->addMenu(tr("Glyph"));

	QAction* glyphDefaultsAction = m_glyphMenu->addAction(tr("Defaults"));
	QObject::connect(glyphDefaultsAction, &QAction::triggered, this, &DataMapperWindow::ChangeGlyphDefaults);
	m_projectDependentActions.push_back(glyphDefaultsAction);

	m_glyphMenu->addSeparator();

	QAction* addGlyphTemplateAction = m_glyphMenu->addAction(tr("Add Glyph Templates"));
	QObject::connect(addGlyphTemplateAction, &QAction::triggered, this, &DataMapperWindow::AddGlyphTemplate);

	QAction* addGlyphTemplateFromLibraryAction = m_glyphMenu->addAction(tr("Add Glyph Templates From Library"));
	QObject::connect(addGlyphTemplateFromLibraryAction, &QAction::triggered, this, &DataMapperWindow::AddGlyphTemplatesFromLibrary);

	QAction* createGlyphTemplateAction = m_glyphMenu->addAction(tr("Create New Glyph Template"));
	QObject::connect(createGlyphTemplateAction, &QAction::triggered, this, &DataMapperWindow::CreateNewGlyphTree);

	m_glyphMenu->addSeparator();

	m_baseObjectMenu = menuBar()->addMenu(tr("Base Object"));

	QAction* scenePropertiesAction = m_baseObjectMenu->addAction(tr("Scene Properties"));
	QObject::connect(scenePropertiesAction, &QAction::triggered, this, &DataMapperWindow::ChangeSceneProperties);

	m_baseObjectMenu->addSeparator();

	QAction* addBaseObjectAction = m_baseObjectMenu->addAction(tr("Add Base Object"));
	QObject::connect(addBaseObjectAction, &QAction::triggered, this, &DataMapperWindow::AddBaseObject);

	m_baseObjectMenu->addSeparator();

	//Create Links Menue
	m_linksMenu = menuBar()->addMenu(tr("Links"));
	QAction* addLinkAction = m_linksMenu->addAction(tr("Add Link"));
	QObject::connect(addLinkAction, &QAction::triggered, this, [&, this](){ m_linksDialog->SetEditRow(-1); ShowLinkDialog(); });

	//Create Datasource Menu
	m_datasourceMenu = menuBar()->addMenu(tr("Data Source"));

	QAction* filterSetupAction = m_datasourceMenu->addAction(tr("Setup Filters"));
	QObject::connect(filterSetupAction, &QAction::triggered, this, &DataMapperWindow::ChangeFrontEndFilters);

	QAction* elasticSetupAction = m_datasourceMenu->addAction(tr("Setup Elastic Lists"));
	QObject::connect(elasticSetupAction, &QAction::triggered, this, &DataMapperWindow::ChangeElasticListFilters);

	m_datasourceMenu->addSeparator();

	QAction* addFileDatasourceAction = m_datasourceMenu->addAction(tr("Add File"));
	QObject::connect(addFileDatasourceAction, &QAction::triggered, this, &DataMapperWindow::AddFileDataSource);

	if (SynGlyphX::GlyphBuilderApplication::AreInternalSGXFeaturesEnabled()) {

		QAction* addDatabaseServerDatasourcesAction = m_datasourceMenu->addAction(tr("Add Database Server"));
		QObject::connect(addDatabaseServerDatasourcesAction, &QAction::triggered, this, &DataMapperWindow::AddDatabaseServerDatasources);

	}

	m_datasourceMenu->addSeparator();

	//Create Legend Menu
	m_legendMenu = menuBar()->addMenu(tr("Legend"));

	QAction* addLegendAction = m_legendMenu->addAction(tr("Add Legend"));
	QObject::connect(addLegendAction, &QAction::triggered, this, &DataMapperWindow::AddLegend);

	m_legendMenu->addSeparator();

    //Create View Menu
	CreateViewMenu();

	m_showAnimation = m_viewMenu->addAction(tr("Show Animation"));
	m_showAnimation->setCheckable(true);
	m_showAnimation->setChecked(true);

    m_viewMenu->addSeparator();

	//Create Tools Menu
	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* optionsAction = m_toolsMenu->addAction(tr("Options"));
	QObject::connect(optionsAction, &QAction::triggered, this, &DataMapperWindow::ChangeOptions);

	QAction* newMappingDefaultsAction = m_toolsMenu->addAction(tr("New Mapping Defaults"));
	QObject::connect(newMappingDefaultsAction, &QAction::triggered, this, &DataMapperWindow::ChangeNewMappingDefaults);

	m_toolsMenu->addSeparator();

	QAction* editProjectAction = m_toolsMenu->addAction(tr("Project Editor"));

	QObject::connect(editProjectAction, &QAction::triggered, [=](){
		ProjectEditDialog dlg;
		dlg.exec();

	}
		);


	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &DataMapperWindow::ChangeMapDownloadSettings);

	CreateHelpMenu();
    
#ifdef __APPLE__
    menuBar()->addSeparator();
#endif
    SynGlyphX::MainWindow::CreateLoginMenu();

	EnableProjectDependentActions(false);
}

void DataMapperWindow::CreateDockWidgets() {

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	QDockWidget* leftDockWidgetGlyphTrees = new QDockWidget(tr("Glyph Trees"), this);

	m_glyphTreesView = new GlyphTreesView(m_dataTransformModel, leftDockWidgetGlyphTrees);
	m_glyphMenu->addActions(m_glyphTreesView->GetGlyphActions().getActions());
	m_editMenu->addActions(m_glyphTreesView->GetEditActions().getActions());

	QObject::connect(m_glyphTreesView, &GlyphTreesView::UpdateStatusBar, statusBar(), &QStatusBar::showMessage);

    //Add Tree View to dock widget on left side
	leftDockWidgetGlyphTrees->setWidget(m_glyphTreesView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidgetGlyphTrees);
	m_viewMenu->addAction(leftDockWidgetGlyphTrees->toggleViewAction());

	QDockWidget* leftDockWidgetBaseObjects = new QDockWidget(tr("Grids/Base Images"), this);

	m_baseObjectsView = new BaseObjectListView(m_dataTransformModel, leftDockWidgetBaseObjects);
	m_baseObjectsModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
	m_baseObjectsModel->setSourceModel(m_dataTransformModel);
	m_baseObjectsModel->setFilterRole(SynGlyphX::DataTransformModel::DataTypeRole);
	m_baseObjectsModel->SetFilterData(SynGlyphX::DataTransformModel::DataType::BaseObjects);
	m_baseObjectsView->setModel(m_baseObjectsModel);
	m_baseObjectMenu->addActions(m_baseObjectsView->actions());

	//Add Base Objects View to dock widget on left side
	leftDockWidgetBaseObjects->setWidget(m_baseObjectsView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidgetBaseObjects);
	m_viewMenu->addAction(leftDockWidgetBaseObjects->toggleViewAction());

	QDockWidget* leftDockWidgetLegends = new QDockWidget(tr("Legends"), this);

	m_legendsView = new LegendListView(m_dataTransformModel, leftDockWidgetBaseObjects);
	m_legendsModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
	m_legendsModel->setSourceModel(m_dataTransformModel);
	m_legendsModel->setFilterRole(SynGlyphX::DataTransformModel::DataTypeRole);
	m_legendsModel->SetFilterData(SynGlyphX::DataTransformModel::DataType::Legends);
	m_legendsView->setModel(m_legendsModel);
	m_legendMenu->addActions(m_legendsView->actions());

	leftDockWidgetLegends->setWidget(m_legendsView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidgetLegends);
	m_viewMenu->addAction(leftDockWidgetLegends->toggleViewAction());

	QDockWidget* leftDockWidgetLinks = new QDockWidget(tr("Links"), this);

	m_linksView = new LinksListView(m_dataTransformModel, leftDockWidgetLinks);
	m_linksModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
	m_linksModel->setSourceModel(m_dataTransformModel);
	m_linksModel->setFilterRole(SynGlyphX::DataTransformModel::DataTypeRole);
	m_linksModel->SetFilterData(SynGlyphX::DataTransformModel::DataType::Links);
	m_linksView->setModel(m_linksModel);
	m_linksMenu->addActions(m_linksView->actions());

	//QObject::connect(m_dataTransformModel, &DataTransformModel::dataChanged, this, [&, this](const QModelIndex& topLeft, const QModelIndex& bottomRight){ setWindowModified(true); });

	QObject::connect(m_linksView, &LinksListView::editLink, this, [&, this](int row){ m_linksDialog->SetEditRow(row); ShowLinkDialog(); });

	//Add linksView to dock widget on left side
	leftDockWidgetLinks->setWidget(m_linksView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidgetLinks);
	m_viewMenu->addAction(leftDockWidgetLinks->toggleViewAction());


	QDockWidget* rightDockWidgetDataStats = new QDockWidget(tr("Data Stats"), this);
	m_dataSourceStats = new DataSourceStatsWidget(m_dataTransformModel, rightDockWidgetDataStats);

	rightDockWidgetDataStats->setWidget(m_dataSourceStats);
	m_dataSourceStats->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidgetDataStats);
	m_viewMenu->addAction(rightDockWidgetDataStats->toggleViewAction());

	QDockWidget* rightDockWidgetDataSources = new QDockWidget(tr("Data Sources"), this);
	m_dataSourcesView = new DataSourcesView(m_dataTransformModel, rightDockWidgetDataSources);
	m_datasourceMenu->addActions(m_dataSourcesView->GetSharedActions().getActions());

	rightDockWidgetDataSources->setWidget(m_dataSourcesView);
	m_dataSourcesView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidgetDataSources);
	m_viewMenu->addAction(rightDockWidgetDataSources->toggleViewAction());

	QDockWidget* rightDockWidgetAliases = new QDockWidget(tr("Aliases"), this);
	m_aliases = new AliasesWidget(rightDockWidgetAliases);

	rightDockWidgetAliases->setWidget(m_aliases);
	m_aliases->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidgetAliases);
	m_viewMenu->addAction(rightDockWidgetAliases->toggleViewAction());

	QDockWidget* topDockWidget = new QDockWidget(tr("Data Bindings"), this);
	m_dataBindingWidget = new DataBindingTablesWidget(m_glyphRolesTableModel, topDockWidget);
	topDockWidget->setWidget(m_dataBindingWidget);
	addDockWidget(Qt::TopDockWidgetArea, topDockWidget);
	m_viewMenu->addAction(topDockWidget->toggleViewAction());

	QDockWidget* rightDockWidgetUndo = new QDockWidget(tr("Undoable Actions "), this);
	m_undoView = new QUndoView(m_undoStack);
	rightDockWidgetUndo->setWidget(m_undoView);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidgetUndo);
	m_viewMenu->addAction(rightDockWidgetUndo->toggleViewAction());

}

void DataMapperWindow::CreateNewProject() {

	if (!AskUserToSave()) {
		return;
	}

	CloseProject();
}

void DataMapperWindow::CloseProject() {

	m_dataSourceStats->ClearTabs();
	ClearAndInitializeDataMapping();
	m_glyphRolesTableModel->Clear();

	EnableProjectDependentActions(false);
	ClearCurrentFile();
}

void DataMapperWindow::OpenProject() {

	if (!AskUserToSave()) {
		return;
	}

	QString openFile = GetFileNameOpenDialog("DataTransformDir", tr("Open Project"), "", "SynGlyphX Data Transform Project Files (*.sdt)");
	if (!openFile.isEmpty()) {
		LoadDataTransform(openFile);
	}
}

bool DataMapperWindow::SaveProject() {

	if (m_currentFilename.isEmpty()) {
		return SaveAsProject();
	}
	else {
		return SaveDataTransform(m_currentFilename);
	}
}

bool DataMapperWindow::SaveAsProject() {

	QString saveFile = GetFileNameSaveDialog("DataTransformDir", tr("Save Project"), "", "SynGlyphX Data Transform Project Files (*.sdt)");

	if (!saveFile.isEmpty()) {

		m_dataTransformModel->ResetDataMappingID();
		return SaveDataTransform(saveFile);
	}
	else {

		return false;
	}
}

bool DataMapperWindow::LoadRecentFile(const QString& filename) {

	if (AskUserToSave()) {

		return LoadDataTransform(filename);
	}

	return true;
}

//NEW LOGIN/LICENSE STUFF FOR DATAMAPPER

bool DataMapperWindow::IsUserLoggedIn() {

	QSettings settings;
	settings.beginGroup("LoggedInUser");
	QString user = settings.value("Username", "Guest").toString();
	QString pass = settings.value("Password", "").toString();
	QString name = settings.value("Name", "Guest").toString();
	QString inst = settings.value("Institution", "").toString();
	bool logged = settings.value("StayLogged", false).toBool();
	settings.endGroup();

	if (m_dataEngineConnection->UserAccessControls()->IsValidConnection()){
		return true;
	}
	else{
		//m_dataEngineConnection->UserAccessControls()->InitializeConnection();
		if (logged){
			int valid = m_dataEngineConnection->UserAccessControls()->ValidateCredentials(user, pass);
			if (valid == 1 || valid == 2){
				return true;
			}
		}
		return false;
	}
}

void DataMapperWindow::Login(){

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	if (loginWidget->Login()){
		MainWindow::UpdateUserMenu(m_dataEngineConnection->UserAccessControls()->NameOfUser());
		UpdateUserMenu();
		if (MainWindow::HasValidLicense()){
			loginDialog->hide();
		}
		else{
			QTimer::singleShot(0, this, SLOT(Logout()));
		}
	}
	else{
		QMessageBox critical_error(QMessageBox::Critical, tr("Failed To Login"), tr("Invalid username or password, please try again"), QMessageBox::Ok, this);
		critical_error.setDetailedText(m_dataEngineConnection->JavaErrors());
		critical_error.setStyleSheet("QLabel{margin-right:75px;},QTextEdit{min-width:500px;}");
		critical_error.setStandardButtons(QMessageBox::Ok);
		critical_error.setDefaultButton(QMessageBox::Ok);
		critical_error.setEscapeButton(QMessageBox::Ok);
		critical_error.exec();
	}
	SynGlyphX::Application::restoreOverrideCursor();
}

void DataMapperWindow::UpdateUserMenu(){
	QObject::connect(MainWindow::LogoutMenu(), &QAction::triggered, this, &DataMapperWindow::Logout);
}

void DataMapperWindow::Logout(){

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	MainWindow::UserLogOut();

	QSettings settings;
	settings.beginGroup("LoggedInUser");
	settings.setValue("Username", "Guest");
	settings.setValue("Password", "");
	settings.setValue("Name", "Guest");
	settings.setValue("Institution", "");
	settings.setValue("StayLogged", false);
	settings.endGroup();

	m_dataEngineConnection->UserAccessControls()->ResetConnection();

	SynGlyphX::Application::restoreOverrideCursor();
	IsUserLoggedIn();
	loginDialog->exec();
}

//NEW LOGIN/LICENSE STUFF FOR DATAMAPPER ENDS

void DataMapperWindow::UpdateMissingFiles(const QString& mappingFilename) {

	try {

		SynGlyphX::DataTransformMapping::SharedPtr mapping = std::make_shared<SynGlyphX::DataTransformMapping>();
		mapping->ReadFromFile(mappingFilename.toStdString());

		std::vector<boost::uuids::uuid> fileDatasourcesToBeUpdated = mapping->GetFileDatasourcesWithInvalidFiles(false);
		std::vector<unsigned int> missingBaseImages = mapping->GetFileBaseObjectsWithInvalidFiles();
		std::vector<unsigned int> missingLegends = mapping->GetLegendsWithInvalidFiles();

		bool wasDataTransformUpdated = false;

		if (!missingBaseImages.empty()) {

			SynGlyphX::Application::restoreOverrideCursor();
			wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateImageFiles(missingBaseImages, mappingFilename, mapping, this);
			if (!wasDataTransformUpdated) {

				throw std::runtime_error("One or more images weren't found.");
			}
			SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		}

		if (!missingLegends.empty()) {

			SynGlyphX::Application::restoreOverrideCursor();
			wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateLegendFiles(missingLegends, mappingFilename, mapping, this);
			if (!wasDataTransformUpdated) {

				throw std::runtime_error("One or more legends weren't found.");
			}
			SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		}

		if (!fileDatasourcesToBeUpdated.empty()) {

			SynGlyphX::Application::restoreOverrideCursor();
			wasDataTransformUpdated = SynGlyphX::ChangeDatasourceFileDialog::UpdateDatasourceFiles(fileDatasourcesToBeUpdated, mappingFilename, mapping, m_dataEngineConnection, this);
			if (!wasDataTransformUpdated) {

				throw std::runtime_error("One or more datasources weren't found.");
			}
			SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		}

		if (wasDataTransformUpdated) {

			mapping->WriteToFile(mappingFilename.toStdString());
		}
	}
	catch (const std::exception&) {

		throw;
	}
}

bool DataMapperWindow::LoadDataTransform(const QString& filename) {
	/*
	try {

		if (!dec.hasJVM()){
			dec.createJVM();
			m_dataTransformModel->SetDataEngineConn(&dec);
			m_dataSourceStats->SetDataEngineConn(&dec);
		}
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("JVM Error"), tr(e.what()));
		return false;
	}*/

	QFileInfo fileInfo(filename);
	if (fileInfo.suffix().toLower() != "sdt") {

		QMessageBox::warning(this, tr("Loading Data Transform Failed"), tr("File is not a recognized format"));
		return false;
	}
	if (!fileInfo.exists()) {

		QMessageBox::warning(this, tr("Loading Data Transform Failed"), tr("File does not exist"));
		return false;
	}
	try {

		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		UpdateMissingFiles(filename);

		CloseProject();

		QObject::disconnect(m_modelResetConnection);

		m_dataTransformModel->LoadDataTransformFile(filename, true);
		m_glyphTreesView->SelectLastGlyphTreeRoot();
		m_aliases->Refresh();
		SelectFirstBaseObject();

		m_modelResetConnection = QObject::connect(m_dataTransformModel, &SynGlyphX::DataTransformModel::modelReset, this, [&, this](){ setWindowModified(true); });
	}
	catch (const std::exception& e) {

		CloseProject();
		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox critical_error(QMessageBox::Critical, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok, this);
		critical_error.setDetailedText(m_dataEngineConnection->JavaErrors());
		critical_error.setStyleSheet("QLabel{margin-right:75px;},QTextEdit{min-width:500px;}");
		critical_error.setStandardButtons(QMessageBox::Ok);
		critical_error.setDefaultButton(QMessageBox::Ok);
		critical_error.setEscapeButton(QMessageBox::Ok);
		critical_error.exec();
		m_dataEngineConnection->ClearJavaErrors();
		//QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return false;
	}
	DMGlobal::Services()->ClearUndoStack();
	SetCurrentFile(filename);
	m_dataTransformModel->ClearAbsentBindings();
	EnableProjectDependentActions(true);
	SynGlyphX::Application::restoreOverrideCursor();
	m_linksDialog->Clear();
	statusBar()->showMessage("Project successfully opened", 3000);
	return true;
}

bool DataMapperWindow::SaveDataTransform(const QString& filename) {

	try {
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		m_dataBindingWidget->CommitChanges();
		m_dataTransformModel->SaveDataTransformFile(filename);
		SetCurrentFile(filename);
		setWindowModified(false);
		SynGlyphX::Application::restoreOverrideCursor();
		statusBar()->showMessage("Data transform successfully saved", 3000);
		return true;
	}
	catch (const std::exception& e) {

		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::critical(this, tr("Failed To Save Data Transformation"), e.what(), QMessageBox::Ok);
		return false;
	}
}

/*
void DataMapperWindow::ProcessCSVFile(const QString& csvFile) {

	//Check if .csvt file exists
	QString csvtFile = csvFile + 't';
	if (!QFile::exists(csvtFile)) {

		SynGlyphX::CSVFileReader csvFileReader(csvFile.toStdString());
		SynGlyphX::CSVFileReader::CSVValues fieldNames = csvFileReader.GetHeaders();

		if (fieldNames.empty()) {

			throw std::runtime_error("No headers found in CSV file");
		}

		QStringList fieldNameList;
		for (std::wstring fieldName : fieldNames) {

			fieldNameList.push_back(QString::fromStdWString(fieldName));
		}

		DatasourceFieldTypesDialog dialog(fieldNameList, "CSV", this);
		if (dialog.exec() == QDialog::Rejected) {

			throw std::runtime_error("CSV file does not have data types associated with its fields");
		}

		QStringList dialogTypes = dialog.GetFieldTypes();

		SynGlyphX::CSVFileReader::CSVValues types;
		for (const QString type : dialogTypes) {

			types.push_back(type.toStdWString());
		}

		SynGlyphX::CSVTFileReaderWriter::WriteCSVTFile(csvtFile.toStdString(), types);
	}
}*/

void DataMapperWindow::AddFileDataSource() {

	QSettings settings;
	settings.beginGroup(s_fileDialogSettingsGroup);
	QString initialDir = settings.value("DatasourcesDir", "").toString();
	if (initialDir.isEmpty()) {

		initialDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	}

	AddFileDatasourceWizard fileDatasourceWizard(initialDir, m_dataEngineConnection, this);
	if (fileDatasourceWizard.Exec() == QDialog::Accepted) {

		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

		const SynGlyphX::FileDatasource& fileDatasource = fileDatasourceWizard.GetFileDatasource();
		QFileInfo fileInfo(QString::fromStdWString(fileDatasource.GetFilename()));
		settings.setValue("DatasourcesDir", fileInfo.absolutePath());

		try {

			m_dataTransformModel->AddFileDatasource(fileDatasource);

			m_dataSourceStats->AddNewStatsViews();
			EnableProjectDependentActions(true);

		}
		catch (const std::exception& e) {

			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::critical(this, tr("Failed To Add Data Source"), QString::fromStdString(e.what()), QMessageBox::Ok);
		}
	}

	settings.endGroup();
	SynGlyphX::Application::restoreOverrideCursor();
	statusBar()->showMessage("Data source successfully added", 3000);

	if (m_dataEngineConnection->IsConnectionOpen()) {

		m_dataEngineConnection->closeConnection();
	}
}

void DataMapperWindow::AddDatabaseServerDatasources() {

	AddDatabaseServerWizard addDatabaseServerWizard(m_dataEngineConnection, this);
	if (addDatabaseServerWizard.exec() == QDialog::Accepted) {

		try {

			SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

			m_dataTransformModel->AddDatabaseServer(addDatabaseServerWizard.GetValues());
			m_dataSourceStats->AddNewStatsViews();
			EnableProjectDependentActions(true);

		}
		catch (const std::exception& e) {

			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::critical(this, tr("Failed To Add Data Source"), QString::fromStdString(e.what()), QMessageBox::Ok);
		}
	}

	SynGlyphX::Application::restoreOverrideCursor();
	statusBar()->showMessage("Data source successfully added", 3000);

	if (m_dataEngineConnection->IsConnectionOpen()) {

		m_dataEngineConnection->closeConnection();
	}
}

void DataMapperWindow::AddBaseObject() {

	BaseImageDialog dialog(true, false, this);
	dialog.setWindowTitle(tr("Add New Base Object"));
	if (dialog.exec() == QDialog::Accepted) {

		const SynGlyphX::BaseImage& baseImage = dialog.GetBaseImage();
		m_dataTransformModel->AddBaseObject(baseImage);
		EnableProjectDependentActions(true);
	}
}

void DataMapperWindow::ShowLinkDialog() {

	if (m_linksDialog->GetEditRow() >= 0) {
		m_linksDialog->setWindowTitle(tr("Edit Link"));
	}
	else
		m_linksDialog->setWindowTitle(tr("Add New Link"));
		
	m_linksDialog->show();
	m_linksDialog->raise();
	m_linksDialog->activateWindow();

}

void DataMapperWindow::OnLinkDialogAccepted() {
	if (m_linksDialog->GetEditRow() >= 0)
		m_dataTransformModel->SetLink(m_linksDialog->GetEditRow(), m_linksDialog->GetLink());
	else
		m_dataTransformModel->CreateAddLinkCommand(m_linksDialog->GetLink());
	EnableProjectDependentActions(true);
}


void DataMapperWindow::AddLegend() {

	SynGlyphX::LegendDialog dialog(this);
	dialog.setWindowTitle(tr("Add New Legend"));
	if (dialog.exec() == QDialog::Accepted) {

		const SynGlyphX::Legend& legend = dialog.GetLegend();
		m_dataTransformModel->AddLegend(legend);
		EnableProjectDependentActions(true);
	}
}

void DataMapperWindow::AddGlyphTemplate() {

	QStringList glyphTemplates = GetFileNamesOpenDialog("GlyphTemplatesDir", tr("Add Glyph Templates"), "", "SynGlyphX Glyph Template Files (*.sgt *.csv)");

	if (glyphTemplates.isEmpty()) {
		return;
	}

	try {
		for (const QString& filename : glyphTemplates) {
			m_dataTransformModel->AddGlyphFile(filename);
		}
	}
	catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Failed To Add Glyph"), e.what(), QMessageBox::Ok);
		return;
	}

	EnableProjectDependentActions(true);
	m_glyphTreesView->SelectLastGlyphTreeRoot();
	statusBar()->showMessage("Glyph Template(s) successfully added", 3000);
}

void DataMapperWindow::AddGlyphTemplatesFromLibrary() {

	SynGlyphX::GlyphTemplateLibraryListWidget* templateListView = new SynGlyphX::GlyphTemplateLibraryListWidget(true, this);	

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Open | QDialogButtonBox::StandardButton::Cancel, templateListView, this);
	dialog.setWindowTitle(tr("Add Glyph Templates From Library"));

	if (dialog.exec() == QDialog::Accepted) {

		QStringList templatesToLoad = templateListView->GetSelectedTemplates();

		if (templatesToLoad.isEmpty()) {
		
			return;
		}

		try {
		
			for (const QString& filename : templatesToLoad) {
		
				m_dataTransformModel->AddGlyphFile(filename);
			}
		}
		catch (const std::exception& e) {
		
			QMessageBox::critical(this, tr("Failed To Add Glyph"), e.what(), QMessageBox::Ok);
			return;
		}

		EnableProjectDependentActions(true);
		m_glyphTreesView->SelectLastGlyphTreeRoot();
		statusBar()->showMessage("Glyph Template(s) successfully added", 3000);
	}
}

void DataMapperWindow::CreateNewGlyphTree() {

	SynGlyphX::DataMappingGlyphGraph::SharedPtr newGlyphTree = SynGlyphX::NewGlyphTreeWizard::RunNewGlyphTreeWizard(this);
	if (newGlyphTree) {

		m_dataTransformModel->AddGlyphTree(newGlyphTree);
		EnableProjectDependentActions(true);
		m_glyphTreesView->SelectLastGlyphTreeRoot();
		statusBar()->showMessage("Glyph Template successfully added", 3000);
	}
}

bool DataMapperWindow::AskUserToSave() {

	if (isWindowModified()) {

		QMessageBox::StandardButton result = QMessageBox::warning(this, tr("Save Changes"), tr("The project has been modified.  Do you wish to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (result == QMessageBox::Save) {
			return SaveProject();
		}
		else if (result == QMessageBox::Cancel) {
			return false;
		}
	}

	return true;
}

void DataMapperWindow::closeEvent(QCloseEvent* event) {

	if (AskUserToSave()) {
		SynGlyphX::MainWindow::closeEvent(event);
	}
	else {
		event->ignore();
	}
}

void DataMapperWindow::EnableProjectDependentActions(bool enable) {

	for (int i = 0; i < m_projectDependentActions.length(); ++i) {
		m_projectDependentActions[i]->setEnabled(enable);
	}
}

void DataMapperWindow::ChangeMapDownloadSettings() {

	DownloadOptionsDialog dialog(this);
	dialog.exec();
}

void DataMapperWindow::ChangeGlyphDefaults() {

	const SynGlyphX::DataMappingDefaults& oldDefaults = m_dataTransformModel->GetDataMapping()->GetDefaults();
	SynGlyphX::GlyphDefaultsWidget* glyphDefaultsWidget = new SynGlyphX::GlyphDefaultsWidget(this);
	glyphDefaultsWidget->SetDefaults(oldDefaults);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, glyphDefaultsWidget, this);
	dialog.setWindowTitle(tr("Glyph Defaults"));

	if (dialog.exec() == QDialog::Accepted) {

		const SynGlyphX::DataMappingDefaults& newDefaults = glyphDefaultsWidget->GetDefaults();
		if (newDefaults != oldDefaults) {

			setWindowModified(true);
		}
		m_dataTransformModel->SetDefaults(newDefaults);
	}
}

void DataMapperWindow::ChangeNewMappingDefaults() {
	
	NewMappingDefaultsWidget* newMappingDefaultsWidget = new NewMappingDefaultsWidget(this);
	newMappingDefaultsWidget->SetDefaults(m_newMappingDefaults, m_newMappingSceneProperties);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, newMappingDefaultsWidget, this);
	dialog.setWindowTitle(tr("New Mapping Defaults"));

	if (dialog.exec() == QDialog::Accepted) {

		m_newMappingDefaults = newMappingDefaultsWidget->GetDataMappingDefaults();
		m_newMappingSceneProperties = newMappingDefaultsWidget->GetScenePropertiesDefaults();
		WriteNewMappingDefaults();
		if (!m_projectDependentActions[0]->isEnabled()) {

			m_dataTransformModel->SetDefaults(m_newMappingDefaults);
			m_dataTransformModel->SetSceneProperties(m_newMappingSceneProperties);
		}
	}
}

void DataMapperWindow::ReadNewMappingDefaults() {

	QSettings settings;
	settings.beginGroup("Defaults");

	std::wistringstream xmlGlyphDefaults(settings.value("glyph", "").toString().toStdWString());
	if (!xmlGlyphDefaults.str().empty()) {

		boost::property_tree::wptree propertyTree;
		boost::property_tree::read_xml(xmlGlyphDefaults, propertyTree, SynGlyphX::XMLPropertyTreeFile::GetReadFlags());
		boost::optional<boost::property_tree::wptree&> defaultsPropertyTree = propertyTree.get_child_optional(SynGlyphX::DataMappingDefaults::s_propertyTreeName);
		if (defaultsPropertyTree.is_initialized()) {

			m_newMappingDefaults = SynGlyphX::DataMappingDefaults(defaultsPropertyTree.get());
		}
	}

	std::wistringstream xmlScenePropertiesDefaults(settings.value("SceneProperties", "").toString().toStdWString());
	if (!xmlScenePropertiesDefaults.str().empty()) {

		boost::property_tree::wptree propertyTree;
		boost::property_tree::read_xml(xmlScenePropertiesDefaults, propertyTree, SynGlyphX::XMLPropertyTreeFile::GetReadFlags());
		boost::optional<boost::property_tree::wptree&> scenePropertiesTree = propertyTree.get_child_optional(SynGlyphX::SceneProperties::s_propertyTreeName);
		if (scenePropertiesTree.is_initialized()) {

			m_newMappingSceneProperties = SynGlyphX::SceneProperties(scenePropertiesTree.get());
		}
	}

	settings.endGroup();
}

void DataMapperWindow::WriteNewMappingDefaults() {

	boost::property_tree::wptree glyphDefaultsPropertyTree;
	m_newMappingDefaults.ExportToPropertyTree(glyphDefaultsPropertyTree);

	std::wostringstream xmlWriteGlyphDefaults;
	boost::property_tree::write_xml(xmlWriteGlyphDefaults, glyphDefaultsPropertyTree, SynGlyphX::XMLPropertyTreeFile::GetWriteSettings());

	boost::property_tree::wptree scenePropertiesPropertyTree;
	m_newMappingSceneProperties.ExportToPropertyTree(scenePropertiesPropertyTree);

	std::wostringstream xmlWriteSceneProperties;
	boost::property_tree::write_xml(xmlWriteSceneProperties, scenePropertiesPropertyTree, SynGlyphX::XMLPropertyTreeFile::GetWriteSettings());

	QSettings settings;
	settings.beginGroup("Defaults");
	settings.setValue("glyph", QString::fromStdWString(xmlWriteGlyphDefaults.str()));
	settings.setValue("SceneProperties", QString::fromStdWString(xmlWriteSceneProperties.str()));
	settings.endGroup();
}

void DataMapperWindow::ClearAndInitializeDataMapping() {

	m_dataTransformModel->ClearAndReset();
	m_dataTransformModel->SetDefaults(m_newMappingDefaults);
	m_dataTransformModel->SetSceneProperties(m_newMappingSceneProperties);
	SelectFirstBaseObject();
}

void DataMapperWindow::SelectFirstBaseObject() {

	m_baseObjectsView->selectionModel()->select(m_baseObjectsModel->index(0, 0), QItemSelectionModel::ClearAndSelect);
}

void DataMapperWindow::ReadSettings(){

	SynGlyphX::MainWindow::ReadSettings();

	QSettings settings;
	settings.beginGroup("Display");
	if (!settings.value("show", true).toBool()) {

		m_showAnimation->toggle();
	}
	m_minMaxGlyph3DWidget->SetBaseImage(SynGlyphX::DefaultBaseImageProperties::s_typeStrings.right.at(settings.value("baseImage", "Clear").toString().toStdWString()));
	settings.endGroup();
}

void DataMapperWindow::WriteSettings() {

	SynGlyphX::MainWindow::WriteSettings();
	QSettings settings;
	settings.beginGroup("Display");
	settings.setValue("show", m_showAnimation->isChecked());
	settings.setValue("baseImage", QString::fromStdWString(SynGlyphX::DefaultBaseImageProperties::s_typeStrings.left.at(m_minMaxGlyph3DWidget->GetBaseImage())));
	settings.endGroup();
}

void DataMapperWindow::ChangeSceneProperties() {

	const SynGlyphX::SceneProperties& oldSceneProperties = m_dataTransformModel->GetDataMapping()->GetSceneProperties();
	SynGlyphX::ScenePropertiesWidget* scenePropertiesWidget = new SynGlyphX::ScenePropertiesWidget(this);
	scenePropertiesWidget->SetWidgetFromProperties(oldSceneProperties);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, scenePropertiesWidget, this);
	dialog.setWindowTitle(tr("Scene Propeties"));

	if (dialog.exec() == QDialog::Accepted) {

		const SynGlyphX::SceneProperties& newSceneProperties = scenePropertiesWidget->GetPropertiesFromWidget();
		if (newSceneProperties != oldSceneProperties) {

			setWindowModified(true);
		}
		m_dataTransformModel->SetSceneProperties(newSceneProperties);
	}
}

void DataMapperWindow::ChangeOptions() {

	SynGlyphX::SingleGlyphViewOptionsWidget* optionsWidget = new SynGlyphX::SingleGlyphViewOptionsWidget(this);
	optionsWidget->SetDefaultBaseImage(m_minMaxGlyph3DWidget->GetBaseImage());
	optionsWidget->SetZToAlwaysZeroIn3D(m_minMaxGlyph3DWidget->GetLockZPositionToZero());

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, optionsWidget, this);
	dialog.setWindowTitle(tr("Options"));
	if (dialog.exec() == QDialog::Accepted) {

		m_minMaxGlyph3DWidget->SetBaseImage(optionsWidget->GetDefaultBaseImage());
		m_minMaxGlyph3DWidget->SetLockZPositionToZero(optionsWidget->IsZAlwaysZeroIn3D());
	}
}

void DataMapperWindow::ChangeFrontEndFilters() {

	FilterSetupWidget* filterSetupWidget = dynamic_cast<FilterSetupWidget*>(m_frontEndFiltersSetupDialog->GetWidget());
	filterSetupWidget->SetFilters(m_dataTransformModel->GetFrontEndFilters());
	
	m_frontEndFiltersSetupDialog->show();
	m_frontEndFiltersSetupDialog->raise();
	m_frontEndFiltersSetupDialog->activateWindow();
}

void DataMapperWindow::OnFrontEndFiltersDialogAccepted() {

	FilterSetupWidget* filterSetupWidget = dynamic_cast<FilterSetupWidget*>(m_frontEndFiltersSetupDialog->GetWidget());
	m_dataTransformModel->SetFrontEndFiltersUndoRedo(filterSetupWidget->GetFilters());
	DMGlobal::Services()->SetModified();
}

void DataMapperWindow::ChangeElasticListFilters() {

	std::map<std::wstring, std::vector<std::wstring>> elasticFields = m_dataTransformModel->GetDataMapping()->GetElasticListFields();
	std::map<std::wstring, std::vector<std::wstring>> availableFields;
	std::map<std::wstring, std::wstring> formattedNames;
	
	for (const auto& table : m_dataTransformModel->GetTableStatsMap()){

		std::wstring formattedName = m_dataTransformModel->GetDataMapping()->GetFormattedName(table.first.GetDatasourceID(), table.first.GetTable());
		std::wstring tableName = boost::lexical_cast<std::wstring>(table.first.GetDatasourceID()) + L":" + table.first.GetTable();
		formattedNames[tableName] = formattedName;
		std::vector<std::wstring> fields;
		for (const auto& field : table.second){
			fields.push_back(field.at(0).toStdWString());
		}
		availableFields[tableName] = fields;
	}
	
	SynGlyphX::ElasticListSetupDialog elsd(this);
	elsd.SetFormattedNames(formattedNames);
	elsd.PopulateTabs(elasticFields, availableFields);

	if (elsd.exec() == QDialog::Accepted) {
		m_dataTransformModel->SaveElasticListFields(elsd.SaveElasticListSelections());
	}
}
