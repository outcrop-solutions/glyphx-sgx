#ifndef DATASOURCESTATSWIDGET_H
#define DATASOURCESTATSWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtSql/QSqlDatabase>
#include <QtWidgets/QTableView>

class DataSourceStatsWidget : public QTabWidget
{
	Q_OBJECT

public:
	DataSourceStatsWidget(QWidget *parent = 0);
	~DataSourceStatsWidget();

	void RebuildStatsViews();

private:
	QList<QTableView*> m_statViews;
};

#endif // DATASOURCESTATSWIDGET_H
