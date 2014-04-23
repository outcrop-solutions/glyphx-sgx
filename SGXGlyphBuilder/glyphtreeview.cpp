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
    scrollTo(selected.indexes()[0]);
}
