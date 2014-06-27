#ifndef DATASTATSMODEL_H
#define DATASTATSMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtSql/QSqlQuery>

class DataStatsModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	DataStatsModel(const QSqlDatabase& db, const QString& tableName, QObject *parent = 0);
	~DataStatsModel();

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual QStringList mimeTypes() const;
	virtual QMimeData* mimeData(const QModelIndexList& indexes) const;

private:
	QList<QStringList> m_stats;
	QStringList m_fieldNames;
};

#endif // DATASTATSMODEL_H
