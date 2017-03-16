#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtWidgets/QHeaderView>
#include <boost/uuid/uuid_io.hpp>
#include "datastatsmodel.h"
#include "datatransformmodel.h"
#include "roledatafilterproxymodel.h"
#include <QtWidgets/QAction>
#include "inputfield.h"
#include "FieldPropertiesDialog.h"
#include "FieldProperties.h"
#include "hashid.h"

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
		catch (const std::exception&) {

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

	int i = 0;
	QString tabName = formattedDatasourceName;
	if (inputTable.GetTable() != L"OnlyTable") {

		tabName += ":" + QString::fromStdWString(inputTable.GetTable());
	}
	SynGlyphX::DataStatsModel* model = new SynGlyphX::DataStatsModel(inputTable, m_model->GetTableStatsMap().at(inputTable), this);
	for (const auto& fieldStats : m_model->GetTableStatsMap().at(inputTable)) {

		SynGlyphX::HashID seed = inputTable.GetHashID();
		SynGlyphX::CombineHashID(seed, fieldStats.at(0).toStdWString());
		std::wstring hashid = std::to_wstring(seed);
		if (m_model->HasFieldProperties(hashid)){
			SynGlyphX::FieldProperties fp = m_model->GetFieldProperties(hashid);
			fp.AddStatsToField(fieldStats);
			model->setData(model->index(i, 2), fp.transformData(2));
			model->setData(model->index(i, 3), fp.transformData(3));
		}
		i++;

	}
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

	view->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
	QAction* m_updateFieldProperties = new QAction(tr("Properties"), view);
	QObject::connect(m_updateFieldProperties, &QAction::triggered, this, &DataSourceStatsWidget::EditFieldProperties);
	view->addAction(m_updateFieldProperties);

	addTab(view, tabName);
}

void DataSourceStatsWidget::OnRowsRemovedFromModel(const QModelIndex& parent, int start, int end) {

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

void DataSourceStatsWidget::EditFieldProperties() {
	
	QTableView* tab = reinterpret_cast<QTableView*>(currentWidget());
	QModelIndex index = tab->currentIndex();
	int row = index.row();

	SynGlyphX::InputTable inputTbl = m_model->GetInputTableForTree(m_model->index(currentIndex()));
	SynGlyphX::DataStatsModel::TableStats tblStats = m_model->GetTableStatsMap().at(inputTbl);
	QStringList stats = tblStats.at(row);

	SynGlyphX::HashID seed = inputTbl.GetHashID();
	SynGlyphX::CombineHashID(seed, stats.at(0).toStdWString());
	std::wstring hashid = std::to_wstring(seed);

	SynGlyphX::FieldProperties field(inputTbl.GetDatasourceID(), inputTbl.GetTable(), stats.at(0).toStdWString());

	if (m_model->HasFieldProperties(hashid)){
		field = m_model->GetFieldProperties(hashid);
	}
	field.AddStatsToField(stats);

	FieldPropertiesDialog dialog(field, this);
	dialog.setWindowTitle(tr("Field Properties"));
	if (dialog.exec() == QDialog::Accepted) {
		field = dialog.SaveSelections(reinterpret_cast<SynGlyphX::DataStatsModel*>(tab->model()), row);
		if (field.GetType() != SynGlyphX::FieldProperties::Type::Default){
			m_model->AddNewFieldProperties(hashid, dialog.SaveSelections(reinterpret_cast<SynGlyphX::DataStatsModel*>(tab->model()), row));
		}
		else{
			if (m_model->HasFieldProperties(hashid)){
				m_model->RemoveFieldProperties(hashid);
			}
		}
	}
	
}
