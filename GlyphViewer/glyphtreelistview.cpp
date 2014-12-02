#include "glyphtreelistview.h"

GlyphTreeListView::GlyphTreeListView(QWidget *parent)
	: SynGlyphX::TreeView(parent)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setDragEnabled(false);
	SetScrollOnSelection(true);
	setContextMenuPolicy(Qt::ActionsContextMenu);
}

GlyphTreeListView::~GlyphTreeListView()
{

}

void GlyphTreeListView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	SynGlyphX::TreeView::selectionChanged(selected, deselected);
}