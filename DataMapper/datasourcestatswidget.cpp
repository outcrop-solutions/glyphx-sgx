#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtWidgets/QHeaderView>
#include <boost/uuid/uuid_io.hpp>
#include "datastatsmodel.h"
#include "sourcedatamanager.h"

DataSourceStatsWidget::DataSourceStatsWidget(DataTransformModel* dataTransformModel, QWidget *parent)
	: QTabWidget(parent),
	m_model(dataTransformModel)
{
	QObject::connect(m_model, &DataTransformModel::modelReset, this, &DataSourceStatsWidget::RebuildStatsViews);
	QObject::connect(m_model, &DataTransformModel::rowsAboutToBeRemoved, this, &DataSourceStatsWidget::OnRowsRemovedFromModel);
}

DataSourceStatsWidget::~DataSourceStatsWidget()
{

}

QSize DataSourceStatsWidget::sizeHint() const {

	return QSize(256, 256);
}

void DataSourceStatsWidget::RebuildStatsViews() {

    ClearTabs();
	AddNewStatsViews();
}

void DataSourceStatsWidget::AddNewStatsViews() {

	const SynGlyphX::DatasourceMaps::FileDatasourceMap& fileDatasources = m_model->GetDataMapping()->GetDatasources().GetFileDatasources();
	SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator iT = fileDatasources.begin();
	for (; iT != fileDatasources.end(); ++iT) {

		try {

			if (findChildren<QTableView*>(QString::fromStdString(boost::uuids::to_string(iT->first))).empty()) {

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

	QList<QTableView*> tabsToBeRemoved = findChildren<QTableView*>("");
	for (QTableView* view : tabsToBeRemoved) {

		//This will also delete the associated model since it is the parent of the model object
		view->setParent(nullptr);
		delete view;
	}
	clear();
}

void DataSourceStatsWidget::CreateTablesFromDatasource(const boost::uuids::uuid& id, const SynGlyphX::Datasource& datasource) {

	QString idString = QString::fromStdString(boost::uuids::to_string(id));
	const std::wstring& formattedName = datasource.GetFormattedName();
	if (datasource.CanDatasourceHaveMultipleTables()) {

		QSqlDatabase datasourceDB = QSqlDatabase::database(idString);

		if (!datasourceDB.open()) {

			throw std::exception("Failed to load data sources");
		}

		for (const auto& table : datasource.GetTables()) {

			QString tableName = QString::fromStdWString(table.first);
			DataStatsModel* model = new DataStatsModel(id, tableName, this);
			CreateTableView(model, QString::fromStdWString(formattedName) + ":" + tableName, idString);
		}
	}
	else {

		DataStatsModel* model = new DataStatsModel(id, m_model->GetCacheConnectionID(), QString::fromStdWString(SynGlyphX::Datasource::SingleTableName), this);
		CreateTableView(model, QString::fromStdWString(formattedName), idString);
	}
}

void DataSourceStatsWidget::CreateTableView(DataStatsModel* model, const QString& tabName, const QString& id) {

	QTableView* view = new QTableView(this);
	view->setObjectName(id);
	view->setSelectionMode(QAbstractItemView::SingleSelection);
	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setDragEnabled(true);
	view->setDragDropMode(QAbstractItemView::DragDrop);
	view->setDropIndicatorShown(true);
	view->setAcceptDrops(false);
	
	view->verticalHeader()->hide();

	model->setParent(view);
	view->setModel(model);

	view->resizeColumnsToContents();
	view->resizeRowsToContents();

	addTab(view, tabName);
}

void DataSourceStatsWidget::OnRowsRemovedFromModel(const QModelIndex& parent, int start, int end) {

	bool rebuildStatsViews = false;
	if (!parent.isValid()) {

		for (int i = start; i <= end; ++i) {

			QModelIndex index = m_model->index(i);
			if (m_model->data(index, DataTransformModel::DataTypeRole).toInt() == DataTransformModel::DataType::DataSources) {

				QString id = m_model->data(index, DataTransformModel::UUIDRole).toString();
				QList<QTableView*> tabsToBeRemoved = findChildren<QTableView*>(id);
				for (QTableView* view : tabsToBeRemoved) {

					removeTab(indexOf(view));

					//This will also delete the associated model since it is the parent of the model object
					view->setParent(nullptr);
					delete view;
				}
			}
		}
	}
}