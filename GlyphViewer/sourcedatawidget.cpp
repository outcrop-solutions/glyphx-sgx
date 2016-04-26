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
#include "csvfilewriter.h"
#include "glyphbuilderapplication.h"
#include <boost/uuid/uuid_io.hpp>

SourceDataWidget::SourceDataWidget(SourceDataCache::ConstSharedPtr sourceDataCache, SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping, QWidget *parent)
	: QWidget(parent),
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

	if (parentWidget() == nullptr) {

		WriteSettings();
		setVisible(false);
		event->ignore();
		emit WindowHidden();
	}
}

void SourceDataWidget::UpdateTables() {

	m_sourceDataTabs->clear();
	m_sqlQueryModels.clear();

	const SourceDataCache::TableNameMap& formattedNames = m_sourceDataCache->GetFormattedNames();

	for (auto tableIDAndName : formattedNames) {

		SourceDataCache::TableColumns columns = m_sourceDataCache->GetColumnsForTable(tableIDAndName.first);
		SourceDataCache::SharedSQLQuery query = m_sourceDataCache->CreateSelectQuery(tableIDAndName.first, columns, GetSourceIndexesForTable(tableIDAndName.first));
		
		/*}
		else {

			SynGlyphX::IndexSet indexesForTable = GetSelectionSourceIndexesForTable(tableIDAndName.first);
			if (indexesForTable.empty()) {

				continue;
			}
			query = m_filteringManager->GetSourceDataCache()->CreateSelectQuery(tableIDAndName.first, columns, indexesForTable);
		}*/

		query->exec();

		QTableView* tableView = new QTableView(this);
		tableView->setObjectName(tableIDAndName.first);
		QSqlQueryModel* queryModel = new QSqlQueryModel(this);

		queryModel->setQuery(*query.data());
		if (queryModel->lastError().isValid()) {

			throw std::runtime_error("Failed to set SQL query for source data widget.");
		}

		tableView->verticalHeader()->setVisible(false);

		tableView->setModel(queryModel);
		tableView->resizeColumnsToContents();
		tableView->resizeRowsToContents();

		m_sourceDataTabs->addTab(tableView, tableIDAndName.second);

		m_sqlQueryModels.push_back(queryModel);
	}
}
/*
SynGlyphX::IndexSet SourceDataWidget::GetSelectionSourceIndexesForTable(const QString& table) {

	SynGlyphX::IndexSet indexes;

	if (m_filteringManager->GetSceneSelectionModel()->hasSelection()) {

		indexes = SynGlyphX::ItemFocusSelectionModel::GetRootRows(m_filteringManager->GetSceneSelectionModel()->selectedIndexes());
	}

	return indexes;
}*/

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
			WriteToFile(m_sqlQueryModels[m_sourceDataTabs->currentIndex()], filename);

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
	QString initialDir = settings.value("vizSaveDir", "").toString();

	QString sdtFilename = QFileDialog::getSaveFileName(this, tr("Save Current Tab To New Visualization"), initialDir, "SynGlyphX Visualization Files (*.sdt)");
	if (!sdtFilename.isEmpty()) {

		try {
			SynGlyphX::GlyphBuilderApplication::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);

			QFileInfo fileInfo(sdtFilename);
			QString stdCanonicalPath = QDir::toNativeSeparators(fileInfo.absolutePath());

			QString csvFileLocation = stdCanonicalPath + QDir::separator() + fileInfo.baseName() + "_selectedsourcedata.csv";
			WriteToFile(m_sqlQueryModels[m_sourceDataTabs->currentIndex()], csvFileLocation);
			QStringList inputTableValues = m_sourceDataTabs->widget(m_sourceDataTabs->currentIndex())->objectName().split(':');
			boost::uuids::string_generator gen;
			SynGlyphX::InputTable inputTable(gen(inputTableValues[0].toStdWString()), (inputTableValues.size() > 1) ? inputTableValues[1].toStdWString() : SynGlyphX::FileDatasource::SingleTableName);
			SynGlyphX::DataTransformMapping::ConstSharedPtr subsetDataMapping = m_dataTransformMapping->CreateSubsetMappingWithSingleTable(inputTable, csvFileLocation.toStdWString());
			subsetDataMapping->WriteToFile(sdtFilename.toStdString());
			
			settings.setValue("vizSaveDir", stdCanonicalPath);
			SynGlyphX::GlyphBuilderApplication::restoreOverrideCursor();

			m_statusBar->showMessage(tr("Visualization successfully created"), 3000);
		}
		catch (const std::exception& e) {

			QMessageBox::warning(this, tr("Visualization creation failed"), tr("Visualization was not created: ") + e.what());
		}
	}

	settings.endGroup();
}