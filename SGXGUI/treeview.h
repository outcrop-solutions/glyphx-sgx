#ifndef SYNGLYPHX_TREEVIEW_H
#define SYNGLYPHX_TREEVIEW_H

#include "sgxgui_global.h"
#include <QtWidgets/QTreeView>
#include <QtWidgets/QAction>

namespace SynGlyphX {

	class SGXGUI_EXPORT TreeView : public QTreeView
	{
		Q_OBJECT

	public:
		TreeView(QWidget *parent);
		~TreeView();

		void SetScrollOnSelection(bool scrollOnSelection);

	protected:
		virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	private slots:
		void OnExpandAll();
		void OnCollapseAll();

	private:
		void SetExpandedOnSelection(bool expand);
		void SetExpandedRecursive(const QModelIndex& index, bool expand);

		QAction* m_expandAllAction;
		QAction* m_collapseAllAction;
		bool m_scrollOnSelection;
	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_TREEVIEW_H
