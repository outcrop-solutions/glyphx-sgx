#include "sourcedatawidget.h"
#include <QtGui/QCloseEvent>
#include <set>

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
	}
}

unsigned int SourceDataWidget::GetRootRow(const QModelIndex& index) const {

	QModelIndex ancestor = index.parent();
	while (ancestor.parent().isValid()) {

		ancestor = ancestor.parent();
	}

	return ancestor.row();
}