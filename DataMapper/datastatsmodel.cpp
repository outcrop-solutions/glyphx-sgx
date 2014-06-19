#include "datastatsmodel.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>

DataStatsModel::DataStatsModel(const QString& tableName, QObject *parent)
	: QAbstractTableModel(parent)
{
	QSqlDatabase database = QSqlDatabase::database();
	QSqlRecord columnNamesRecord = database.record(tableName);

	QSqlQuery query;
	//query.prepare("SELECT min(:colName), max(:colName), AVG(:colName), COUNT(:colName) FROM " + tableName);

	for (int i = 0; i < columnNamesRecord.count(); ++i) {

		QSqlField field = columnNamesRecord.field(i);
		query.prepare(QString("SELECT TYPEOF(%1), MIN(%1), MAX(%1), AVG(%1), COUNT(%1) FROM ").arg("\"" + field.name() + "\"") + tableName);
		m_columnNames.append(field.name());
		//query.bindValue(":colName", "\"" + columnNamesRecord.fieldName(i) + "\"");
		query.exec();
		query.first();

		QSqlRecord record = query.record();
		QStringList columnStats;
		//columnStats.append(QVariant::typeToName(field.type()));
		columnStats.append(record.value(0).toString());
		columnStats.append(record.value(1).toString());
		columnStats.append(record.value(2).toString());
		if (field.type() == QVariant::String) {
			columnStats.append("N/A");
		}
		else {
			columnStats.append(record.value(3).toString());
		}
		columnStats.append(record.value(4).toString());
		m_stats.append(columnStats);
	}
	
	/*
	query.addBindValue(m_columnNames);
	query.addBindValue(m_columnNames);
	query.addBindValue(m_columnNames);
	query.addBindValue(m_columnNames);

	bool success = query.execBatch();

	while (query.next()) {

		QSqlRecord record = query.record();
		QStringList columnStats;
		columnStats.append(record.value(0).typeName());
		columnStats.append(record.value(1).typeName());
		columnStats.append(record.value(2).typeName());
		columnStats.append(record.value(3).typeName());
		m_stats.append(columnStats);
	}*/
}

DataStatsModel::~DataStatsModel()
{

}

int DataStatsModel::rowCount(const QModelIndex& parent) const {

	return 5;
}

int DataStatsModel::columnCount(const QModelIndex& parent) const {

	return m_columnNames.length();
}

QVariant DataStatsModel::data(const QModelIndex& index, int role) const {

	if ((role == Qt::DisplayRole) && (index.isValid())) {
		
		return m_stats.at(index.column()).at(index.row());
	}

	return QVariant();
}

QVariant DataStatsModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {

			return m_columnNames[section];
		}
		else {
			if (section == 0) {
				return "Type";
			}
			else if (section == 1) {
				return "Min";
			}
			else if (section == 2) {
				return "Max";
			}
			else if (section == 3) {
				return "Average";
			}
			else if (section == 4) {
				return "Count";
			}
		}
	}

	return QVariant();
}