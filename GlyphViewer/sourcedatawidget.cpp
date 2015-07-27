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

SourceDataWidget::SourceDataWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, QWidget *parent)
	: QWidget(parent),
	//m_model(model),
	//m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	m_sourceDataTabs = new QTabWidget(this);
	m_sourceDataTabs->setTabsClosable(false);
	mainLayout->addWidget(m_sourceDataTabs, 1);

	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	QPushButton* saveButton = new QPushButton(tr("Save Current Tab To File"), this);
	buttonsLayout->addWidget(saveButton);
	QObject::connect(saveButton, &QPushButton::clicked, this, &SourceDataWidget::SaveCurrentTabToFile);

	buttonsLayout->addStretch(1);

	QPushButton* closeButton = new QPushButton(tr("Close"), this);
	buttonsLayout->addWidget(closeButton);
	QObject::connect(closeButton, &QPushButton::clicked, this, &SourceDataWidget::close);

	mainLayout->addLayout(buttonsLayout);

	m_statusBar = new QStatusBar(this);
	mainLayout->addWidget(m_statusBar);

	setLayout(mainLayout);

	setWindowTitle(tr("Source Data Of Selected Glyphs"));
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

void SourceDataWidget::UpdateTables(const SynGlyphX::SourceDataCache::IndexSetMap& dataIndexes) {

	m_sourceDataTabs->clear();
	m_sqlQueryModels.clear();
	if (!dataIndexes.empty()) {

		const SynGlyphX::SourceDataCache::TableNameMap& formattedNames = m_sourceDataCache->GetFormattedNames();

		for (auto indexSet : dataIndexes) {

			QTableView* tableView = new QTableView(this);
			QSqlQueryModel* queryModel = new QSqlQueryModel(this);
			
			SynGlyphX::TableColumns columns = m_sourceDataCache->GetColumnsForTable(indexSet.first);
			SynGlyphX::SharedSQLQuery query = m_sourceDataCache->CreateSelectQueryForIndexSet(indexSet.first, columns, indexSet.second);
			query->exec();
			queryModel->setQuery(*query.data());
			if (queryModel->lastError().isValid()) {

				throw std::exception("Failed to set SQL query for source data widget.");
			}

			tableView->verticalHeader()->setVisible(false);

			tableView->setModel(queryModel);
			tableView->resizeColumnsToContents();
			tableView->resizeRowsToContents();
			
			m_sourceDataTabs->addTab(tableView, formattedNames.at(indexSet.first));

			m_sqlQueryModels.push_back(queryModel);
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