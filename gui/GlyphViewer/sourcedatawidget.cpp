#include "sourcedatawidget.h"
#include <QtGui/QCloseEvent>
#include <set>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QStandardPaths>
#include "csvfilewriter.h"
#include "glyphbuilderapplication.h"
#include <boost/uuid/uuid_io.hpp>
#include "HeaderProxyModel.h"

SourceDataWidget::SourceDataWidget(SourceDataCache::ConstSharedPtr sourceDataCache, SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping, QWidget *parent)
	: QWidget(parent, Qt::Window),
	m_sourceDataCache(sourceDataCache),
	m_dataTransformMapping(dataTransformMapping)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	m_sourceDataTabs = new QTabWidget(this);
	m_sourceDataTabs->setTabsClosable(false);
	mainLayout->addWidget(m_sourceDataTabs, 1);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();

	QPushButton* saveButton = new QPushButton(tr("Save Current Tab To File"), this);
	buttonsLayout->addWidget(saveButton);
	QObject::connect(saveButton, &QPushButton::clicked, this, &SourceDataWidget::SaveCurrentTabToFile);

	QPushButton* createVizButton = new QPushButton(tr("Create Visualization From Current Tab"), this);
	buttonsLayout->addWidget(createVizButton);
	QObject::connect(createVizButton, &QPushButton::clicked, this, &SourceDataWidget::CreateSubsetVisualization);

	buttonsLayout->addStretch(1);

	QPushButton* closeButton = new QPushButton(tr("Close"), this);
	buttonsLayout->addWidget(closeButton);
	QObject::connect(closeButton, &QPushButton::clicked, this, &SourceDataWidget::close);

	mainLayout->addLayout(buttonsLayout);

	m_statusBar = new QStatusBar(this);
	mainLayout->addWidget(m_statusBar);

	setLayout(mainLayout);

	ReadSettings();
}

SourceDataWidget::~SourceDataWidget()
{
	WriteSettings();
}

void SourceDataWidget::closeEvent(QCloseEvent* event) {

	WriteSettings();
	emit WindowHidden();
	QWidget::closeEvent(event);
}

void SourceDataWidget::DeleteTabs() {

	while (m_sourceDataTabs->count() > 0) {

		QTableView* tableView = dynamic_cast<QTableView*>(m_sourceDataTabs->widget(0));
		m_sourceDataTabs->removeTab(0);
		delete tableView;
	}
	
	//The objects in this collection have already been deleted by deleting the table views so just clear this collection
	m_sqlQueryModels.clear();

	m_tableToFieldsMap.clear();
}

void SourceDataWidget::OnNewVisualization() {

	DeleteTabs();

	SynGlyphX::DataTransformMapping::DatasourceMap datasourceMap = m_dataTransformMapping->GetDatasourcesInUse();
	for (auto datasource : datasourceMap) {

		for (auto table : datasource.second->GetTables()) {

			SynGlyphX::InputTable inputTable(datasource.first, table.first);
			if (m_sourceDataCache->IsTableInCache(inputTable)) {

				std::unordered_map<std::wstring, std::wstring> fieldToAliasMap = m_dataTransformMapping->GetFieldToAliasMapForTable(inputTable);

				QString sourceDataTablename = SourceDataCache::CreateTablename(inputTable);

				QTableView* tableView = new QTableView(m_sourceDataTabs);
				tableView->setObjectName(sourceDataTablename);
				tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
				tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
				tableView->verticalHeader()->setVisible(false);

				QSqlQueryModel* queryModel = new QSqlQueryModel(tableView);
				SynGlyphX::HeaderProxyModel* headerProxyModel = new SynGlyphX::HeaderProxyModel(tableView);
				headerProxyModel->setSourceModel(queryModel);
				headerProxyModel->SetHorizontalHeaderMap(fieldToAliasMap);
				tableView->setModel(headerProxyModel);
				
				m_sourceDataTabs->addTab(tableView, m_sourceDataCache->GetFormattedNames().at(sourceDataTablename));

				m_sqlQueryModels[sourceDataTablename] = queryModel;

				m_tableToFieldsMap[sourceDataTablename] = m_sourceDataCache->GetColumnsForTable(inputTable);
			}
		}
	}
}

void SourceDataWidget::UpdateTables() {

	for (unsigned int i = 0; i < m_sourceDataTabs->count(); ++i) {

		QTableView* tableView = dynamic_cast<QTableView*>(m_sourceDataTabs->widget(i));
		QString sourceDataTablename = tableView->objectName();

		SourceDataCache::SharedSQLQuery query = m_sourceDataCache->CreateSelectQuery(sourceDataTablename,
			m_tableToFieldsMap[sourceDataTablename], GetSourceIndexesForTable(sourceDataTablename));
		query->exec();

		QSqlQueryModel* queryModel = m_sqlQueryModels[sourceDataTablename];
		queryModel->query().clear();
		queryModel->clear();
		queryModel->setQuery(*query.data());
		if (queryModel->lastError().isValid()) {

			throw std::runtime_error("Failed to set SQL query for source data widget.");
		}
	}
}

void SourceDataWidget::ReadSettings() {

	QSettings settings;

	settings.beginGroup("SourceDataWidget");
	resize(settings.value("size", QSize(700, 400)).toSize());
	restoreGeometry(settings.value("geometry").toByteArray());
	settings.endGroup();
}

void SourceDataWidget::WriteSettings() {

	QSettings settings;
	settings.beginGroup("SourceDataWidget");
	settings.setValue("size", size());
	settings.setValue("geometry", saveGeometry());
	settings.endGroup();
}

void SourceDataWidget::SaveCurrentTabToFile() {

	QSettings settings;
	settings.beginGroup("SourceDataWidget");
	QString initialDir = settings.value("fileSaveDir", "").toString() + QDir::separator() + "selectedsourcedata.csv";

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Current Tab To File"), initialDir, "CSV Files (*.csv)");
	if (!filename.isEmpty()) {

		try {
			SynGlyphX::GlyphBuilderApplication::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
			WriteToFile(m_sqlQueryModels[m_sourceDataTabs->currentWidget()->objectName()], filename);

			QFileInfo fileInfo(filename);
			settings.setValue("fileSaveDir", fileInfo.absolutePath());
			SynGlyphX::GlyphBuilderApplication::restoreOverrideCursor();

			m_statusBar->showMessage(tr("File successfully saved"), 3000);
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("File save failed"), tr("File was not written: ") + e.what());
		}
	}

	settings.endGroup();
}

void SourceDataWidget::WriteToFile(QSqlQueryModel* queryModel, const QString& filename) {

	SynGlyphX::CSVFileWriter csvFile(filename.toStdString());

	SynGlyphX::CSVFileHandler::CSVValues headers;
	for (int i = 0; i < queryModel->columnCount(); ++i) {

		headers.push_back(queryModel->headerData(i, Qt::Horizontal).toString().toStdWString());
	}
	csvFile.WriteLine(headers);

	//QSqlQueryModel may not have loaded all rows, so force it here
	while (queryModel->canFetchMore()) {
	
		queryModel->fetchMore();
	}

	for (int j = 0; j < queryModel->rowCount(); ++j) {

		SynGlyphX::CSVFileHandler::CSVValues lineOfValues;
		QSqlRecord record = queryModel->record(j);
		for (int i = 0; i < queryModel->columnCount(); ++i) {

			QVariant var = record.value(i);
			if (!var.isValid()) {

				lineOfValues.push_back(L"\" \"");
			}
			else if ((var.type() == QVariant::Char) || (var.type() == QVariant::String)) {

				lineOfValues.push_back(L"\"" + var.toString().toStdWString() + L"\"");
			}
			else {

				lineOfValues.push_back(var.toString().toStdWString());
			}
		}

		csvFile.WriteLine(lineOfValues);
	}

	csvFile.Close();
}

void SourceDataWidget::CreateSubsetVisualization() {

	QSettings settings;
	settings.beginGroup("SourceDataWidget");
	QString initialDir = settings.value("vizSaveDir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();

	QString sdtFilename = QFileDialog::getSaveFileName(this, tr("Save Current Tab To New Visualization"), initialDir, "SynGlyphX Visualization Files (*.sdt)");
	if (!sdtFilename.isEmpty()) {

		try {
			SynGlyphX::GlyphBuilderApplication::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

			QFileInfo fileInfo(sdtFilename);
			QString stdCanonicalPath = QDir::toNativeSeparators(fileInfo.absolutePath());

			QString csvFileLocation = stdCanonicalPath + QDir::separator() + fileInfo.baseName() + "_selectedsourcedata.csv";
			WriteToFile(m_sqlQueryModels[m_sourceDataTabs->currentWidget()->objectName()], csvFileLocation);
			QStringList inputTableValues = m_sourceDataTabs->widget(m_sourceDataTabs->currentIndex())->objectName().split(':');
			boost::uuids::string_generator gen;
			SynGlyphX::InputTable inputTable(gen(inputTableValues[0].toStdWString()), (inputTableValues.size() > 1) ? inputTableValues[1].toStdWString() : SynGlyphX::FileDatasource::SingleTableName);
			SynGlyphX::DataTransformMapping::ConstSharedPtr subsetDataMapping = m_dataTransformMapping->CreateSubsetMappingWithSingleTable(inputTable, csvFileLocation.toStdWString());
			subsetDataMapping->WriteToFile(sdtFilename.toStdString());
			
			settings.setValue("vizSaveDir", stdCanonicalPath);
			SynGlyphX::GlyphBuilderApplication::restoreOverrideCursor();

			m_statusBar->showMessage(tr("Visualization successfully created"), 3000);

			emit SubsetVisualizationCreated(sdtFilename);
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("Visualization creation failed"), tr("Visualization was not created: ") + e.what());
		}
	}

	settings.endGroup();
}