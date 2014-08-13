#include "glyphtreelistview.h"

GlyphTreeListView::GlyphTreeListView(QWidget *parent)
	: QTreeView(parent)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setDragEnabled(false);
}

GlyphTreeListView::~GlyphTreeListView()
{

}

void GlyphTreeListView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	QTreeView::selectionChanged(selected, deselected);
	const QModelIndexList& indicies = selected.indexes();
	if (indicies.length() > 0) {
		const QModelIndex& index = indicies.back();
		if (index.isValid()) {
			scrollTo(index);
		}
	}
}