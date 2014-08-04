#ifndef DATASOURCESTATSWIDGET_H
#define DATASOURCESTATSWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtSql/QSqlDatabase>
#include <QtWidgets/QTableView>
#include "datatransform.h"

class DataSourceStatsWidget : public QTabWidget
{
	Q_OBJECT

public:
    DataSourceStatsWidget(boost::shared_ptr<const SynGlyphX::DataTransform> transform, QWidget *parent = 0);
	~DataSourceStatsWidget();

	void RebuildStatsViews();
	void AddNewStatsViews(const unsigned int numNewDatasources);
	void ClearTabs();

private:
    void CreateTablesFromDatasource(const boost::uuids::uuid& id, QSqlDatabase& db, const std::vector<std::wstring>& tables);
	void CreateTableView(const boost::uuids::uuid& id, const QSqlDatabase& db, const QString& tableName);
	QList<QTableView*> m_statViews;
    boost::shared_ptr<const SynGlyphX::DataTransform> m_transform;
};

#endif // DATASOURCESTATSWIDGET_H
