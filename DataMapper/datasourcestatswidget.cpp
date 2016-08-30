#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtWidgets/QHeaderView>
#include <boost/uuid/uuid_io.hpp>
#include "datastatsmodel.h"
#include "datatransformmodel.h"
#include "roledatafilterproxymodel.h"

DataSourceStatsWidget::DataSourceStatsWidget(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent)
	: QTabWidget(parent),
	m_model(dataTransformModel)
{
	QObject::connect(m_model, &SynGlyphX::DataTransformModel::modelReset, this, &DataSourceStatsWidget::RebuildStatsViews);
	QObject::connect(m_model, &SynGlyphX::DataTransformModel::rowsAboutToBeRemoved, this, &DataSourceStatsWidget::OnRowsRemovedFromModel);
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

void DataSourceStatsWidget::SetDataEngineConnection(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection) {
	
	m_dataEngineConnection = dataEngineConnection;
}

void DataSourceStatsWidget::AddNewStatsViews() {

	const SynGlyphX::DataTransformMapping::DatasourceMap& datasources = m_model->GetDataMapping()->GetDatasources();
	SynGlyphX::DataTransformMapping::DatasourceMap::const_iterator iT = datasources.begin();
	for (; iT != datasources.end(); ++iT) {

		try {
			if (findChildren<QTableView*>(QString::fromStdString(boost::uuids::to_string(iT->first))).empty()) {

				for (const auto& table : iT->second->GetTableNames()) {

					SynGlyphX::InputTable inputTable(iT->first, table);
					CreateTablesFromDatasource(inputTable, QString::fromStdWString(iT->second->GetFormattedName()));
				}
			}
		}
		catch (const std::exception& e) {

			ClearTabs();
			throw;
		}
	}

}

void DataSourceStatsWidget::ClearTabs() {

	RemoveTableViews();
	clear();
}

void DataSourceStatsWidget::CreateTablesFromDatasource(const SynGlyphX::InputTable& inputTable, const QString& formattedDatasourceName) {

	QString tabName = formattedDatasourceName;
	if (inputTable.GetTable() != L"OnlyTable") {

		tabName += ":" + QString::fromStdWString(inputTable.GetTable());
	}
	SynGlyphX::DataStatsModel* model = new SynGlyphX::DataStatsModel(inputTable, m_model->GetTableStatsMap().at(inputTable), this);
	CreateTableView(model, tabName, QString::fromStdString(boost::uuids::to_string(inputTable.GetDatasourceID())));
}

void DataSourceStatsWidget::CreateTableView(SynGlyphX::DataStatsModel* model, const QString& tabName, const QString& id) {

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
			if (m_model->data(index, SynGlyphX::DataTransformModel::DataTypeRole).toInt() == SynGlyphX::DataTransformModel::DataType::DataSources) {

				QString id = m_model->data(index, SynGlyphX::DataTransformModel::UUIDRole).toString();
				RemoveTableViews(id);
			}
		}
	}
}

void DataSourceStatsWidget::RemoveTableViews(const QString& name) {

	//This will also delete the associated model since it is the parent of the model object
	while (QTableView* view = findChild<QTableView*>(name)) {

		view->clearSelection();
		view->clearFocus();
		removeTab(indexOf(view));
		delete view;
	}
}