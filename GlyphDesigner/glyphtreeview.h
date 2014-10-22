#ifndef GLYPHTREEVIEW_H
#define GLYPHTREEVIEW_H

#include <QtWidgets/QTreeView>

class GlyphTreeView : public QTreeView
{
    Q_OBJECT

public:
    GlyphTreeView(QWidget *parent = 0);
    ~GlyphTreeView();

	const QList<QAction*>& GetGlyphActions() const;
	const QList<QAction*>& GetEditActions() const;

protected:
    virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private slots:
	//void DeleteSelected();

private:
	//void CreateContextMenuActions();

	QList<QAction*> m_glyphActions;
	QList<QAction*> m_editActions;
};

#endif // GLYPHTREEVIEW_H
