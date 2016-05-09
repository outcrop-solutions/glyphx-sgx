#include "addfiledatasourcewizard.h"
#include <QtWidgets/QMessageBox>
#include "tablechoicewidget.h"
#include "singlewidgetdialog.h"

AddFileDatasourceWizard::AddFileDatasourceWizard(const QString& startingDirectory, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
	: SynGlyphX::ValidatedOpenFileDialog(startingDirectory, tr("Add File Datasource"), "CSV files (*.csv);;SQLite DB files(*.*);;All datasource files (*.*)", parent),
	m_dataEngineConnection(dataEngineConnection),
	m_fileDatasource(SynGlyphX::FileDatasource::FileType::CSV, L"")
{
	
}

AddFileDatasourceWizard::~AddFileDatasourceWizard()
{

}

const SynGlyphX::FileDatasource& AddFileDatasourceWizard::GetFileDatasource() const {

	return m_fileDatasource;
}

bool AddFileDatasourceWizard::IsFileValid(const QString& filename) {

	SynGlyphX::FileDatasource::FileType fileType;
	try {

		fileType = SynGlyphX::FileDatasource::GetFileTypeForFile(filename.toStdWString());
	}
	catch (const std::exception& e) {

		QMessageBox::warning(dynamic_cast<QWidget*>(parent()), tr("Invalid File"), tr(e.what()), QMessageBox::StandardButton::Ok);
		return false;
	}

	QStringList chosenTables;
	if (SynGlyphX::FileDatasource::CanFileTypeHaveMultipleTables(fileType)) {

		TableChoiceWidget* tableChoiceWidget = new TableChoiceWidget(dynamic_cast<QWidget*>(parent()));
		SynGlyphX::SingleWidgetDialog tableChoiceDialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, tableChoiceWidget, dynamic_cast<QWidget*>(parent()));
		tableChoiceDialog.setWindowTitle(tr("Select Tables"));

		QString url = QString::fromStdWString(L"sqlite:") + filename;
		QString user("");
		QString pass("");
		QStringList databases = m_dataEngineConnection->connectToServer(url, user, pass, "sqlite3");

		QStringList tables = m_dataEngineConnection->getTables();
		tableChoiceWidget->SetTables(tables);

		if (tableChoiceDialog.exec() == QDialog::Accepted) {

			chosenTables = tableChoiceWidget->GetChosenMainTables();
			if (chosenTables.isEmpty()) {

				QMessageBox::warning(dynamic_cast<QWidget*>(parent()), tr("Table Choice Error"), tr("At least one table must be chosen from the data source."), QMessageBox::StandardButton::Ok);
				return false;
			}
			m_dataEngineConnection->setChosenTables(chosenTables);
		}
		else {

			return false;
		}
	}
	else if (fileType == SynGlyphX::FileDatasource::FileType::CSV) {

		//m_dataEngineConnection->loadCSV(filename.toStdString());
	}

	m_fileDatasource = SynGlyphX::FileDatasource(fileType, filename.toStdWString());
	if (!chosenTables.isEmpty()) {

		SynGlyphX::Datasource::TableNames tableNames;
		for (const auto& table : chosenTables) {

			tableNames.push_back(table.toStdWString());
		}
		m_fileDatasource.AddTables(tableNames);
	}

	//m_dataEngineConnection->closeConnection();

	return true;
}