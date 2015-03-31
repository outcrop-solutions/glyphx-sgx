#include "datamapperwindow.h"
#include <QtCore/QSettings>
#include <QtCore/QDir>
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
#include "application.h"
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
#include "glyphdefaultswidget.h"
#include "newglyphtreewizard.h"
#include "databaseinfo.h"

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent),
	m_baseObjectsView(nullptr),
	m_glyphTreesView(nullptr),
	m_dataSourceStats(nullptr),
	m_dataBindingWidget(nullptr),
	m_minMaxGlyphModel(nullptr),
	m_dataTransformModel(nullptr),
	m_minMaxGlyph3DWidget(nullptr),
	m_baseObjectsModel(nullptr)
{
	QSettings settings;
	settings.beginGroup("ANTzExport");
	m_antzExportDirectory = settings.value("default", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzTemplate").toString();
	m_antzzSpaceExportDirectory = settings.value("zSpace", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "ANTzzSpaceTemplate").toString();
	settings.endGroup();

	m_dataTransformModel = new DataTransformModel(this);
	QObject::connect(m_dataTransformModel, &DataTransformModel::dataChanged, this, [&, this](const QModelIndex& topLeft, const QModelIndex& bottomRight){ setWindowModified(true); });
	QObject::connect(m_dataTransformModel, &DataTransformModel::rowsInserted, this, [&, this](const QModelIndex& parent, int first, int last){ setWindowModified(true); });
	QObject::connect(m_dataTransformModel, &DataTransformModel::rowsRemoved, this, [&, this](const QModelIndex& parent, int first, int last){ setWindowModified(true); });
	
	CreateMenus();
    CreateDockWidgets();

	CreateCenterWidget();

	ReadNewMappingDefaults();
	ClearAndInitializeDataMapping();

	QObject::connect(m_baseObjectsModel, &SynGlyphX::RoleDataFilterProxyModel::dataChanged, m_dataBindingWidget, &DataBindingWidget::OnBaseObjectChanged);

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

void DataMapperWindow::CreateCenterWidget() {

	m_minMaxGlyphModel = new MinMaxGlyphModel(m_dataTransformModel, this);
	QObject::connect(m_minMaxGlyphModel, &MinMaxGlyphModel::dataChanged, this, [&, this](const QModelIndex& topLeft, const QModelIndex& bottomRight){ setWindowModified(true); });
	QObject::connect(m_glyphTreesView, &GlyphTreesView::SelectionChangedSourceModel, this, &DataMapperWindow::OnGlyphTreesViewSelectionChanged);
	QObject::connect(m_glyphTreesView->GetClearSelectedInputBindingsAction(), &QAction::triggered, m_minMaxGlyphModel, &MinMaxGlyphModel::ClearInputBindings);

	QSplitter* centerWidget = new QSplitter(Qt::Vertical, this);
	
	m_minMaxGlyph3DWidget = new DataMapping3DWidget(m_dataTransformModel, centerWidget);
	m_minMaxGlyph3DWidget->SetModel(dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(m_glyphTreesView->model()), m_glyphTreesView->selectionModel());

	QObject::connect(m_showAnimation, &QAction::toggled, m_minMaxGlyph3DWidget, &DataMapping3DWidget::EnableAnimation);

	//m_viewMenu->addAction(glyphViewDockWidget->toggleViewAction());

	m_dataBindingWidget = new DataBindingWidget(m_minMaxGlyphModel, centerWidget);

	centerWidget->addWidget(m_dataBindingWidget);
	centerWidget->addWidget(m_minMaxGlyph3DWidget);

	centerWidget->setStretchFactor(1, 2);
	centerWidget->setStretchFactor(0, 0);
	
	setCentralWidget(centerWidget);
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

	bool defaultANTzTemplateExists = DoesANTzTemplateExist(m_antzExportDirectory);
	bool zSpaceANTzTemplateExists = DoesANTzTemplateExist(m_antzzSpaceExportDirectory);

	if (defaultANTzTemplateExists || zSpaceANTzTemplateExists) {

		m_fileMenu->addSeparator();
	}

	if (defaultANTzTemplateExists) {

		QAction* exportToANTzAction = CreateMenuAction(m_fileMenu, tr("Create Portable Visualization"));
		QObject::connect(exportToANTzAction, &QAction::triggered, this, [this]{ ExportToANTz(m_antzExportDirectory); });
		m_projectDependentActions.push_back(exportToANTzAction);
	}

	if (zSpaceANTzTemplateExists) {

		QAction* exportTozSpaceANTzAction = CreateMenuAction(m_fileMenu, tr("Create Portable Visualization (zSpace)"));
		QObject::connect(exportTozSpaceANTzAction, &QAction::triggered, this, [this]{ ExportToANTz(m_antzzSpaceExportDirectory); });
		m_projectDependentActions.push_back(exportTozSpaceANTzAction);
	}

	m_fileMenu->addActions(m_recentFileActions);

    m_fileMenu->addSeparator();

    QAction* exitAction = CreateMenuAction(m_fileMenu, tr("Exit"), QKeySequence::Quit);
    QObject::connect(exitAction, &QAction::triggered, this, &DataMapperWindow::close);

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

	QAction* addBaseObjectAction = m_baseObjectMenu->addAction(tr("Add Base Object"));
	QObject::connect(addBaseObjectAction, &QAction::triggered, this, &DataMapperWindow::AddBaseObject);

	m_baseObjectMenu->addSeparator();

	//Create Datasource Menu
	m_datasourceMenu = menuBar()->addMenu(tr("Data Source"));
    
	QAction* addDataSourcesAction = m_datasourceMenu->addAction(tr("Add Data Sources"));
    QObject::connect(addDataSourcesAction, &QAction::triggered, this, &DataMapperWindow::AddDataSources);

	//m_datasourceMenu->addSeparator();

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("View"));

	m_showAnimation = m_viewMenu->addAction(tr("Show Animation"));
	m_showAnimation->setCheckable(true);
	m_showAnimation->setChecked(true);

	m_viewMenu->addSeparator();

    CreateFullScreenAction(m_viewMenu);

    m_viewMenu->addSeparator();

	//Create Tools Menu
	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* newMappingDefaultsAction = m_toolsMenu->addAction(tr("New Mapping Glyph Defaults"));
	QObject::connect(newMappingDefaultsAction, &QAction::triggered, this, &DataMapperWindow::ChangeNewMappingDefaults);

	m_toolsMenu->addSeparator();

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &DataMapperWindow::ChangeMapDownloadSettings);

	CreateHelpMenu();

	EnableProjectDependentActions(false);
}

void DataMapperWindow::CreateDockWidgets() {

	QDockWidget* leftDockWidgetGlyphTrees = new QDockWidget(tr("Glyph Trees"), this);

	m_glyphTreesView = new GlyphTreesView(m_dataTransformModel, leftDockWidgetGlyphTrees);
	m_glyphMenu->addActions(m_glyphTreesView->actions());

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

	QDockWidget* rightDockWidget = new QDockWidget(tr("Data Stats"), this);
	m_dataSourceStats = new DataSourceStatsWidget(m_dataTransformModel, rightDockWidget);

	rightDockWidget->setWidget(m_dataSourceStats);
	addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
	m_viewMenu->addAction(rightDockWidget->toggleViewAction());
}

void DataMapperWindow::CreateNewProject() {

	if (!AskUserToSave()) {
		return;
	}

	ClearAndInitializeDataMapping();
	m_dataSourceStats->ClearTabs();
	m_minMaxGlyphModel->Clear();

	EnableProjectDependentActions(false);
	UpdateFilenameWindowTitle("Untitled");
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

bool DataMapperWindow::LoadDataTransform(const QString& filename) {

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

		m_dataTransformModel->LoadDataTransformFile(filename);
		m_dataSourceStats->RebuildStatsViews();
		m_glyphTreesView->SelectLastGlyphTreeRoot();
		SelectFirstBaseObject();
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return false;
	}

	SetCurrentFile(filename);

	EnableProjectDependentActions(true);

	statusBar()->showMessage("Project successfully opened", 3000);
	return true;
}

bool DataMapperWindow::SaveDataTransform(const QString& filename) {

	try {
		m_dataBindingWidget->CommitChanges();
		m_dataTransformModel->SaveDataTransformFile(filename);
		SetCurrentFile(filename);
		setWindowModified(false);
		statusBar()->showMessage("Data transform successfully saved", 3000);
		return true;
	}
	catch (const std::exception& e) {

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
}

void DataMapperWindow::AddDataSources() {

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

		boost::uuids::uuid newDBID;
		try {

			SynGlyphX::FileDatasource::SourceType fileDatasourceType = SynGlyphX::FileDatasource::SQLITE3;
			if (datasource.right(4).toLower() == ".csv") {

				fileDatasourceType = SynGlyphX::FileDatasource::CSV;
				ProcessCSVFile(datasource);
			}

			newDBID = m_dataTransformModel->AddFileDatasource(fileDatasourceType, datasource.toStdWString());
			SynGlyphX::Datasource::TableSet tables;
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

			++numNewDatasources;
		}
		catch (const std::exception& e) {

			QSqlDatabase::removeDatabase(QString::fromStdString(boost::uuids::to_string(newDBID)));
			QMessageBox::critical(this, tr("Failed To Add Data Source"), QString::fromStdString(e.what()), QMessageBox::Ok);
			continue;
		}
	}

	if (numNewDatasources > 0) {

		m_dataSourceStats->AddNewStatsViews();
		EnableProjectDependentActions(true);
	}
}

void DataMapperWindow::ExportToANTz(const QString& templateDir) {

	QString csvDirectory = QDir::toNativeSeparators(GetExistingDirectoryDialog("ANTzExportDir", tr("Select Directory For Portable Visualization"), ""));
	if (csvDirectory.isEmpty()) {

		return;
	}

	QDir dir(csvDirectory);
	if (dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() > 0) {

		if (QMessageBox::question(this, tr("Directory isn't empty"), tr("Selected directory is not empty.  All contents of the directory will be deleted before export.  Do you wish to continue?")) == QMessageBox::No) {

			return;
		}
	}

	SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);

	try {

		bool useOldANTzFilenames = !QFile::exists(templateDir + QDir::separator() + "usr" + QDir::separator() + "csv" + QDir::separator() + "antzglobals.csv");
		SynGlyphXANTz::ANTzExportTransformer transformer(csvDirectory, templateDir, SynGlyphX::Application::applicationDirPath() + QDir::separator() + "world.png", useOldANTzFilenames);
		transformer.Transform(*(m_dataTransformModel->GetDataMapping().get()));
	}
	catch (const std::exception& e) {

		SynGlyphX::Filesystem::RemoveContentsOfDirectory(csvDirectory.toStdString());
		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::critical(this, "Export to ANTz Error", e.what());
		return;
	}

	SynGlyphX::Application::restoreOverrideCursor(); 
	statusBar()->showMessage("Data transform sucessfully exported to ANTz", 6000);
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
	GlyphDefaultsWidget* glyphDefaultsWidget = new GlyphDefaultsWidget(oldDefaults, this);
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
	
	GlyphDefaultsWidget* glyphDefaultsWidget = new GlyphDefaultsWidget(m_newMappingDefaults, this);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, glyphDefaultsWidget, this);
	dialog.setWindowTitle(tr("New Mapping Glyph Defaults"));

	if (dialog.exec() == QDialog::Accepted) {

		m_newMappingDefaults = glyphDefaultsWidget->GetDefaults();
		WriteNewMappingDefaults();
		if (!m_projectDependentActions[0]->isEnabled()) {

			m_dataTransformModel->SetDefaults(m_newMappingDefaults);
		}
	}
}

void DataMapperWindow::ReadNewMappingDefaults() {

	QSettings settings;
	settings.beginGroup("Defaults");
	std::wistringstream xmlDefaults(settings.value("glyph", "").toString().toStdWString());
	settings.endGroup();

	if (!xmlDefaults.str().empty()) {

		boost::property_tree::wptree propertyTree;
		boost::property_tree::read_xml(xmlDefaults, propertyTree, SynGlyphX::XMLPropertyTreeFile::GetReadFlags());
		boost::optional<boost::property_tree::wptree&> defaultsPropertyTree = propertyTree.get_child_optional(SynGlyphX::DataMappingDefaults::s_propertyTreeName);
		if (defaultsPropertyTree.is_initialized()) {

			m_newMappingDefaults = SynGlyphX::DataMappingDefaults(defaultsPropertyTree.get());
		}
	}
}

void DataMapperWindow::WriteNewMappingDefaults() {

	boost::property_tree::wptree propertyTree;
	m_newMappingDefaults.ExportToPropertyTree(propertyTree);

	std::wostringstream xmlWriteDefaults;
	boost::property_tree::write_xml(xmlWriteDefaults, propertyTree, SynGlyphX::XMLPropertyTreeFile::GetWriteSettings());

	QSettings settings;
	settings.beginGroup("Defaults");
	settings.setValue("glyph", QString::fromStdWString(xmlWriteDefaults.str()));
	settings.endGroup();
}

void DataMapperWindow::ClearAndInitializeDataMapping() {

	m_dataTransformModel->ClearAndReset();
	m_dataTransformModel->SetDefaults(m_newMappingDefaults);
	SelectFirstBaseObject();
}

void DataMapperWindow::OnGlyphTreesViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty()) {

		m_minMaxGlyphModel->Clear();
	}
	else {

		m_minMaxGlyphModel->SetMinMaxGlyph(selected.indexes()[0]);
	}
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
	settings.endGroup();
}

void DataMapperWindow::WriteSettings() {

	SynGlyphX::MainWindow::WriteSettings();
	QSettings settings;
	settings.beginGroup("Display");
	settings.setValue("show", m_showAnimation->isChecked());
	settings.endGroup();
}