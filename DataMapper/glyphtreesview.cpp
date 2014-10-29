#include "glyphtreesview.h"

GlyphTreesView::GlyphTreesView(MinMaxGlyphModel* minMaxGlyphModel, QWidget *parent)
	: QTreeView(parent)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_removeGlyphTreeAction = m_sharedActions.AddAction(tr("Remove"), QKeySequence::Delete);
	QObject::connect(m_removeGlyphTreeAction, &QAction::triggered, this, &GlyphTreesView::RemoveGlyph);

	m_sharedActions.AddSeparator();

	m_clearSelectedInputBindingsAction = m_sharedActions.AddAction(tr("Clear Input Bindings"));
	QObject::connect(m_clearSelectedInputBindingsAction, &QAction::triggered, minMaxGlyphModel, &MinMaxGlyphModel::ClearInputBindings);

	m_sharedActions.EnableActions(false);
	addActions(m_sharedActions);
}

GlyphTreesView::~GlyphTreesView()
{

}

const SynGlyphX::SharedActionList& GlyphTreesView::GetSharedActions() {

	return m_sharedActions;
}

void GlyphTreesView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	QTreeView::selectionChanged(selected, deselected);
	const QModelIndexList& indicies = selected.indexes();
	if (indicies.length() > 0) {
		const QModelIndex& index = indicies.back();
		if (index.isValid()) {
			scrollTo(index);
		}
	}

	EnableActions();
}

void GlyphTreesView::EnableActions() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		m_removeGlyphTreeAction->setEnabled(!selected.front().parent().isValid());
		m_clearSelectedInputBindingsAction->setEnabled(true);
	}
	else {

		m_sharedActions.EnableActions(false);
	}
}

void GlyphTreesView::RemoveGlyph() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		model()->removeRow(selected.front().row());
	}
}