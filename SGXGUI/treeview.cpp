#include "treeview.h"
#include "sharedactionlist.h"
#include <QtGui/QMouseEvent>

namespace SynGlyphX {

	TreeView::TreeView(QWidget *parent)
		: QTreeView(parent),
		m_scrollOnSelection(false),
		m_preventMouseFromCausingUnselect(false)
	{
		m_expandAllAction = new QAction(tr("Expand All"), this);
		m_expandAllAction->setEnabled(false);
		QObject::connect(m_expandAllAction, &QAction::triggered, this, &TreeView::OnExpandAll);
		addAction(m_expandAllAction);

		m_collapseAllAction = new QAction(tr("Collapse All"), this);
		m_collapseAllAction->setEnabled(false);
		QObject::connect(m_collapseAllAction, &QAction::triggered, this, &TreeView::OnCollapseAll);
		addAction(m_collapseAllAction);
	}

	TreeView::~TreeView()
	{

	}

	void TreeView::SetScrollOnSelection(bool scrollOnSelection) {

		m_scrollOnSelection = scrollOnSelection;
	}

	void TreeView::SetPreventMouseFromCausingUnselect(bool prevent) {

		m_preventMouseFromCausingUnselect = prevent;
	}

	void TreeView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

		bool isSelectedNotEmpty = !selected.empty();
		QTreeView::selectionChanged(selected, deselected);
		m_expandAllAction->setEnabled(isSelectedNotEmpty);
		m_collapseAllAction->setEnabled(isSelectedNotEmpty);

		if (m_scrollOnSelection && isSelectedNotEmpty) {

			const QModelIndexList& indicies = selected.indexes();		
			const QModelIndex& index = indicies.back();
			if (index.isValid()) {
					
				scrollTo(index);
			}
		}
	}

	void TreeView::OnExpandAll() {

		SetExpandedOnSelection(true);
	}

	void TreeView::OnCollapseAll() {

		SetExpandedOnSelection(false);
	}

	void TreeView::SetExpandedOnSelection(bool expand) {

		QItemSelectionModel* itemSelectionModel = selectionModel();

		if ((itemSelectionModel != nullptr) && !itemSelectionModel->selection().empty()) {

			Q_FOREACH(const QModelIndex& index, itemSelectionModel->selectedIndexes()) {

				SetExpandedRecursive(index, expand);
			}
		}
	}

	void TreeView::SetExpandedRecursive(const QModelIndex& index, bool expand) {

		QAbstractItemModel* itemModel = model();

		setExpanded(index, expand);
		for (int i = 0; i < itemModel->rowCount(index); ++i) {

			SetExpandedRecursive(itemModel->index(i, 0, index), expand);
		}
	}

	void TreeView::mouseReleaseEvent(QMouseEvent* event) {

		//Prevent mouse from causing an unselect
		if (m_preventMouseFromCausingUnselect && (event->button() == Qt::LeftButton) && (!indexAt(event->pos()).isValid())) {

			return;
		}

		QTreeView::mouseReleaseEvent(event);
	}

} //namespace SynGlyphX