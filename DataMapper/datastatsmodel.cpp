#include "datastatsmodel.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QtCore/QMimeData>
#include "databaseservices.h"

DataStatsModel::DataStatsModel(const QSqlDatabase& db, const QString& tableName, QObject *parent)
	: QAbstractTableModel(parent),
	m_db(db),
	m_tableName(tableName)
{
	QSqlRecord columnNamesRecord = db.record(tableName);

	QSqlQuery query(db);
	//query.prepare("SELECT min(:colName), max(:colName), AVG(:colName), COUNT(:colName) FROM " + tableName);

	for (int i = 0; i < columnNamesRecord.count(); ++i) {

		QSqlField field = columnNamesRecord.field(i);
		query.prepare(QString("SELECT TYPEOF(%1), MIN(%1), MAX(%1), AVG(%1), COUNT(%1) FROM ").arg("\"" + field.name() + "\"") + tableName);
		m_fieldNames.append(field.name());
		//query.bindValue(":colName", "\"" + columnNamesRecord.fieldName(i) + "\"");
		query.exec();
		query.first();

		QSqlRecord record = query.record();
		QStringList fieldStats;
		//columnStats.append(QVariant::typeToName(field.type()));
		fieldStats.append(record.value(0).toString());
		fieldStats.append(record.value(1).toString());
		fieldStats.append(record.value(2).toString());
		if (field.type() == QVariant::String) {
			fieldStats.append("N/A");
		}
		else {
			fieldStats.append(record.value(3).toString());
		}
		fieldStats.append(record.value(4).toString());
		m_stats.append(fieldStats);
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

	return m_fieldNames.length();
}

int DataStatsModel::columnCount(const QModelIndex& parent) const {

	return 5;
}

QVariant DataStatsModel::data(const QModelIndex& index, int role) const {

	if ((role == Qt::DisplayRole) && (index.isValid())) {
		
		return m_stats.at(index.row()).at(index.column());
	}

	return QVariant();
}

QVariant DataStatsModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Vertical) {

			return m_fieldNames[section];
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


QStringList DataStatsModel::mimeTypes() const {

	QStringList list;
	list.push_back("application/datasource-field");
	return list;
}

QMimeData* DataStatsModel::mimeData(const QModelIndexList& indexes) const {

	QMimeData* mimeData = new QMimeData();
	mimeData->setText(DatabaseServices::GetFormattedDBName(m_db) + ":" + m_tableName + ":" + m_fieldNames[indexes.front().row()]);
	return mimeData;
}

Qt::ItemFlags DataStatsModel::flags(const QModelIndex & index) const {

	Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

	if (index.isValid()) {

		return Qt::ItemIsDragEnabled | defaultFlags;
	}
	else {

		return defaultFlags;
	}
}