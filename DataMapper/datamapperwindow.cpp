#include "datamapperwindow.h"
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
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
#include "antztransformer.h"
#include "datasourcefieldtypesdialog.h"
#include "csvfilereader.h"
#include "csvtfilereaderwriter.h"
#include "singlewidgetdialog.h"
#include "glyphdefaultswidget.h"

const QString ANTzTemplateDir = "ANTzTemplate";

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent),
	m_baseObjectsView(nullptr),
	m_glyphTreesView(nullptr),
	m_dataSourceStats(nullptr),
	m_dataBindingWidget(nullptr),
	m_minMaxGlyphModel(nullptr),
	m_dataTransformModel(nullptr),
	m_minMaxGlyph3DWidget(nullptr),
	m_glyphTreesModel(nullptr),
	m_baseObjectsModel(nullptr)
{
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

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

DataMapperWindow::~DataMapperWindow()
{
	
}

void DataMapperWindow::CreateCenterWidget() {

	m_minMaxGlyphModel = new MinMaxGlyphModel(m_dataTransformModel, this);
	QObject::connect(m_minMaxGlyphModel, &MinMaxGlyphModel::dataChanged, this, [&, this](const QModelIndex& topLeft, const QModelIndex& bottomRight){ setWindowModified(true); });
	QObject::connect(m_glyphTreesView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DataMapperWindow::OnGlyphTreesViewSelectionChanged);
	QObject::connect(m_glyphTreesView->GetClearSelectedInputBindingsAction(), &QAction::triggered, m_minMaxGlyphModel, &MinMaxGlyphModel::ClearInputBindings);

	QSplitter* centerWidget = new QSplitter(Qt::Vertical, this);
	
	m_minMaxGlyph3DWidget = new DataMapping3DWidget(m_dataTransformModel, centerWidget);
	m_minMaxGlyph3DWidget->SetModel(m_glyphTreesModel, m_glyphTreesView->selectionModel());

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

	if (DoesANTzTemplateExist()) {

		m_fileMenu->addSeparator();

		QAction* exportToANTzAction = CreateMenuAction(m_fileMenu, tr("Export to ANTz"));
		QObject::connect(exportToANTzAction, &QAction::triggered, this, &DataMapperWindow::ExportToANTz);
		m_projectDependentActions.push_back(exportToANTzAction);
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

	QAction* addGlyphTemplateAction = m_glyphMenu->addAction(tr("Add Glyph Templates"));
	QObject::connect(addGlyphTemplateAction, &QAction::triggered, this, &DataMapperWindow::AddGlyphTemplate);

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
    CreateFullScreenAction(m_viewMenu);

    m_viewMenu->addSeparator();

	//Create Tools Menu
	m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* newMappingDefaultsAction = m_toolsMenu->addAction(tr("New Mapping Glyph Defaults"));
	QObject::connect(newMappingDefaultsAction, &QAction::triggered, this, &DataMapperWindow::ChangeNewMappingDefaults);

	m_toolsMenu->addSeparator();

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &DataMapperWindow::ChangeMapDownloadSettings);

    m_helpMenu = menuBar()->addMenu(tr("Help"));
    QAction* aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
    QObject::connect(aboutBoxAction, SIGNAL(triggered()), this, SLOT(ShowAboutBox()));

	EnableProjectDependentActions(false);
}

void DataMapperWindow::CreateDockWidgets() {

	QDockWidget* leftDockWidgetGlyphTrees = new QDockWidget(tr("Glyph Trees"), this);

	m_glyphTreesView = new GlyphTreesView(leftDockWidgetGlyphTrees);
	m_glyphTreesModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	m_glyphTreesModel->setFilterRole(DataTransformModel::DataTypeRole);
	m_glyphTreesModel->setSourceModel(m_dataTransformModel);
	m_glyphTreesModel->SetFilterData(DataTransformModel::DataType::GlyphTrees);
	m_glyphTreesView->setModel(m_glyphTreesModel);
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

void DataMapperWindow::ShowAboutBox() {

    QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
    QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion());
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

	QString openFile = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("SynGlyphX Data Transform Project Files (*.sdt)"));
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

	QString saveFile = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("SynGlyphX Data Transform Project Files (*.sdt)"));

	if (!saveFile.isEmpty()) {

		m_dataTransformModel->ResetDataMappingID();
		return SaveDataTransform(saveFile);
	}
	else {

		return false;
	}
}

void DataMapperWindow::LoadRecentFile(const QString& filename) {

	if (AskUserToSave()) {
		LoadDataTransform(filename);
	}
}

void DataMapperWindow::LoadDataTransform(const QString& filename) {

	try {

		m_dataTransformModel->LoadDataTransformFile(filename);
		m_dataSourceStats->RebuildStatsViews();
		SelectLastGlyphTreeRoot();
		SelectFirstBaseObject();
	}
	catch (const std::exception& e) {

		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	SetCurrentFile(filename);

	EnableProjectDependentActions(true);

	statusBar()->showMessage("Project successfully opened", 3000);
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

	if (SynGlyphX::SourceDataManager::IsSQLiteDB(datasource)) {
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

		SynGlyphX::DatasourceFieldTypesDialog dialog(fieldNameList, "CSV", this);
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

	QStringList dataSources = QFileDialog::getOpenFileNames(this, tr("Add Data Source"), "", "All datasource files (*.*);;CSV files (*.csv)");

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

			QSqlDatabase db = QSqlDatabase::database(QString::fromStdString(boost::uuids::to_string(newDBID)));

			if (!db.open()) {

				throw std::exception((datasource + tr(" could not be opened")).toStdString().c_str());
			}

			if (newDatasource.CanDatasourceHaveMultipleTables()) {

				QStringList qtables = SynGlyphX::SourceDataManager::GetListOfTablesWithoutAutogeneratedTables(db);

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

void DataMapperWindow::ExportToANTz() {

	QString csvDirectory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Export to ANTz"), ""));
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

		SynGlyphX::Filesystem::RemoveContentsOfDirectory(csvDirectory.toStdString());
		SynGlyphX::Filesystem::CopyDirectoryOverwrite(QDir::toNativeSeparators(SynGlyphX::Application::applicationDirPath() + QDir::separator() + ANTzTemplateDir).toStdString(), csvDirectory.toStdString(), true);

		SynGlyphXANTz::ANTzTransformer transformer(csvDirectory);
		transformer.Transform(*(m_dataTransformModel->GetDataMapping().get()));
	}
	catch (const std::exception& e) {

		SynGlyphX::Filesystem::RemoveContentsOfDirectory(csvDirectory.toStdString());
		SynGlyphX::Application::restoreOverrideCursor();
		QMessageBox::critical(this, "Export to ANTz Error", e.what());
		return;
	}

	SynGlyphX::Application::restoreOverrideCursor(); 
	statusBar()->showMessage("Data transform sucessfully exported to ANTz", 3000);
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

	QStringList glyphTemplates = QFileDialog::getOpenFileNames(this, tr("Add Glyph Templates"), "", "SynGlyphX Glyph Template Files (*.sgt)");

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
	SelectLastGlyphTreeRoot();
	statusBar()->showMessage("Glyph Template successfully added", 3000);
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

bool DataMapperWindow::DoesANTzTemplateExist() const {

	boost::filesystem::path antzTemplatePath(QDir::toNativeSeparators(SynGlyphX::Application::applicationDirPath() + QDir::separator() + ANTzTemplateDir).toStdString());
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

	m_dataTransformModel->Clear();
	m_dataTransformModel->SetDefaults(m_newMappingDefaults);
	SelectFirstBaseObject();
}

void DataMapperWindow::SelectLastGlyphTreeRoot() {

	m_glyphTreesView->selectionModel()->select(m_glyphTreesModel->index(m_dataTransformModel->GetDataMapping()->GetGlyphTrees().size() - 1, 0), QItemSelectionModel::ClearAndSelect);
}

void DataMapperWindow::OnGlyphTreesViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty()) {

		m_minMaxGlyphModel->Clear();
	}
	else {

		m_minMaxGlyphModel->SetMinMaxGlyph(m_glyphTreesModel->mapSelectionToSource(selected).indexes()[0]);
	}
}

void DataMapperWindow::SelectFirstBaseObject() {

	m_baseObjectsView->selectionModel()->select(m_baseObjectsModel->index(0, 0), QItemSelectionModel::ClearAndSelect);
}