#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtWidgets/QHeaderView>
#include <boost/uuid/uuid_io.hpp>
#include "datastatsmodel.h"
#include "sourcedatamanager.h"

DataSourceStatsWidget::DataSourceStatsWidget(DataTransformModel* model, QWidget *parent)
	: QTabWidget(parent),
	m_model(model)
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

    const SynGlyphX::DatasourceMaps::FileDatasourceMap& fileDatasources = m_model->GetDataMapping()->GetDatasources().GetFileDatasources();
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

	const std::wstring& formattedName = datasource.GetFormattedName();
	if (datasource.CanDatasourceHaveMultipleTables()) {

		for (const std::wstring& table : datasource.GetTables()) {

			CreateTableView(id, QString::fromStdWString(table), QString::fromStdWString(formattedName + L":" + table));
		}
	}
	else {

		CreateTableView(id, QString::fromStdWString(*datasource.GetTables().begin()), QString::fromStdWString(formattedName));
	}

	m_datasourcesShownInTabs.insert(id);
}

void DataSourceStatsWidget::CreateTableView(const boost::uuids::uuid& id, const QString& tableName, const QString& tabName) {

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

	addTab(view, tabName);
}