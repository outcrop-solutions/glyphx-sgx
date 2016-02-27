#include "adddatabaseserverwizard.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>

AddDatabaseServerWizard::AddDatabaseServerWizard(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent)
	: QWizard(parent),
	m_dataEngineConnection(dataEngineConnection)
{
	//setMinimumWidth(512);
	setWindowTitle(tr("Add Database Server"));

	CreateDatabaseInfoPage();
	CreateSchemaSelectionPage();
	CreateTableSelectionPage();
}

AddDatabaseServerWizard::~AddDatabaseServerWizard()
{

}
/*
void AddDatabaseServerWizard::SetValues(const SynGlyphX::DatabaseServerDatasource& datasource) {

	m_typeComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings.left.at(datasource.GetDBType())));
	OnTypeComboBoxChanged();
	SetConnection(QString::fromStdWString(datasource.GetHost()));
	m_usernameLineEdit->setText(QString::fromStdWString(datasource.GetUsername()));
	m_passwordLineEdit->SetPassword(QString::fromStdWString(datasource.GetPassword()));
}*/

SynGlyphX::DatabaseServerDatasource AddDatabaseServerWizard::GetValues() const {

	std::wstring schema = L"";
	if (!m_schemas.isEmpty()) {

		schema = m_schemaListWidget->selectedItems()[0]->text().toStdWString();
	}

	SynGlyphX::DatabaseServerDatasource datasource(static_cast<SynGlyphX::DatabaseServerDatasource::DBType>(m_typeComboBox->currentData().toUInt()),
												   GetConnection().toStdWString(),
												   schema,
												   m_usernameLineEdit->text().toStdWString(),
												   m_passwordLineEdit->GetPassword().toStdWString());

	SynGlyphX::Datasource::TableNames tables;
	for (const auto& table : m_tableChoiceModel->GetChosenTables()) {

		tables.push_back(table.toStdWString());
	}
	datasource.AddTables(tables);

	return datasource;
}

void AddDatabaseServerWizard::CreateDatabaseInfoPage() {

	QWizardPage* wizardPage = new QWizardPage(this);

	QFormLayout* formLayout = new QFormLayout(wizardPage);

	QHBoxLayout* comboBoxLayout = new QHBoxLayout(wizardPage);
	m_typeComboBox = new QComboBox(wizardPage);
	QStringList databaseTypes;
	for (auto dbType : SynGlyphX::DatabaseServerDatasource::s_dbTypeStrings) {

		m_typeComboBox->addItem(QString::fromStdWString(dbType.get_right()), dbType.get_left());
	}
	QObject::connect(m_typeComboBox, &QComboBox::currentTextChanged, this, &AddDatabaseServerWizard::OnTypeComboBoxChanged);

	comboBoxLayout->addWidget(m_typeComboBox);
	comboBoxLayout->addStretch(1);
	formLayout->addRow(tr("Database Type"), comboBoxLayout);

	m_connectionLineEdit = new QLineEdit(wizardPage);
	m_connectionValidator = new SynGlyphX::PrefixSuffixValidator(wizardPage);
	m_connectionLineEdit->setValidator(m_connectionValidator);
	formLayout->addRow(tr("Connection"), m_connectionLineEdit);
	OnTypeComboBoxChanged();

	m_usernameLineEdit = new QLineEdit(wizardPage);
	formLayout->addRow(tr("Username"), m_usernameLineEdit);

	m_passwordLineEdit = new SynGlyphX::PasswordLineEdit(wizardPage);
	m_passwordLineEdit->layout()->setContentsMargins(0, 0, 0, 0);
	formLayout->addRow(tr("Password"), m_passwordLineEdit);

	wizardPage->setLayout(formLayout);

	wizardPage->setTitle(tr("Database Info"));

	setPage(DatabaseInfoPage, wizardPage);
}

void AddDatabaseServerWizard::CreateSchemaSelectionPage() {

	QWizardPage* wizardPage = new QWizardPage(this);

	QVBoxLayout* pageLayout = new QVBoxLayout(wizardPage);
	m_schemaListWidget = new QListWidget(wizardPage);
	m_schemaListWidget->setSortingEnabled(false);
	m_schemaListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	pageLayout->addWidget(m_schemaListWidget);

	wizardPage->setLayout(pageLayout);

	wizardPage->setTitle(tr("Select Schema"));

	setPage(SchemaSelectionPage, wizardPage);
}

void AddDatabaseServerWizard::CreateTableSelectionPage() {

	QWizardPage* wizardPage = new QWizardPage(this);

	QVBoxLayout* pageLayout = new QVBoxLayout(wizardPage);

	QStringList tableChoiceHeaders;
	tableChoiceHeaders << tr("Tables");

	m_tableChoiceModel = new TableChoiceModel(true, this);

	m_tableChoiceWidget = new SynGlyphX::MultiListFilteredTreeWidget(tableChoiceHeaders, m_tableChoiceModel, wizardPage);
	pageLayout->addWidget(m_tableChoiceWidget);

	wizardPage->setLayout(pageLayout);

	wizardPage->setTitle(tr("Select Tables"));

	setPage(TableSelectionPage, wizardPage);
}

void AddDatabaseServerWizard::OnTypeComboBoxChanged() {

	QString prefixSeparator = QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_prefixSeparator);
	SynGlyphX::DatabaseServerDatasource::DBType dbType = static_cast<SynGlyphX::DatabaseServerDatasource::DBType>(m_typeComboBox->currentData().toUInt());
	QString newPrefix = QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypePrefixes.left.at(dbType));
	QString connection = GetConnection();

	m_connectionValidator->SetPrefix(newPrefix + prefixSeparator);

	SetConnection(connection);
}

void AddDatabaseServerWizard::SetConnection(const QString& connection) {

	m_connectionLineEdit->setText(m_connectionValidator->GetPrefix() + connection);
}

QString AddDatabaseServerWizard::GetConnection() const {

	return m_connectionLineEdit->text().mid(m_connectionValidator->GetPrefix().size());
}

int AddDatabaseServerWizard::nextId() const {

	int currentPageId = currentId();
	if (currentPageId == SchemaSelectionPage) {

		return TableSelectionPage;
	}
	else if (currentPageId == DatabaseInfoPage) {

		if (m_schemas.isEmpty()) {

			return TableSelectionPage;
		}
		else {

			return SchemaSelectionPage;
		}
	}

	return -1;
}

bool AddDatabaseServerWizard::validateCurrentPage() {

	int currentPageId = currentId();
	if (currentPageId == DatabaseInfoPage) {

		if (GetConnection().isEmpty()) {

			QMessageBox::warning(this, tr("Invalid value"), tr("Connection must not be an empty value"));
			return false;
		}
		
		return ValidateDatabaseInfo();
	}
	else if (currentPageId == SchemaSelectionPage) {

		if (m_schemaListWidget->selectedItems().isEmpty()) {

			QMessageBox::warning(this, tr("Schema Selection Error"), tr("A schema needs to be selected to continue"));
			return false;
		}
		else if (!DoesSchemaHaveTables(m_schemaListWidget->selectedItems()[0]->text())) {

			QMessageBox::warning(this, tr("Schema Selection Error"), tr("The selected schema has no tables"));
			return false;
		}
		
		return true;
	}
	else if (currentPageId == TableSelectionPage) {

		QStringList tables = m_tableChoiceModel->GetChosenTables();
		if (tables.isEmpty()) {

			QMessageBox::warning(this, tr("Table Selection Error"), tr("At least one table must be selected."));
			return false;
		}

		return true;
	}
	else {

		return QWizard::validateCurrentPage();
	}
}

void AddDatabaseServerWizard::initializePage(int id) {

	if (id == SchemaSelectionPage) {

		m_schemaListWidget->clear();
		m_schemaListWidget->addItems(m_schemas);
	}
	else if (id == TableSelectionPage) {

		QString schema;
		if (!m_schemas.isEmpty()) {

			schema = m_schemaListWidget->selectedItems()[0]->text();
		}

		QStringList tables;
		if (schema.isEmpty()) {
			
			tables = m_dataEngineConnection->getTables();
		}
		else {

			tables = m_dataEngineConnection->getSchemaTableNames(schema);
		}
		m_tableChoiceModel->SetTables(tables);
	}
}

bool AddDatabaseServerWizard::ValidateDatabaseInfo() {

	SynGlyphX::DatabaseServerDatasource::DBType dbType = static_cast<SynGlyphX::DatabaseServerDatasource::DBType>(m_typeComboBox->currentData().toUInt());

	try {
		m_schemas = m_dataEngineConnection->connectToServer(m_connectionLineEdit->text(),
															m_usernameLineEdit->text(),
															m_passwordLineEdit->GetPassword(),
															QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypePrefixes.left.at(dbType)));
		if (m_schemas.isEmpty()) {

			if (m_dataEngineConnection->getTables().isEmpty()) {

				QMessageBox::warning(this, tr("Database Failure"), tr("Connection is valid, but the given database has no tables."));
				return false;
			}
		}

		return true;
	}
	catch (...) {

		QMessageBox::warning(this, tr("Database Connection Failed"), tr("No connection was able to made using the given database information.  Verify that the given database information is correct."));
		return false;
	}
}

bool AddDatabaseServerWizard::DoesSchemaHaveTables(const QString& schema) const {

	return (!m_dataEngineConnection->getSchemaTableNames(schema).isEmpty());
}