#include "datamapperwindow.h"
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

const QString ANTzTemplateDir = "ANTzTemplate";

DataMapperWindow::DataMapperWindow(QWidget *parent)
    : SynGlyphX::MainWindow(parent),
	m_dataBindingWidget(nullptr)
{
	m_dataTransformModel = new DataTransformModel(this);
    CreateMenus();
    CreateDockWidgets();

	CreateCenterWidget();

	QObject::connect(m_glyphTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&, this](const QItemSelection& selected, const QItemSelection& deselected){ m_minMaxGlyphModel->SetMinMaxGlyph(selected.indexes()[0]); });

	QObject::connect(m_minMaxGlyphModel, &MinMaxGlyphModel::dataChanged, this, [&, this](const QModelIndex& topLeft, const QModelIndex& bottomRight){ setWindowModified(true); });

	statusBar()->showMessage(SynGlyphX::Application::applicationName() + " Started", 3000);
}

DataMapperWindow::~DataMapperWindow()
{

}

void DataMapperWindow::CreateCenterWidget() {

	m_glyphTreeModel = new GlyphTreeModel(this);
	m_selectionTranslator = new SelectionTranslator(m_dataTransformModel, m_glyphTreeModel, m_glyphTreeView->selectionModel(), this);

	QSplitter* centerWidget = new QSplitter(Qt::Vertical, this);
	
	m_antzWidget = new ANTzWidget(m_glyphTreeModel, m_selectionTranslator->Get3DViewSelectionModel(), false, centerWidget);
	m_antzWidget->SetEditingMode(ANTzWidget::EditingMode::None);

	//m_viewMenu->addAction(glyphViewDockWidget->toggleViewAction());

	m_minMaxGlyphModel = new MinMaxGlyphModel(m_dataTransformModel, m_selectionTranslator, this);
	m_dataBindingWidget = new DataBindingWidget(m_minMaxGlyphModel, centerWidget);

	centerWidget->addWidget(m_dataBindingWidget);
	centerWidget->addWidget(m_antzWidget);

	centerWidget->setStretchFactor(1, 2);
	centerWidget->setStretchFactor(0, 0);
	
	centerWidget->setEnabled(false);
	setCentralWidget(centerWidget);
}

void DataMapperWindow::CreateMenus() {

    //Create File Menu
    m_fileMenu = menuBar()->addMenu(tr("File"));

    QAction* newProjectAction = CreateMenuAction(m_fileMenu, tr("New Project"), QKeySequence::New);
    QObject::connect(newProjectAction, &QAction::triggered, this, &DataMapperWindow::CreateNewProject);

    QAction* openProjectAction = CreateMenuAction(m_fileMenu, tr("Open Project"), QKeySequence::Open);
    QObject::connect(openProjectAction, &QAction::triggered, this, &DataMapperWindow::OpenProject);

    QAction* saveProjectAction = CreateMenuAction(m_fileMenu, tr("Save Project"), QKeySequence::Save);
    QObject::connect(saveProjectAction, &QAction::triggered, this, &DataMapperWindow::SaveProject);
	m_projectDependentActions.push_back(saveProjectAction);

    QAction* saveAsProjectAction = CreateMenuAction(m_fileMenu, tr("Save As Project"), QKeySequence::SaveAs);
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

	QAction* addGlyphTemplateAction = m_glyphMenu->addAction(tr("Add Glyph Templates"));
	QObject::connect(addGlyphTemplateAction, &QAction::triggered, this, &DataMapperWindow::AddGlyphTemplate);

	//Create Datasource Menu
	m_datasourceMenu = menuBar()->addMenu(tr("Datasource"));
    
	QAction* addDataSourcesAction = m_datasourceMenu->addAction(tr("Add Data Sources"));
    QObject::connect(addDataSourcesAction, &QAction::triggered, this, &DataMapperWindow::AddDataSources);

	m_datasourceMenu->addSeparator();

	QAction* baseImageAction = m_datasourceMenu->addAction(tr("Choose Base Image"));
    QObject::connect(baseImageAction, &QAction::triggered, this, &DataMapperWindow::ChangeBaseImage);

    //Create View Menu
    m_viewMenu = menuBar()->addMenu(tr("View"));
    CreateFullScreenAction(m_viewMenu);

    m_viewMenu->addSeparator();

	/*m_toolsMenu = menuBar()->addMenu(tr("Tools"));

	QAction* mapDownloadSettingsAction = m_toolsMenu->addAction(tr("Map Download Settings"));
	QObject::connect(mapDownloadSettingsAction, &QAction::triggered, this, &DataMapperWindow::ChangeMapDownloadSettings);*/

    m_helpMenu = menuBar()->addMenu(tr("Help"));
    QAction* aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
    QObject::connect(aboutBoxAction, SIGNAL(triggered()), this, SLOT(ShowAboutBox()));

	EnableProjectDependentActions(false);
}

void DataMapperWindow::CreateDockWidgets() {

	QDockWidget* leftDockWidget = new QDockWidget(tr("Glyph Tree"), this);

	m_glyphTreeView = new QTreeView(leftDockWidget);
	m_glyphTreeView->setModel(m_dataTransformModel);
	m_glyphTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_glyphTreeView->setHeaderHidden(true);

    //Add Tree View to dock widget on left side
    leftDockWidget->setWidget(m_glyphTreeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);
    m_viewMenu->addAction(leftDockWidget->toggleViewAction());

	QDockWidget* rightDockWidget = new QDockWidget(tr("Data Stats"), this);
    m_dataSourceStats = new DataSourceStatsWidget(m_dataTransformModel->GetDataTransform(), rightDockWidget);

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

	m_dataTransformModel->Clear();
	m_dataSourceStats->ClearTabs();
	m_minMaxGlyphModel->Clear();

	EnableProjectDependentActions(false);
	centralWidget()->setEnabled(false);
	m_glyphTreeModel->ShowGlyph(false);
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
		m_glyphTreeView->selectionModel()->select(m_dataTransformModel->index(m_dataTransformModel->rowCount() - 1), QItemSelectionModel::ClearAndSelect);
		m_dataSourceStats->RebuildStatsViews();
		m_dataBindingWidget->EnablePositionXYMixMaxWidgets(m_dataTransformModel->GetDataTransform()->GetBaseImage().GetType() != SynGlyphX::BaseImage::Type::DownloadedMap);
	}
	catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Failed To Open Project"), tr("Failed to open project.  Error: ") + e.what(), QMessageBox::Ok);
		return;
	}

	SetCurrentFile(filename);

	EnableProjectDependentActions(true);
	centralWidget()->setEnabled(true);

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
		for (std::string fieldName : fieldNames) {

			fieldNameList.push_back(QString::fromStdString(fieldName));
		}

		SynGlyphX::DatasourceFieldTypesDialog dialog(fieldNameList, "CSV", this);
		if (dialog.exec() == QDialog::Rejected) {

			throw std::exception("CSV file does not have data types associated with its fields");
		}

		QStringList dialogTypes = dialog.GetFieldTypes();

		SynGlyphX::CSVFileReader::CSVValues types;
		for (const QString type : dialogTypes) {

			types.push_back(type.toStdString());
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
			const SynGlyphX::Datasource& newDatasource = m_dataTransformModel->GetDataTransform()->GetDatasources().GetFileDatasources().at(newDBID);

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
		setWindowModified(true);
	}
}

void DataMapperWindow::ExportToANTz() {

	QString csvDirectory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Export to ANTz"), ""));
	if (!csvDirectory.isEmpty()) {

		SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);

		try {

			//SynGlyphX::Filesystem::RemoveContentsOfDirectory(csvDirectory.toStdString());
			SynGlyphX::Filesystem::CopyDirectoryOverwrite(QDir::toNativeSeparators(SynGlyphX::Application::applicationDirPath() + QDir::separator() + ANTzTemplateDir).toStdString(), csvDirectory.toStdString(), true);

			ANTzTransformer transformer(csvDirectory);
			transformer.Transform(*(m_dataTransformModel->GetDataTransform().get()));
		}
		catch (const std::exception& e) {

			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox::critical(this, "Export to ANTz Error", e.what());
			return;
		}

		SynGlyphX::Application::restoreOverrideCursor(); 
		statusBar()->showMessage("Data transform sucessfully exported to ANTz", 3000);
	}
}

void DataMapperWindow::ChangeBaseImage() {

	BaseImageDialog dialog(this);
	dialog.SetBaseImage(m_dataTransformModel->GetDataTransform()->GetBaseImage());
	if (dialog.exec() == QDialog::Accepted) {

		const SynGlyphX::BaseImage& baseImage = dialog.GetBaseImage();
		m_dataTransformModel->SetBaseImage(baseImage);
		EnableProjectDependentActions(true);
		setWindowModified(true);
		m_dataBindingWidget->EnablePositionXYMixMaxWidgets(baseImage.GetType() != SynGlyphX::BaseImage::Type::DownloadedMap);
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
	centralWidget()->setEnabled(true);
	m_glyphTreeView->selectionModel()->select(m_dataTransformModel->index(m_dataTransformModel->rowCount() - 1), QItemSelectionModel::ClearAndSelect);
	setWindowModified(true);
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