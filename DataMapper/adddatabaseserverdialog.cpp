#include "adddatabaseserverdialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>

AddDatabaseServerDialog::AddDatabaseServerDialog(QWidget *parent)
	: QDialog(parent)
{
	setMinimumWidth(512);
	setWindowTitle(tr("Add Database Server"));

	QFormLayout* formLayout = new QFormLayout(this);

	QHBoxLayout* comboBoxLayout = new QHBoxLayout(this);
	m_typeComboBox = new QComboBox(this);
	QStringList databaseTypes;
	for (auto dbType : SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings) {

		m_typeComboBox->addItem(QString::fromStdWString(dbType.get_right()), dbType.get_left());
	}
	QObject::connect(m_typeComboBox, &QComboBox::currentTextChanged, this, &AddDatabaseServerDialog::OnTypeComboBoxChanged);

	comboBoxLayout->addWidget(m_typeComboBox);
	comboBoxLayout->addStretch(1);
	formLayout->addRow(tr("Database Type"), comboBoxLayout);

	m_connectionLineEdit = new QLineEdit(this);
	m_connectionValidator = new SynGlyphX::PrefixSuffixValidator(this);
	m_connectionLineEdit->setValidator(m_connectionValidator);
	formLayout->addRow(tr("Connection"), m_connectionLineEdit);
	OnTypeComboBoxChanged();

	m_usernameLineEdit = new QLineEdit(this);
	formLayout->addRow(tr("Username"), m_usernameLineEdit);

	m_passwordLineEdit = new SynGlyphX::PasswordLineEdit(this);
	m_passwordLineEdit->layout()->setContentsMargins(0, 0, 0, 0);
	formLayout->addRow(tr("Password"), m_passwordLineEdit);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	formLayout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	setLayout(formLayout);
}

AddDatabaseServerDialog::~AddDatabaseServerDialog()
{

}

void AddDatabaseServerDialog::SetValues(const SynGlyphX::DatabaseServerDatasource& datasource) {

	m_typeComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings.left.at(datasource.GetDBType())));
	OnTypeComboBoxChanged();
	SetConnection(QString::fromStdWString(datasource.GetHost()));
	m_usernameLineEdit->setText(QString::fromStdWString(datasource.GetUsername()));
	m_passwordLineEdit->SetPassword(QString::fromStdWString(datasource.GetPassword()));
}

SynGlyphX::DatabaseServerDatasource AddDatabaseServerDialog::GetValues() const {

	SynGlyphX::DatabaseServerDatasource datasource(static_cast<SynGlyphX::DatabaseServerDatasource::DBType>(m_typeComboBox->currentData().toUInt()),
												   GetConnection().toStdWString(),
												   m_usernameLineEdit->text().toStdWString(),
												   m_passwordLineEdit->GetPassword().toStdWString());

	return datasource;
}

void AddDatabaseServerDialog::OnTypeComboBoxChanged() {

	QString prefixSeparator = QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_prefixSeparator);
	SynGlyphX::DatabaseServerDatasource::DBType dbType = static_cast<SynGlyphX::DatabaseServerDatasource::DBType>(m_typeComboBox->currentData().toUInt());
	QString newPrefix = QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypePrefixes.left.at(dbType));
	QString connection = GetConnection();

	m_connectionValidator->SetPrefix(newPrefix + prefixSeparator);

	SetConnection(connection);
}

void AddDatabaseServerDialog::SetConnection(const QString& connection) {

	m_connectionLineEdit->setText(m_connectionValidator->GetPrefix() + connection);
}

QString AddDatabaseServerDialog::GetConnection() const {

	return m_connectionLineEdit->text().mid(m_connectionValidator->GetPrefix().size());
}

void AddDatabaseServerDialog::accept() {

	if (GetConnection().isEmpty()) {

		QMessageBox::warning(this, tr("Invalid value"), tr("Connection must not be an empty value"));
		return;
	}
	else {

		
	}

	QDialog::accept();
}