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
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "glyphbuilderapplication.h"
#include "sourcedatamanager.h"
#include "downloadoptionsdialog.h"
#include "baseimagedialog.h"
#include "networkdownloader.h"
#include "filesystem.h"
#include "antzexporttransformer.h"
#include "datasourcefieldtypesdialog.h"
#include "csvfilereader.h"
#include "csvtfilereaderwriter.h"
#include "singlewidgetdialog.h"
#include "newglyphtreewizard.h"
#include "databaseinfo.h"
#include "newmappingdefaultswidget.h"
#include "singleglyphviewoptionswidget.h"
#include "changedatasourcefiledialog.h"
#include "changeimagefiledialog.h"
#include <fstream>

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(0, parent),
	m_baseObjectsView(nullptr),
	m_glyphTreesView(nullptr),
	m_dataSourceStats(nullptr),
	m_dataBindingWidget(nullptr),
	m_glyphRolesTableModel(nullptr),
	m_dataTransformModel(nullptr),
	m_minMaxGlyph3DWidget(nullptr),
	m_baseObjectsModel(nullptr),
	m_dataSourcesView(nullptr)
{
	SynGlyphX::PortableVisualizationExport::SetupSourceDirectories();

	m_dataTransformModel = new DataTransformModel(this);
	QObject::connect(m_dataTransformModel, &DataTransformModel::dataChanged, this, [&, this](const QModelIndex& topLeft, const QModelIndex& bottomRight){ setWindowModified(true); });
	QObject::connect(m_dataTransformModel, &DataTransformModel::rowsInserted, this, [&, this](const QModelIndex& parent, int first, int last){ setWindowModified(true); });
	QObject::connect(m_dataTransformModel, &DataTransformModel::rowsRemoved, this, [&, this](const QModelIndex& parent, int first, int last){ setWindowModified(true); });
	
	m_glyphRolesTableModel = new GlyphRolesTableModel(m_dataTransformModel, this);

	CreateMenus();
    CreateDockWidgets();

	QObject::connect(m_glyphTreesView, &GlyphTreesView::SelectionChangedSourceModel, m_glyphRolesTableModel, &GlyphRolesTableModel::SetSelectedGlyphTreeIndexes);
	QObject::connect(m_glyphTreesView->GetClearSelectedInputBindingsAction(), &QAction::triggered, m_glyphRolesTableModel, &GlyphRolesTableModel::ClearInputBindings);

	CreateCenterWidget();

	ReadNewMappingDefaults();
	ClearAndInitializeDataMapping();

	//Setup data transform
	SynGlyphXANTz::ANTzExportTransformer::SetLogoFilename(SynGlyphX::GlyphBuilderApplication::applicationDirPath() + QDir::separator() + "logo.png");
	SynGlyphX::Transformer::SetDefaultImagesDirectory(SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation());

	QObject::connect(m_baseObjectsModel, &SynGlyphX::RoleDataFilterProxyModel::dataChanged, m_dataBindingWidget, &DataBindingTablesWidget::OnBaseObjectChanged);

	QStringList commandLineArguments = SynGlyphX::Application::arguments();
	if (commandLineArguments.size() > 1) {

		QDir dataTransformToLoad(commandLineArguments[1]);
		LoadDataTransform(QDir::toNativeSeparators(dataTransformToLoad.canonicalPath()));
	}

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

DataMapperWindow::~DataMapperWindow()
{
	
}

void DataMapperWindow::closeJVM(){
	dec.destroyJVM();
}

void DataMapperWindow::CreateCenterWidget() {
	
	m_minMaxGlyph3DWidget = new DataMapping3DWidget(m_dataTransformModel, this);
	m_minMaxGlyph3DWidget->SetModel(dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(m_glyphTreesView->model()), m_glyphTreesView->selectionModel());
	m_minMaxGlyph3DWidget->SetAllowMultiselect(true);

	m_minMaxGlyph3DWidget->addActions(m_glyphTreesView->GetGlyphActions());
	m_minMaxGlyph3DWidget->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	m_minMaxGlyph3DWidget->addActions(m_glyphTreesView->GetEditActions());
	m_minMaxGlyph3DWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

	QObject::connect(m_showAnimation, &QAction::toggled, m_minMaxGlyph3DWidget, &DataMapping3DWidget::EnableAnimation);
	
	setCentralWidget(m_minMaxGlyph3DWidget);
}

void DataMapperWindow::CreateExportToPortableVisualizationSubmenu() {

	bool windowsANTzTemplateExists = DoesANTzTemplateExist(m_antzExportDirectories[SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::Windows]);
	bool zSpaceANTzTemplateExists = DoesANTzTemplateExist(m_antzExportDirectories[SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::WindowsZSpace]);
	bool macANTzTemplateExists = DoesANTzTemplateExist(m_antzExportDirectories[SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::Mac]);

	if (windowsANTzTemplateExists || zSpaceANTzTemplateExists || macANTzTemplateExists) {

		m_fileMenu->addSeparator();
		QMenu* portableVisualizationMenu = m_fileMenu->addMenu(tr("Create Portable Visualization"));

		if (windowsANTzTemplateExists) {

			QAction* exportToANTzAction = CreateMenuAction(portableVisualizationMenu, tr("Windows"));
			QObject::connect(exportToANTzAction, &QAction::triggered, this, [this]{ ExportToANTz(SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::Windows); });
			m_projectDependentActions.push_back(exportToANTzAction);
		}

		if (zSpaceANTzTemplateExists) {

			QAction* exportTozSpaceANTzAction = CreateMenuAction(portableVisualizationMenu, tr("Windows (zSpace)"));
			QObject::connect(exportTozSpaceANTzAction, &QAction::triggered, this, [this]{ ExportToANTz(SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::WindowsZSpace); });
			m_projectDependentActions.push_back(exportTozSpaceANTzAction);
		}

		if (macANTzTemplateExists) {

			QAction* exportToMacANTzAction = CreateMenuAction(portableVisualizationMenu, tr("Mac"));
			QObject::connect(exportToMacANTzAction, &QAction::triggered, this, [this]{ ExportToANTz(SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::Mac); });
			m_projectDependentActions.push_back(exportToMacANTzAction);
		}
	}
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

	CreateExportToPortableVisualizationSubmenu();

	m_fileMenu->addActions(m_recentFileActions);

    m_fileMenu->addSeparator();

    QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
    QObject::connect(exitAction, &QAction::triggered, this, &DataMapperWindow::close);

	//Create Edit Menu
	m_editMenu = menuBar()->addMenu(tr("Edit"));

    //Create Glyph Menu
    m_glyphMenu = menuBar()->addMenu(tr("Glyph"));

	QAction* glyphDefaultsAction = m_glyphMenu->addAction(tr("Defaults"));
	QObject::connect(glyphDefaultsAction, &QAction::triggered, this, &DataMapperWindow::ChangeGlyphDefaults);
	m_projectDependentActions.push_back(glyphDefaultsAction);

	m_glyphMenu->addSeparator();

	QAction* addGlyphTemplateAction = m_glyphMenu->addAction(tr("Add Glyph Templates"));
	QObject::connect(addGlyphTemplateAction, &QAction::triggered, this, &DataMapperWindow::AddGlyphTemplate);

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

	//Create Datasource Menu
	m_datasourceMenu = menuBar()->addMenu(tr("Data Source"));
    
	QAction* addDataSourcesAction = m_datasourceMenu->addAction(tr("Add Data Sources"));
    QObject::connect(addDataSourcesAction, &QAction::triggered, this, &DataMapperWindow::AddDataSources);

	m_datasourceMenu->addSeparator();

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

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &DataMapperWindow::ChangeMapDownloadSettings);

	CreateHelpMenu();

	EnableProjectDependentActions(false);
}

void DataMapperWindow::CreateDockWidgets() {

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	QDockWidget* leftDockWidgetGlyphTrees = new QDockWidget(tr("Glyph Trees"), this);

	m_glyphTreesView = new GlyphTreesView(m_dataTransformModel, leftDockWidgetGlyphTrees);
	m_glyphMenu->addActions(m_glyphTreesView->GetGlyphActions());
	m_editMenu->addActions(m_glyphTreesView->GetEditActions());

	QObject::connect(m_glyphTreesView, &GlyphTreesView::UpdateStatusBar, statusBar(), &QStatusBar::showMessage);

    //Add Tree View to dock widget on left side
	leftDockWidgetGlyphTrees->setWidget(m_glyphTreesView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidgetGlyphTrees);
	m_viewMenu->addAction(leftDockWidgetGlyphTrees->toggleViewAction());

	QDockWidget* leftDockWidgetBaseObjects = new QDockWidget(tr("Grids/Base Images"), this);

	m_baseObjectsView = new BaseObjectListView(m_dataTransformModel, leftDockWidgetBaseObjects);
	m_baseObjectsModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	m_baseObjectsModel->setSourceModel(m_dataTransformModel);
	m_baseObjectsModel->setFilterRole(DataTransformModel::DataTypeRole);
	m_baseObjectsModel->SetFilterData(DataTransformModel::DataType::BaseObjects);
	m_baseObjectsView->setModel(m_baseObjectsModel);
	m_baseObjectMenu->addActions(m_baseObjectsView->actions());

	//Add Base Objects View to dock widget on left side
	leftDockWidgetBaseObjects->setWidget(m_baseObjectsView);
	addDockWidget(Qt::LeftDockWidgetArea, leftDockWidgetBaseObjects);
	m_viewMenu->addAction(leftDockWidgetBaseObjects->toggleViewAction());

	QDockWidget* rightDockWidgetDataStats = new QDockWidget(tr("Data Stats"), this);
	m_dataSourceStats = new DataSourceStatsWidget(m_dataTransformModel, rightDockWidgetDataStats);

	rightDockWidgetDataStats->setWidget(m_dataSourceStats);
	m_dataSourceStats->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidgetDataStats);
	m_viewMenu->addAction(rightDockWidgetDataStats->toggleViewAction());

	QDockWidget* rightDockWidgetDataSources = new QDockWidget(tr("Data Sources"), this);
	m_dataSourcesView = new DataSourcesView(m_dataTransformModel, rightDockWidgetDataSources);
	m_datasourceMenu->addActions(m_dataSourcesView->GetSharedActions());

	rightDockWidgetDataSources->setWidget(m_dataSourcesView);
	m_dataSourcesView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidgetDataSources);
	m_viewMenu->addAction(rightDockWidgetDataSources->toggleViewAction());

	QDockWidget* topDockWidget = new QDockWidget(tr("Data Bindings"), this);
	m_dataBindingWidget = new DataBindingTablesWidget(m_glyphRolesTableModel, topDockWidget);
	topDockWidget->setWidget(m_dataBindingWidget);
	addDockWidget(Qt::TopDockWidgetArea, topDockWidget);
	m_viewMenu->addAction(topDockWidget->toggleViewAction());
}

void DataMapperWindow::CreateNewProject() {

	if (!AskUserToSave()) {
		return;
	}

	ClearAndInitializeDataMapping();
	m_dataSourceStats->ClearTabs();
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

void DataMapperWindow::UpdateMissingFiles(const QString& mappingFilename) {

	SynGlyphX::DataTransformMapping::SharedPtr mapping = std::make_shared<SynGlyphX::DataTransformMapping>();
	mapping->ReadFromFile(mappingFilename.toStdString());

	std::vector<boost::uuids::uuid> fileDatasourcesToBeUpdated = mapping->GetFileDatasourcesWithInvalidFiles(false);
	std::vector<unsigned int> localBaseImageIndexes = mapping->GetFileBaseObjectsWithInvalidFiles();

	bool wasDataTransformUpdated = false;

	if (!localBaseImageIndexes.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeImageFileDialog::UpdateImageFiles(localBaseImageIndexes, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	}

	if (!fileDatasourcesToBeUpdated.empty()) {

		SynGlyphX::Application::restoreOverrideCursor();
		wasDataTransformUpdated = SynGlyphX::ChangeDatasourceFileDialog::UpdateDatasourceFiles(fileDatasourcesToBeUpdated, mapping, this);
		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	}

	if (wasDataTransformUpdated) {

		mapping->WriteToFile(mappingFilename.toStdString());
	}
}

bool DataMapperWindow::LoadDataTransform(const QString& filename) {

	if (!dec.hasJVM()){
		dec.createJVM();
		m_dataTransformModel->SetDataEngineConn(&dec);
		m_dataSourceStats->SetDataEngineConn(&dec);
	}

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

		QObject::disconnect(m_modelResetConnection);

		m_dataTransformModel->LoadDataTransformFile(filename);
		m_glyphTreesView->SelectLastGlyphTreeRoot();
		SelectFirstBaseObject();

		m_modelResetConnection = QObject::connect(m_dataTransformModel, &DataTransformModel::modelReset, this, [&, this](){ setWindowModified(true); });
	}
	catch (const std::exception& e) {

		m_dataTransformModel->ClearAndReset();
		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return false;
	}

	SetCurrentFile(filename);

	EnableProjectDependentActions(true);
	SynGlyphX::Application::restoreOverrideCursor();
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

bool DataMapperWindow::ValidateNewDatasource(const QString& datasource) {

	if (datasource.right(4).toLower() == ".csv") {
		return true;
	}

	if (SynGlyphX::DatabaseInfo::IsSQLiteDB(datasource)) {
		return true;
	}

	return false;
}
/*
void DataMapperWindow::ProcessCSVFile(const QString& csvFile) {

	//Check if .csvt file exists
	QString csvtFile = csvFile + 't';
	if (!QFile::exists(csvtFile)) {

		SynGlyphX::CSVFileReader csvFileReader(csvFile.toStdString());
		SynGlyphX::CSVFileReader::CSVValues fieldNames = csvFileReader.GetHeaders();

		if (fieldNames.empty()) {

			throw std::exception("No headers found in CSV file");
		}

		QStringList fieldNameList;
		for (std::wstring fieldName : fieldNames) {

			fieldNameList.push_back(QString::fromStdWString(fieldName));
		}

		DatasourceFieldTypesDialog dialog(fieldNameList, "CSV", this);
		if (dialog.exec() == QDialog::Rejected) {

			throw std::exception("CSV file does not have data types associated with its fields");
		}

		QStringList dialogTypes = dialog.GetFieldTypes();

		SynGlyphX::CSVFileReader::CSVValues types;
		for (const QString type : dialogTypes) {

			types.push_back(type.toStdWString());
		}

		SynGlyphX::CSVTFileReaderWriter::WriteCSVTFile(csvtFile.toStdString(), types);
	}
}*/

void DataMapperWindow::AddDataSources() {

	if (!dec.hasJVM()){
		dec.createJVM();
		m_dataTransformModel->SetDataEngineConn(&dec);
		m_dataSourceStats->SetDataEngineConn(&dec);
	}

	QStringList dataSources = GetFileNamesOpenDialog("DatasourcesDir", tr("Add Data Source"), "", "All datasource files (*.*);;CSV files (*.csv)");

	if (dataSources.isEmpty()) {
		return;
	}

	unsigned int numNewDatasources = 0;
	for (const QString& datasource : dataSources) {

		if (!ValidateNewDatasource(datasource)) {

			QMessageBox::critical(this, tr("Failed To Add Data Source"), datasource + tr(" is not a recognized type"), QMessageBox::Ok);
			continue;
		}

		try {

			SynGlyphX::FileDatasource::SourceType fileDatasourceType = SynGlyphX::FileDatasource::SQLITE3;
			if (datasource.right(4).toLower() == ".csv") {

				fileDatasourceType = SynGlyphX::FileDatasource::CSV;
				//ProcessCSVFile(datasource);
			}

			boost::uuids::uuid newDBID = m_dataTransformModel->AddFileDatasource(fileDatasourceType, datasource.toStdWString());
			//m_dataTransformModel->AddFileDatasource(fileDatasourceType, datasource.toStdWString());
			qDebug() << boost::lexical_cast<std::string>(newDBID).c_str();
			/*
			SynGlyphX::Datasource::TableNames tables;
			const SynGlyphX::Datasource& newDatasource = m_dataTransformModel->GetDataMapping()->GetDatasources().GetFileDatasources().at(newDBID);

			if (newDatasource.CanDatasourceHaveMultipleTables()) {

				QSqlDatabase db = QSqlDatabase::database(QString::fromStdString(boost::uuids::to_string(newDBID)));

				if (!db.open()) {

					throw std::exception((datasource + tr(" could not be opened")).toStdString().c_str());
				}

				QStringList qtables = SynGlyphX::DatabaseInfo::GetListOfTablesWithoutAutogeneratedTables(db);

				if (!qtables.isEmpty()) {

					for (const QString& qtable : qtables) {
						tables.insert(qtable.toStdWString());
					}

					m_dataTransformModel->EnableTables(newDBID, tables, true);
				}
				else {

					throw std::exception((tr("No tables in ") + datasource).toStdString().c_str());
				}
			}
			*/
			++numNewDatasources;
		}
		catch (const std::exception& e) {

			//QSqlDatabase::removeDatabase(QString::fromStdString(boost::uuids::to_string(newDBID)));
			QMessageBox::critical(this, tr("Failed To Add Data Source"), QString::fromStdString(e.what()), QMessageBox::Ok);
			continue;
		}
	}

	if (numNewDatasources > 0) {

		m_dataSourceStats->AddNewStatsViews();
		EnableProjectDependentActions(true);
	}
}

void DataMapperWindow::CreatePortableVisualization(SynGlyphX::PortableVisualizationExport::Platform platform) {

	SynGlyphX::DataTransformMapping::ConstSharedPtr dataMapping = m_dataTransformModel->GetDataMapping();

	if (!dataMapping->GetDatasources().HasDatasources()) {

		QMessageBox::critical(this, tr("Export to ANTz Error"), tr("Visualization has no datasources."));
		return;
	}

	if (dataMapping->GetGlyphGraphs().empty()) {

		QMessageBox::critical(this, tr("Export to ANTz Error"), tr("Visualization has no glyph templates."));
		return;
	}

	if (!dataMapping->DoesAtLeastOneGlyphGraphHaveBindingsOnPosition()) {

		QMessageBox::critical(this, tr("Export to ANTz Error"), tr("Visualization has no glyph templates with bindings on Position X, Position Y, or Position Z."));
		return;
	}

	std::vector<unsigned int> missingBaseObjects = dataMapping->GetFileBaseObjectsWithInvalidFiles();
	if (!missingBaseObjects.empty()) {

		if (SynGlyphX::ChangeImageFileDialog::UpdateImageFiles(missingBaseObjects, std::const_pointer_cast<SynGlyphX::DataTransformMapping>(dataMapping), this)) {

			if (!m_currentFilename.isEmpty()) {

				m_dataTransformModel->SaveDataTransformFile(m_currentFilename);
			}
		}
		else {

			QMessageBox::critical(this, tr("Export to ANTz Error"), tr("Portable visualization can't be created when images are still missing."));
			return;
		}
	}

	
	std::set<QDir> projectFileDirs;
	projectFileDirs.insert(QFileInfo(m_currentFilename).canonicalPath());
	for (const auto& fileDatasource : m_dataTransformModel->GetDataMapping()->GetDatasources().GetFileDatasources()) {

		projectFileDirs.insert(QFileInfo(QString::fromStdWString(fileDatasource.second.GetFilename())).canonicalPath());
	}

	QString csvDirectory = QDir::toNativeSeparators(GetExistingEmptyDirectory(projectFileDirs, "ANTzExportDir", tr("Select Directory For Portable Visualization"), "", tr("Selected directory contains one or more files relevant to the project.")));
	if (csvDirectory.isEmpty()) {

		return;
	}

	
	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

	try {

		//bool useOldANTzFilenames = !QFile::exists(templateDir + QDir::separator() + "usr" + QDir::separator() + "csv" + QDir::separator() + "antzglobals.csv");
		SaveProject();
		DataEngine::GlyphEngine ge;
		std::string baseImageDir = SynGlyphX::GlyphBuilderApplication::GetDefaultBaseImagesLocation().toStdString();
		std::string baseFilename = (QString::fromStdWString(SynGlyphX::DefaultBaseImageProperties::GetBasefilename()).toStdString());
		ge.initiate(dec.getEnv(), m_currentFilename.toStdString(), csvDirectory.toStdString() + "\\", SynGlyphX::PortableVisualizationExport::GetSourceDirectory(platform).toStdString() + "\\", baseImageDir, baseFilename, "DataMapper");
		ge.getDownloadedBaseImage(m_dataTransformModel->GetDataMapping().get()->GetBaseObjects());
		ge.generateGlyphs();

		//SynGlyphXANTz::ANTzExportTransformer transformer(csvDirectory, m_antzExportDirectories[platform], platform, false);
		//transformer.Transform(*(m_dataTransformModel->GetDataMapping().get()));

		SynGlyphX::Application::restoreOverrideCursor();

		const QString& GlyphEngineError = ge.getError();
		
		//const QString& transformerError = transformer.GetError();
		if (!GlyphEngineError.isNull()) {

			QMessageBox::information(this, "Transformation Error", GlyphEngineError, QMessageBox::Ok);
		}
	}
	catch (const std::exception& e) {

		try{
			SynGlyphX::Filesystem::RemoveContentsOfDirectory(csvDirectory.toStdString());
			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::critical(this, tr("Create Portable Visualization Error"), e.what());
		}
		catch (...)
		{
			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::information(this, tr("Directory in use"), tr("Could not create portable visualization because files in this directory are currently in use."), QMessageBox::Ok);
		}
		return;
	}

	statusBar()->showMessage("Data transform successfully exported to ANTz", 6000);
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

void DataMapperWindow::AddGlyphTemplate() {

	QStringList glyphTemplates = GetFileNamesOpenDialog("GlyphTemplatesDir", tr("Add Glyph Templates"), m_glyphTemplatesDirectory, "SynGlyphX Glyph Template Files (*.sgt *.csv)");

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
	statusBar()->showMessage("Glyph Template successfully added", 3000);
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

bool DataMapperWindow::DoesANTzTemplateExist(const QString& templateDir) const {

	boost::filesystem::path antzTemplatePath(QDir::toNativeSeparators(templateDir).toStdString());
	if (!boost::filesystem::exists(antzTemplatePath) || !boost::filesystem::is_directory(antzTemplatePath)) {

		return false;
	}

	boost::filesystem::path csvTemplatePath = antzTemplatePath / "usr" / "csv";
	if (!boost::filesystem::exists(csvTemplatePath) || !boost::filesystem::is_directory(csvTemplatePath)) {

		return false;
	}

	boost::filesystem::path imageTemplatePath = antzTemplatePath / "usr" / "images";
	if (!boost::filesystem::exists(imageTemplatePath) || !boost::filesystem::is_directory(imageTemplatePath)) {

		return false;
	}

	return true;
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
	m_minMaxGlyph3DWidget->SetBaseImage(SynGlyphX::DefaultBaseImageProperties::s_typeStrings.right.at(settings.value("baseImage", "World").toString().toStdWString()));
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
