#include "glyphtreesview.h"

GlyphTreesView::GlyphTreesView(QWidget *parent)
	: SynGlyphX::TreeView(parent)
{
	SetScrollOnSelection(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_removeGlyphTreeAction = m_sharedActions.AddAction(tr("Remove"), QKeySequence::Delete);
	QObject::connect(m_removeGlyphTreeAction, &QAction::triggered, this, &GlyphTreesView::RemoveGlyph);

	m_sharedActions.AddSeparator();

	m_clearSelectedInputBindingsAction = m_sharedActions.AddAction(tr("Clear Input Bindings"));

	m_sharedActions.EnableActions(false);
	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_sharedActions);
}

GlyphTreesView::~GlyphTreesView()
{

}

const SynGlyphX::SharedActionList& GlyphTreesView::GetSharedActions() {

	return m_sharedActions;
}

const QAction* const GlyphTreesView::GetClearSelectedInputBindingsAction() const {

	return m_clearSelectedInputBindingsAction;
}

void GlyphTreesView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	SynGlyphX::TreeView::selectionChanged(selected, deselected);
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