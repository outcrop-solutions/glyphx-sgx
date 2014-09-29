#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtWidgets/QHeaderView>
#include <boost/uuid/uuid_io.hpp>
#include "datastatsmodel.h"
#include "databaseservices.h"

DataSourceStatsWidget::DataSourceStatsWidget(boost::shared_ptr<const SynGlyphX::DataTransformMapping> mapping, QWidget *parent)
	: QTabWidget(parent),
    m_mapping(mapping)
{
	
}

DataSourceStatsWidget::~DataSourceStatsWidget()
{

}

void DataSourceStatsWidget::RebuildStatsViews() {

	m_datasourcesShownInTabs.clear();
    ClearTabs();
	AddNewStatsViews();
}

void DataSourceStatsWidget::AddNewStatsViews() {

    const SynGlyphX::DatasourceMaps::FileDatasourceMap& fileDatasources = m_mapping->GetDatasources().GetFileDatasources();
	SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator iT = fileDatasources.begin();
	for (; iT != fileDatasources.end(); ++iT) {

		try {
			if (m_datasourcesShownInTabs.find(iT->first) == m_datasourcesShownInTabs.end()) {

				CreateTablesFromDatasource(iT->first, iT->second);
			}
		}
		catch (const std::exception& e) {

			ClearTabs();
			throw;
		}
    }
}

void DataSourceStatsWidget::ClearTabs() {

	clear();
	m_statViews.clear();
}

void DataSourceStatsWidget::CreateTablesFromDatasource(const boost::uuids::uuid& id, const SynGlyphX::Datasource& datasource) {

	QSqlDatabase datasourceDB = QSqlDatabase::database(QString::fromStdString(boost::uuids::to_string(id)));

	if (!datasourceDB.open()) {
		
		throw std::exception("Failed to load data sources");
	}

	if (datasource.GetTables().empty()) {

		CreateTableView(id, datasourceDB, "");
	}
	else {

		for (const std::wstring& table : datasource.GetTables()) {

			CreateTableView(id, datasourceDB, QString::fromStdWString(table));
		}
	}

	m_datasourcesShownInTabs.insert(id);
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
	
	QString tabName = SynGlyphX::DatabaseServices::GetFormattedDBName(db);
	if (!tableName.isEmpty()) {

		tabName += ":" + tableName;
	}

	addTab(view, tabName);
}