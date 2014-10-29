#include "glyphtreesview.h"

GlyphTreesView::GlyphTreesView(QWidget *parent)
	: QTreeView(parent)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_removeGlyphTreeAction = m_sharedActions.AddAction(tr("Remove"), QKeySequence::Delete);
	QObject::connect(m_removeGlyphTreeAction, &QAction::triggered, this, &GlyphTreesView::RemoveGlyph);

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