#ifndef GLYPHTREEVIEW_H
#define GLYPHTREEVIEW_H

#include "treeview.h"
#include "minmaxglyphtreemodel.h"
#include "sharedactionlist.h"

class GlyphTreeView : public SynGlyphX::TreeView
{
    Q_OBJECT

public:
	GlyphTreeView(SynGlyphXANTz::MinMaxGlyphTreeModel* model, SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent = 0);
    ~GlyphTreeView();

	const SynGlyphX::SharedActionList& GetGlyphActions() const;
	const SynGlyphX::SharedActionList& GetEditActions() const;

public slots:
	void AddChildren();

protected:
    virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private slots:
	void DeleteSelected();
	void DeleteChildrenFromSelected();
	void PropertiesActivated();

private:
	void CreateContextMenuActions();
	void CreatePropertiesDialog();
	void CreateAddChildrenDialog();
	void EnableActions();

	SynGlyphXANTz::MinMaxGlyphTreeModel* m_model;
	SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType m_glyphTreeType;

	SynGlyphX::SharedActionList m_glyphActions;
	SynGlyphX::SharedActionList m_editActions;

	//Edit menu actions
	QAction* m_cutAction;
	QAction* m_copyAction;
	QAction* m_pasteAction;
	QAction* m_pasteAsChildAction;
	QAction* m_deleteAction;
	QAction* m_deleteChildrenAction;
	QAction* m_propertiesAction;

	//Glyph menu actions
	QAction* m_addChildrenAction;
};

#endif // GLYPHTREEVIEW_H
