#include "databaseserverinfowidget.h"
#include <QtWidgets/QFormLayout>

namespace SynGlyphX {

	DatabaseServerInfoWidget::DatabaseServerInfoWidget(bool makeEditable, QWidget *parent)
		: QWidget(parent)
	{
		QFormLayout* formLayout = new QFormLayout(this);

		QHBoxLayout* comboBoxLayout = new QHBoxLayout(this);
		m_typeComboBox = new QComboBox(this);
		m_typeComboBox->setEnabled(makeEditable);
		QStringList databaseTypes;
		for (auto dbType : SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings) {

			m_typeComboBox->addItem(QString::fromStdWString(dbType.get_right()), dbType.get_left());
		}
		QObject::connect(m_typeComboBox, &QComboBox::currentTextChanged, this, &DatabaseServerInfoWidget::OnTypeComboBoxChanged);

		comboBoxLayout->addWidget(m_typeComboBox);
		comboBoxLayout->addStretch(1);
		formLayout->addRow(tr("Database Type:"), comboBoxLayout);

		m_connectionLineEdit = new QLineEdit(this);
		m_connectionLineEdit->setReadOnly(!makeEditable);
		m_connectionValidator = new SynGlyphX::PrefixSuffixValidator(this);
		m_connectionLineEdit->setValidator(m_connectionValidator);
		formLayout->addRow(tr("Connection:"), m_connectionLineEdit);
		OnTypeComboBoxChanged();

		m_usernameLineEdit = new QLineEdit(this);
		m_usernameLineEdit->setReadOnly(!makeEditable);
		formLayout->addRow(tr("Username:"), m_usernameLineEdit);

		m_passwordLineEdit = new SynGlyphX::PasswordLineEdit(this);
		m_passwordLineEdit->SetReadOnly(!makeEditable);
		m_passwordLineEdit->layout()->setContentsMargins(0, 0, 0, 0);
		formLayout->addRow(tr("Password:"), m_passwordLineEdit);

		setLayout(formLayout);
	}

	DatabaseServerInfoWidget::~DatabaseServerInfoWidget()
	{

	}

	DatabaseServerDatasource::DBType DatabaseServerInfoWidget::GetDBType() const {

		return static_cast<SynGlyphX::DatabaseServerDatasource::DBType>(m_typeComboBox->currentData().toUInt());
	}

	void DatabaseServerInfoWidget::SetConnection(const QString& connection) {

		QString prefix = connection.left(connection.indexOf(':'));
		m_typeComboBox->setCurrentText(prefix);
		m_connectionLineEdit->setText(connection);
	}

	QString DatabaseServerInfoWidget::GetConnection() const {

		return m_connectionLineEdit->text();
	}

	void DatabaseServerInfoWidget::SetUsername(const QString& username) {

		m_usernameLineEdit->setText(username);
	}

	QString DatabaseServerInfoWidget::GetUsername() const {

		return m_usernameLineEdit->text();
	}

	void DatabaseServerInfoWidget::SetPassword(const QString& password) {

		m_passwordLineEdit->SetPassword(password);
	}

	QString DatabaseServerInfoWidget::GetPassword() const {

		return m_passwordLineEdit->GetPassword();
	}

	void DatabaseServerInfoWidget::OnTypeComboBoxChanged() {

		QString prefixSeparator = QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_prefixSeparator);
		SynGlyphX::DatabaseServerDatasource::DBType dbType = static_cast<SynGlyphX::DatabaseServerDatasource::DBType>(m_typeComboBox->currentData().toUInt());
		QString newPrefix = QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypePrefixes.left.at(dbType));
		QString connection = GetConnection();

		m_connectionValidator->SetPrefix(newPrefix + prefixSeparator);

		SetConnection(connection);
	}

	void DatabaseServerInfoWidget::SetConnectionWithoutPrefix(const QString& connection) {

		m_connectionLineEdit->setText(m_connectionValidator->GetPrefix() + connection);
	}

	QString DatabaseServerInfoWidget::GetConnectionWithoutPrefix() const {

		return m_connectionLineEdit->text().mid(m_connectionValidator->GetPrefix().size());
	}

}//namespace SynGlyphX