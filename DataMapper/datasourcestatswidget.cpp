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

void DataSourceStatsWidget::SetDataEngineConn(DataEngine::DataEngineConnection *dec){
	this->dec = dec;
}

void DataSourceStatsWidget::AddNewStatsViews() {

	const SynGlyphX::DatasourceMaps::FileDatasourceMap& fileDatasources = m_model->GetDataMapping()->GetDatasources().GetFileDatasources();
	SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator iT = fileDatasources.begin();
	for (; iT != fileDatasources.end(); ++iT) {

		try {
			if (findChildren<QTableView*>(QString::fromStdString(boost::uuids::to_string(iT->first))).empty()) {
				QString datasource = QString::fromStdWString(iT->second.GetFilename());

				if (iT->second.GetType() == SynGlyphX::FileDatasource::SQLITE3){

					SynGlyphX::Datasource::TableNames tables;

					QString url("sqlite:" + datasource);
					QString user("");
					QString pass("");
					QString type("sqlite");
					//QString url("mysql://33.33.33.1");
					//QString user("root");
					//QString pass("jarvis");
					//QString type("mysql");
					QStringList databases = dec->connectToServer(url, user, pass, type); 
					QString database("");
					//QString database("world");
					QStringList qtables = dec->chooseDatabase(database);
					QStringList chosenTables;
					chosenTables = qtables;
					dec->setChosenTables(chosenTables);

					if (!qtables.isEmpty()) {
						for (const QString& qtable : qtables) {
							tables.insert(qtable.toStdWString());
						}
						m_model->EnableTables(iT->first, tables, true);
					}
					else {
						throw std::exception((tr("No tables in ") + datasource).toStdString().c_str());
					}

					for (int i = 0; i < qtables.size(); i++){
						CreateTablesFromDatasource(iT->first, i, QString::fromStdWString(iT->second.GetFormattedName()) + ":" + qtables[i], iT->second.GetType());
					}
				}
				else if (iT->second.GetType() == SynGlyphX::FileDatasource::CSV){
					dec->loadCSV(datasource.toUtf8().constData());
					CreateTablesFromDatasource(iT->first, 0, QString::fromStdWString(iT->second.GetFormattedName()), iT->second.GetType());
				}
			}
		}catch (const std::exception& e) {

			ClearTabs();
			throw;
		}
	}

}

void DataSourceStatsWidget::ClearTabs() {

	RemoveTableViews();
	clear();
}

void DataSourceStatsWidget::CreateTablesFromDatasource(const boost::uuids::uuid& id, int place, QString file, SynGlyphX::FileDatasource::SourceType type) {

	QString idString = QString::fromStdString(boost::uuids::to_string(id));
	QString tableName;
	if (file.right(4).toLower() == ".csv"){
		tableName = "OnlyTable";
	}
	else{
		QStringList qsl = file.split(":");
		tableName = qsl.at(1);
	}
	DataStatsModel* model = new DataStatsModel(id, place, type, tableName, this->dec, this);
	CreateTableView(model, file, idString);
	std::wstring temp = file.toStdWString();
	dec->addTableName(id, tableName.toStdWString());
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
				RemoveTableViews(id);
			}
		}
	}
}

void DataSourceStatsWidget::RemoveTableViews(const QString& name) {

	//This will also delete the associated model since it is the parent of the model object
	while (QTableView* view = findChild<QTableView*>(name)) {

		removeTab(indexOf(view));
		delete view;
	}
}