#include "sourcedatawidget.h"
#include <QtGui/QCloseEvent>
#include <set>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlError>
#include <QtCore/QSettings>

SourceDataWidget::SourceDataWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, QWidget *parent)
	: QTabWidget(parent),
	//m_model(model),
	//m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	setWindowTitle(tr("Source Data Of Selected Glyphs"));
	ReadSettings();
}

SourceDataWidget::~SourceDataWidget()
{
	WriteSettings();
}

void SourceDataWidget::closeEvent(QCloseEvent* event) {

	if (parentWidget() == nullptr) {

		WriteSettings();
		setVisible(false);
		event->ignore();
		emit WindowHidden();
	}
}

void SourceDataWidget::UpdateTables(const SynGlyphX::SourceDataCache::IndexSetMap& dataIndexes) {

	clear();
	if (!dataIndexes.empty()) {

		const SynGlyphX::SourceDataCache::TableNameMap& formattedNames = m_sourceDataCache->GetFormattedNames();

		for (auto indexSet : dataIndexes) {

			QTableView* tableView = new QTableView(this);
			QSqlQueryModel* queryModel = new QSqlQueryModel(this);
			
			SynGlyphX::TableColumns columns = m_sourceDataCache->GetColumnsForTable(indexSet.first);
			SynGlyphX::SharedSQLQuery query = m_sourceDataCache->CreateSelectQueryForIndexSet(indexSet.first, columns, indexSet.second);
			query->exec();
			queryModel->setQuery(*query.data());
			if (queryModel->lastError().isValid()) {

				throw std::exception("Failed to set SQL query for source data widget.");
			}

			tableView->verticalHeader()->setVisible(false);

			tableView->setModel(queryModel);
			tableView->resizeColumnsToContents();
			tableView->resizeRowsToContents();
			
			addTab(tableView, formattedNames.at(indexSet.first));
		}
	}
}

void SourceDataWidget::ReadSettings() {

	QSettings settings;

	settings.beginGroup("SourceDataWidget");
	resize(settings.value("size", QSize(700, 400)).toSize());
	restoreGeometry(settings.value("geometry").toByteArray());
	settings.endGroup();
}

void SourceDataWidget::WriteSettings() {

	QSettings settings;
	settings.beginGroup("SourceDataWidget");
	settings.setValue("size", size());
	settings.setValue("geometry", saveGeometry());
	settings.endGroup();
}