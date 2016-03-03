#include "adddatabaseserverwizard.h"
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

	SynGlyphX::DatabaseServerDatasource datasource(m_databaseServerInfoWidget->GetDBType(),
												   m_databaseServerInfoWidget->GetConnection().toStdWString(),
												   schema,
												   m_databaseServerInfoWidget->GetUsername().toStdWString(),
												   m_databaseServerInfoWidget->GetPassword().toStdWString());

	QStringList chosenForeignTables = m_tableChoiceWidget->GetChosenForiegnTables();
	if (chosenForeignTables.isEmpty()) {

		SynGlyphX::Datasource::TableNames tables;
		for (const auto& table : m_tableChoiceWidget->GetChosenMainTables()) {

			tables.push_back(table.toStdWString());
		}
		datasource.AddTables(tables);
	}
	else {

		QString chosenMainTable = m_tableChoiceWidget->GetChosenMainTables()[0];
		DataEngine::DataEngineConnection::ForiegnKeyVector allForeignKeysForTable = m_dataEngineConnection->getForeignKeys(chosenMainTable);
		DataEngine::DataEngineConnection::ForiegnKeyVector foreignKeysForJoining;
		for (const auto& foreignKey : allForeignKeysForTable) {

			if (chosenForeignTables.contains(foreignKey.origin)) {

				foreignKeysForJoining.push_back(foreignKey);
			}
		}

		QString query = DataEngine::DataEngineConnection::CreateInnerJoinQueryFromForiegnKeys(chosenMainTable, foreignKeysForJoining);

		SynGlyphX::Datasource::Tables tables;
		tables.insert(std::pair<std::wstring, SynGlyphX::DatasourceTable>(L"Merged", SynGlyphX::DatasourceTable(L"Merged", query.toStdWString())));
		datasource.AddTables(tables);
	}

	return datasource;
}

void AddDatabaseServerWizard::CreateDatabaseInfoPage() {

	QWizardPage* wizardPage = new QWizardPage(this);
	QVBoxLayout* pageLayout = new QVBoxLayout(wizardPage);

	m_databaseServerInfoWidget = new SynGlyphX::DatabaseServerInfoWidget(true, wizardPage);
	m_databaseServerInfoWidget->layout()->setContentsMargins(0, 0, 0, 0);
	pageLayout->addWidget(m_databaseServerInfoWidget);

	wizardPage->setLayout(pageLayout);
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

	m_tableChoiceWidget = new TableChoiceWidget(wizardPage);
	pageLayout->addWidget(m_tableChoiceWidget);

	wizardPage->setLayout(pageLayout);

	wizardPage->setTitle(tr("Select Tables"));

	setPage(TableSelectionPage, wizardPage);
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

		if (m_databaseServerInfoWidget->GetConnection().isEmpty()) {

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

		QStringList tables = m_tableChoiceWidget->GetChosenMainTables();
		if (tables.isEmpty()) {

			QMessageBox::warning(this, tr("Table Selection Error"), tr("At least one main table must be selected."));
			return false;
		}

		m_dataEngineConnection->closeConnection();
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

		TableChoiceWidget::Table2ForiegnKeyTablesMap foriegnKeyTablesMap;
		for (const auto& table : tables) {

			DataEngine::DataEngineConnection::ForiegnKeyVector foriegnKeys = m_dataEngineConnection->getForeignKeys(table);
			QStringList foriegnKeyTables;
			for (const auto& foriegnKey : foriegnKeys) {

				foriegnKeyTables.push_back(foriegnKey.origin);
			}

			foriegnKeyTablesMap[table] = foriegnKeyTables;
 		}

		m_tableChoiceWidget->SetTables(tables, foriegnKeyTablesMap);
	}
}

bool AddDatabaseServerWizard::ValidateDatabaseInfo() {

	SynGlyphX::DatabaseServerDatasource::DBType dbType = m_databaseServerInfoWidget->GetDBType();;

	try {
		m_schemas = m_dataEngineConnection->connectToServer(m_databaseServerInfoWidget->GetConnection(),
															m_databaseServerInfoWidget->GetUsername(),
															m_databaseServerInfoWidget->GetPassword(),
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