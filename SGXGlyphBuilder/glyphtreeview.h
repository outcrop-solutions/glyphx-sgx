#ifndef GLYPHTREEVIEW_H
#define GLYPHTREEVIEW_H

#include <QtWidgets/QTreeView>

class GlyphTreeView : public QTreeView
{
    Q_OBJECT

public:
    GlyphTreeView(QWidget *parent = 0);
    ~GlyphTreeView();

protected:
    virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    
};

#endif // GLYPHTREEVIEW_H
