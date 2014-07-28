#ifndef DATASTATSMODEL_H
#define DATASTATSMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtSql/QSqlQuery>
#include <boost/uuid/uuid.hpp>

class DataStatsModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	DataStatsModel(const boost::uuids::uuid& id, const QString& tableName, QObject *parent = 0);
	~DataStatsModel();

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList& indexes) const;

private:
	QList<QStringList> m_stats;
	QStringList m_fieldNames;
	QList<QVariant::Type> m_fieldTypes;
	QString m_tableName;
	boost::uuids::uuid m_id;
};

#endif // DATASTATSMODEL_H
