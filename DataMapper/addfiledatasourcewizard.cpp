#include "addfiledatasourcewizard.h"

AddFileDatasourceWizard::AddFileDatasourceWizard(const QString& startingDirectory, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
	: QFileDialog(parent, tr("Add File Datasource"), startingDirectory),
	m_dataEngineConnection(dataEngineConnection)
{
	QStringList suffixFilters;
	suffixFilters << "CSV files (*.csv)" << "All datasource files (*.*)";
	setNameFilters(suffixFilters);

	setAcceptMode(QFileDialog::AcceptOpen);
	setFileMode(QFileDialog::ExistingFile);
}

AddFileDatasourceWizard::~AddFileDatasourceWizard()
{

}

void AddFileDatasourceWizard::accept() {

	QString selectedFile = selectedFiles()[0];
	//QFileDialog::accept();
}