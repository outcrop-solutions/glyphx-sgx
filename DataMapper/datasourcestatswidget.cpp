#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include "datastatsmodel.h"
#include "databaseservices.h"

DataSourceStatsWidget::DataSourceStatsWidget(QWidget *parent)
	: QTabWidget(parent)
{
	
}

DataSourceStatsWidget::~DataSourceStatsWidget()
{

}

void DataSourceStatsWidget::RebuildStatsViews() {

	ClearTabs();

	QSqlDatabase projectDatabase = QSqlDatabase::database(DatabaseServices::GetProjectDBConnectionName());
	QSqlQuery dataSourceListQuery(projectDatabase);
	dataSourceListQuery.prepare("SELECT ConnectionName,DatabaseName FROM DataSources");
	dataSourceListQuery.exec();

	while (dataSourceListQuery.next()) {
		QSqlRecord record = dataSourceListQuery.record();
		QSqlDatabase newDataSourceDB = QSqlDatabase::addDatabase("QSQLITE", record.value(0).toString());
		newDataSourceDB.setDatabaseName(record.value(1).toString());

		if (!newDataSourceDB.open()) {
			ClearTabs();
			throw std::exception("Failed to load data sources");
		}

		CreateTablesFromDB(newDataSourceDB);
	}
}

void DataSourceStatsWidget::ClearTabs() {

	clear();
	m_statViews.clear();
}

void DataSourceStatsWidget::CreateTablesFromDB(const QSqlDatabase& db) {

	QStringList tables = db.tables();
	DatabaseServices::RemoveAutoGeneratedTableNamesFromList(tables);
	for (int i = 0; i < tables.length(); ++i) {
		CreateTableView(db, tables[i]);
	}
}

void DataSourceStatsWidget::CreateTableView(const QSqlDatabase& db, const QString& tableName) {

	QTableView* view = new QTableView(this);

	DataStatsModel* model = new DataStatsModel(db, tableName, this);
	view->setModel(model);
	view->resizeColumnsToContents();
	addTab(view, DatabaseServices::GetFormattedDBName(db) + ":" + tableName);
}