#include "sourcedatawidget.h"
#include <QtGui/QCloseEvent>
#include <set>
#include <QtWidgets/QTableView>
#include <QtSql/QSqlQueryModel>

SourceDataWidget::SourceDataWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QTabWidget(parent),
	m_model(model),
	m_sourceDataCache(sourceDataCache)
{
	setWindowTitle(tr("Source Data Of Selected Glyphs"));
	QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &SourceDataWidget::OnSelectionChanged);
	UpdateTables(selectionModel->selection());
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

	UpdateTables(selected);
}

void SourceDataWidget::UpdateTables(const QItemSelection& selected) {

	clear();
	if (!selected.empty()) {

		std::set<unsigned int> selectedDataRows;
		Q_FOREACH(const QModelIndex& index, selected.indexes()) {

			selectedDataRows.insert(GetRootRow(index));
		}

		SynGlyphX::SourceDataCache::TableQueryMap queries = m_sourceDataCache->CreateQueriesForIndicies(selectedDataRows);
		const SynGlyphX::SourceDataCache::TableNameMap& formattedNames = m_sourceDataCache->GetFormattedNames();

		for (auto table : queries) {

			QTableView* tableView = new QTableView(this);
			QSqlQueryModel* queryModel = new QSqlQueryModel(this);
			queryModel->setQuery(table.second);

			tableView->setModel(queryModel);
			addTab(tableView, formattedNames.at(table.first));
		}
	}
}

unsigned int SourceDataWidget::GetRootRow(const QModelIndex& index) const {

	QModelIndex ancestor = index.parent();
	while (ancestor.parent().isValid()) {

		ancestor = ancestor.parent();
	}

	return ancestor.row();
}