#include "adddatabaseserverdialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>

AddDatabaseServerDialog::AddDatabaseServerDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Add Database Server Datasource"));

	QFormLayout* formLayout = new QFormLayout(this);

	m_typeComboBox = new QComboBox(this);
	QStringList databaseTypes;
	for (auto dbType : SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings) {

		m_typeComboBox->addItem(QString::fromStdWString(dbType.get_right()));
	}
	formLayout->addRow(tr("Type"), m_typeComboBox);

	m_connectionLineEdit = new QLineEdit(this);
	formLayout->addRow(tr("Connection"), m_connectionLineEdit);

	m_usernameLineEdit = new QLineEdit(this);
	formLayout->addRow(tr("Username"), m_usernameLineEdit);

	m_passwordLineEdit = new SynGlyphX::PasswordLineEdit(this);
	formLayout->addRow(tr("Password"), m_passwordLineEdit);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(formLayout, 1);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	setLayout(mainLayout);
}

AddDatabaseServerDialog::~AddDatabaseServerDialog()
{

}

void AddDatabaseServerDialog::SetValues(const SynGlyphX::DatabaseServerDatasource& datasource) {

	m_typeComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings.left.at(datasource.GetDBType())));
	m_connectionLineEdit->setText(QString::fromStdWString(datasource.GetHost()));
	m_usernameLineEdit->setText(QString::fromStdWString(datasource.GetUsername()));
	m_passwordLineEdit->SetPassword(QString::fromStdWString(datasource.GetPassword()));
}

SynGlyphX::DatabaseServerDatasource AddDatabaseServerDialog::GetValues() const {

	SynGlyphX::DatabaseServerDatasource datasource(SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings.right.at(m_typeComboBox->currentText().toStdWString()),
												   m_connectionLineEdit->text().toStdWString(),
												   m_usernameLineEdit->text().toStdWString(),
												   m_passwordLineEdit->GetPassword().toStdWString());

	return datasource;
}

void AddDatabaseServerDialog::accept() {

	if (m_connectionLineEdit->text().isEmpty()) {

		QMessageBox::warning(this, tr("Invalid value"), tr("Connection must not be empty"));
		return;
	}
	else {

		
	}

	QDialog::accept();
}