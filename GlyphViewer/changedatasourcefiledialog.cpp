#include "changedatasourcefiledialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "databaseservices.h"

ChangeDatasourceFileDialog::ChangeDatasourceFileDialog(const QString& oldDatasourceFile, QWidget *parent)
	: QDialog(parent)
{
	QFormLayout* layout = new QFormLayout(this);

	QLabel* oldDatasourceLabel = new QLabel(oldDatasourceFile, this);
	layout->addRow(tr("Old Datasource:"), oldDatasourceLabel);
	m_newDatasourceFileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
	m_newDatasourceFileLineEdit->SetInitalBrowseDirectory(oldDatasourceFile);
	m_newDatasourceFileLineEdit->SetText(oldDatasourceFile);
	layout->addRow(tr("New Datasource:"), m_newDatasourceFileLineEdit);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ChangeDatasourceFileDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ChangeDatasourceFileDialog::reject);

	setLayout(layout);

	setWindowTitle(tr("Change Datasource File"));
}

ChangeDatasourceFileDialog::~ChangeDatasourceFileDialog()
{

}

QString ChangeDatasourceFileDialog::GetNewDatasourceFile() const {

	return m_newDatasourceFileLineEdit->GetText();
}

void ChangeDatasourceFileDialog::accept() {

	if (ValidateNewDatasourceFile()) {

		QDialog::accept();
	}
	else {

		QMessageBox::warning(this, tr("Change Datasource"), tr("New datasource is not a valid replacement for old datasource."));
	}
}

bool ChangeDatasourceFileDialog::ValidateNewDatasourceFile() const {

	QString newDatasource = m_newDatasourceFileLineEdit->GetText();
	if (SynGlyphX::DatabaseServices::IsSQLiteDB(newDatasource)) {

		return true;
	}

	return false;
}