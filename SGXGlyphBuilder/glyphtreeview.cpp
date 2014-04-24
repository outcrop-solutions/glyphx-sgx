#include "glyphtreeview.h"

GlyphTreeView::GlyphTreeView(QWidget *parent)
    : QTreeView(parent)
{
    
}

GlyphTreeView::~GlyphTreeView()
{

}

void GlyphTreeView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

    QTreeView::selectionChanged(selected, deselected);
    const QModelIndexList& indicies = selected.indexes();
    if ((indicies.length() > 0) && (indicies[0].isValid())) {
        scrollTo(indicies[0]);
    }
}
