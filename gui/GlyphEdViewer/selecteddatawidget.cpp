#include "selecteddatawidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtWidgets/QMessageBox>

SelectedDataWidget::SelectedDataWidget(QWidget *parent, QString cache_location, QString table, QStringList ids)
	: QDialog(parent),
	cache_location(cache_location)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	tableWidget = new QTableWidget(this);

	queryDatabase(table, ids);

	mainLayout->addWidget(tableWidget);

	setLayout(mainLayout);
}

SelectedDataWidget::~SelectedDataWidget()
{

}

void SelectedDataWidget::queryDatabase(QString table_name, QStringList ids)
{
	QString queryStr = "SELECT * FROM " + table_name + " WHERE rowid IN (" + ids.join(",") + ")";

	const QString DRIVER("QSQLITE");

	if (QSqlDatabase::isDriverAvailable(DRIVER)){

		QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);
		db.setDatabaseName(cache_location + "data.db");
		if (!db.open())
			//qWarning() << "ERROR: " << db.lastError();
			QString err = "Error";
		
		QSqlQuery query(queryStr);

		QStringList fields;
		QSqlRecord record = query.record();
		for (int var = 0; var < record.count(); ++var) {
			fields.append(record.fieldName(var));
		}
		tableWidget->setColumnCount(fields.size());
		tableWidget->setHorizontalHeaderLabels(fields);

		query.exec();
		QList<QStringList> rows;
		while (query.next()){
			QStringList row;
			for (QString field : fields) {
				row << query.value(field).toString();
			}
			rows.append(row);
		}
		tableWidget->setRowCount(rows.size());

		for (int i = 0; i < rows.size(); i++){
			for (int j = 0; j < rows.at(i).size(); j++){
				QTableWidgetItem *newItem = new QTableWidgetItem();
				newItem->setText(rows.at(i).at(j));
				tableWidget->setItem(i, j, newItem);
			}
		}

	}
}
