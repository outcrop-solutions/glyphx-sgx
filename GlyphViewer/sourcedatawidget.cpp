#include "sourcedatawidget.h"
#include <QtGui/QCloseEvent>
#include <set>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlError>

SourceDataWidget::SourceDataWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QTabWidget(parent),
	m_model(model),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	setWindowTitle(tr("Source Data Of Selected Glyphs"));
	QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &SourceDataWidget::OnSelectionChanged);
	UpdateTables();
}

SourceDataWidget::~SourceDataWidget()
{

}

void SourceDataWidget::closeEvent(QCloseEvent* event) {

	if (parentWidget() == nullptr) {

		setVisible(false);
		event->ignore();
		emit WindowHidden();
	}
}

void SourceDataWidget::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	UpdateTables();
}

void SourceDataWidget::UpdateTables() {

	clear();
	if (!m_selectionModel->selection().empty()) {

		SynGlyphX::SourceDataCache::IndexSet selectedDataRows;
		Q_FOREACH(const QModelIndex& index, m_selectionModel->selection().indexes()) {

			selectedDataRows.insert(GetRootRow(index));
		}

		SynGlyphX::SourceDataCache::IndexSetMap indexSets = m_sourceDataCache->SplitIndexSet(selectedDataRows);
		const SynGlyphX::SourceDataCache::TableNameMap& formattedNames = m_sourceDataCache->GetFormattedNames();

		for (auto indexSet : indexSets) {

			QTableView* tableView = new QTableView(this);
			QSqlQueryModel* queryModel = new QSqlQueryModel(this);
			
			QStringList columns = m_sourceDataCache->GetColumnsForTable(indexSet.first);
			QSqlQuery query = m_sourceDataCache->CreateSelectQueryForIndexSet(indexSet.first, columns, indexSet.second);
			query.exec();
			queryModel->setQuery(query);
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

unsigned long SourceDataWidget::GetRootRow(const QModelIndex& index) const {

	QModelIndex ancestor = index;
	while (ancestor.parent().isValid()) {

		ancestor = ancestor.parent();
	}

	return ancestor.row();
}