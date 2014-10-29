#ifndef GLYPHTREESVIEW_H
#define GLYPHTREESVIEW_H

#include <QtWidgets/QTreeView>
#include <QtWidgets/QAction>
#include "sharedactionlist.h"

class GlyphTreesView : public QTreeView
{
	Q_OBJECT

public:
	GlyphTreesView(QWidget *parent = 0);
	~GlyphTreesView();

	const SynGlyphX::SharedActionList& GetSharedActions();

protected:
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private slots:
	void RemoveGlyph();

private:
	void EnableActions();

	SynGlyphX::SharedActionList m_sharedActions;
	QAction* m_removeGlyphTreeAction;
	QAction* m_clearInputBindingsAction;
};

#endif // GLYPHTREESVIEW_H
