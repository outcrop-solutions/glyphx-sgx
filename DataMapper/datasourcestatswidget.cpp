#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtWidgets/QHeaderView>
#include "datastatsmodel.h"
#include "databaseservices.h"

DataSourceStatsWidget::DataSourceStatsWidget(boost::shared_ptr<const SynGlyphX::DataTransform> transform, QWidget *parent)
	: QTabWidget(parent),
    m_transform(transform)
{
	
}

DataSourceStatsWidget::~DataSourceStatsWidget()
{

}

void DataSourceStatsWidget::RebuildStatsViews() {

    ClearTabs();
	AddNewStatsViews(m_transform->GetDatasources().size());
}

void DataSourceStatsWidget::AddNewStatsViews(const unsigned int numNewDatasources) {

    const SynGlyphX::DataTransform::DatasourceMap& datasources = m_transform->GetDatasources();
	SynGlyphX::DataTransform::DatasourceMap::const_iterator iT = datasources.begin();
	std::advance(iT, datasources.size() - numNewDatasources);
    for (; iT != datasources.end(); ++iT) {

        QSqlDatabase newDataSourceDB = QSqlDatabase::database(QString::fromStdString(boost::uuids::to_string(iT->first)));
        //newDataSourceDB.setDatabaseName(QString::fromStdWString(iT->second.GetDBName()));

        if (!newDataSourceDB.open()) {
            ClearTabs();
            throw std::exception("Failed to load data sources");
        }

        CreateTablesFromDatasource(iT->first, newDataSourceDB, iT->second.GetTables());
    }
}

void DataSourceStatsWidget::ClearTabs() {

	clear();
	m_statViews.clear();
}

void DataSourceStatsWidget::CreateTablesFromDatasource(const boost::uuids::uuid& id, QSqlDatabase& db, const std::vector<std::wstring>& tables) {

    const QStringList& dbTables = db.tables();

	for (const std::wstring& table : tables) {

        QString qtable = QString::fromStdWString(table);
        if (!dbTables.contains(qtable)) {
            ClearTabs();
            throw std::exception("Table in datatransform does not exist");
        }
        CreateTableView(id, db, qtable);
	}
}

void DataSourceStatsWidget::CreateTableView(const boost::uuids::uuid& id, const QSqlDatabase& db, const QString& tableName) {

	QTableView* view = new QTableView(this);
	view->setSelectionMode(QAbstractItemView::SingleSelection);
	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setDragEnabled(true);
	view->setDragDropMode(QAbstractItemView::DragDrop);
	view->setDropIndicatorShown(true);
	view->setAcceptDrops(false);
	
	view->verticalHeader()->hide();

	DataStatsModel* model = new DataStatsModel(id, tableName, this);
	view->setModel(model);

	view->resizeColumnsToContents();
	view->resizeRowsToContents();
	
	m_statViews.push_back(view);
	
	addTab(view, DatabaseServices::GetFormattedDBName(db) + ":" + tableName);
}