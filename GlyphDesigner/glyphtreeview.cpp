#include "glyphtreeview.h"

GlyphTreeView::GlyphTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
}

GlyphTreeView::~GlyphTreeView()
{

}

void GlyphTreeView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

    QTreeView::selectionChanged(selected, deselected);
    const QModelIndexList& indicies = selected.indexes();
    if (indicies.length() > 0) {
        const QModelIndex& index = indicies.back();
        if (index.isValid()) {
            scrollTo(index);
        }
    }
}
