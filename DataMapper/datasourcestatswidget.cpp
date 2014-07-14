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

    const std::unordered_map<std::wstring, SynGlyphX::Datasource>& datasources = m_transform->GetDatasources();
	std::unordered_map<std::wstring, SynGlyphX::Datasource>::const_iterator iT = datasources.begin();
	std::advance(iT, datasources.size() - numNewDatasources);
    for (; iT != datasources.end(); ++iT) {

        QSqlDatabase newDataSourceDB = QSqlDatabase::addDatabase(QString::fromStdWString(iT->second.GetType()), QString::fromStdWString(iT->first));
        newDataSourceDB.setDatabaseName(QString::fromStdWString(iT->second.GetDBName()));

        if (!newDataSourceDB.open()) {
            ClearTabs();
            throw std::exception("Failed to load data sources");
        }

        CreateTablesFromDatasource(newDataSourceDB, iT->second.GetTables());
    }
}

void DataSourceStatsWidget::ClearTabs() {

	clear();
	m_statViews.clear();
}

void DataSourceStatsWidget::CreateTablesFromDatasource(const QSqlDatabase& db, const std::vector<std::wstring>& tables) {

    const QStringList& dbTables = db.tables();

	for (const std::wstring& table : tables) {

        QString qtable = QString::fromStdWString(table);
        if (!dbTables.contains(qtable)) {
            ClearTabs();
            throw std::exception("Table in datatransform does not exist");
        }
        CreateTableView(db, qtable);
	}
}

void DataSourceStatsWidget::CreateTableView(const QSqlDatabase& db, const QString& tableName) {

	QTableView* view = new QTableView(this);
	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setSelectionMode(QAbstractItemView::SingleSelection);
	view->setDragEnabled(true);
	view->setDragDropMode(QAbstractItemView::DragDrop);
	view->setAcceptDrops(false);
	view->setDropIndicatorShown(true);
	QHeaderView* fieldsHeader = view->verticalHeader();
	fieldsHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	fieldsHeader->setSelectionMode(QAbstractItemView::SingleSelection);
	fieldsHeader->setDragEnabled(true);
	fieldsHeader->setDragDropMode(QAbstractItemView::DragDrop);
	fieldsHeader->setAcceptDrops(false);
	fieldsHeader->setDropIndicatorShown(true);

	view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	DataStatsModel* model = new DataStatsModel(db, tableName, this);
	view->setModel(model);
	m_statViews.push_back(view);
	addTab(view, DatabaseServices::GetFormattedDBName(db) + ":" + tableName);
}