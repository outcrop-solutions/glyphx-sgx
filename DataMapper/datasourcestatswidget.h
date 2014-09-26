#ifndef DATASOURCESTATSWIDGET_H
#define DATASOURCESTATSWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtSql/QSqlDatabase>
#include <QtWidgets/QTableView>
#include "datatransformmapping.h"

class DataSourceStatsWidget : public QTabWidget
{
	Q_OBJECT

public:
    DataSourceStatsWidget(boost::shared_ptr<const SynGlyphX::DataTransformMapping> mapping, QWidget *parent = 0);
	~DataSourceStatsWidget();

	void RebuildStatsViews();
	void AddNewStatsViews();
	void ClearTabs();

private:
    void CreateTablesFromDatasource(const boost::uuids::uuid& id, QSqlDatabase& db, const std::vector<std::wstring>& tables);
	void CreateTableView(const boost::uuids::uuid& id, const QSqlDatabase& db, const QString& tableName);
	QList<QTableView*> m_statViews;
	boost::shared_ptr<const SynGlyphX::DataTransformMapping> m_mapping;
	std::set<boost::uuids::uuid> m_datasourcesShownInTabs;
};

#endif // DATASOURCESTATSWIDGET_H
