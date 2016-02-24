#include "addfiledatasourcewizard.h"
#include "filedatasource.h"
#include <QtWidgets/QMessageBox>

AddFileDatasourceWizard::AddFileDatasourceWizard(const QString& startingDirectory, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
	: SynGlyphX::ValidatedOpenFileDialog(startingDirectory, tr("Add File Datasource"), "CSV files (*.csv);;All datasource files (*.*)", parent),
	m_dataEngineConnection(dataEngineConnection)
{
	
}

AddFileDatasourceWizard::~AddFileDatasourceWizard()
{

}

const QString& AddFileDatasourceWizard::GetFilename() const {

	return m_filename;
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

	m_filename = filename;

	return true;
}