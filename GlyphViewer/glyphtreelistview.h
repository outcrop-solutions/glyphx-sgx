#ifndef GLYPHTREELISTVIEW_H
#define GLYPHTREELISTVIEW_H

#include "treeview.h"

class GlyphTreeListView : public SynGlyphX::TreeView
{
	Q_OBJECT

public:
	GlyphTreeListView(QWidget *parent = 0);
	~GlyphTreeListView();

protected:
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	
};

#endif // GLYPHTREELISTVIEW_H
