#include "datasourcestatswidget.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtSql/QSqlQueryModel>

DataSourceStatsWidget::DataSourceStatsWidget(QWidget *parent)
	: QTabWidget(parent)
{
	
}

DataSourceStatsWidget::~DataSourceStatsWidget()
{

}

void DataSourceStatsWidget::RebuildStatsViews() {

	clear();
	m_statViews.clear();

	QSqlQuery dataSourceListQuery;
	dataSourceListQuery.prepare("SELECT ID,DatabaseName,TableName FROM DataSources");
	dataSourceListQuery.exec();

	while (dataSourceListQuery.next()) {
		QSqlRecord record = dataSourceListQuery.record();
		QTableView* view = new QTableView(this);
		QSqlQueryModel* model = new QSqlQueryModel(this);
		model->setQuery("SELECT * FROM DataSource" + record.value(0).toString());
		view->setModel(model);
		view->resizeColumnsToContents();
		addTab(view, record.value(1).toString() + ":" + record.value(2).toString());
	}
}