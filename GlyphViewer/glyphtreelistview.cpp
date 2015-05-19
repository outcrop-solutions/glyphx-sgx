#include "glyphtreelistview.h"
#include <QtWidgets/QMessageBox>
#include "application.h"

GlyphTreeListView::GlyphTreeListView(QWidget *parent)
	: SynGlyphX::TreeView(parent),
	m_itemFocusSelectionModel(nullptr)
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setDragEnabled(false);
	SetScrollOnSelection(true);
	setContextMenuPolicy(Qt::ActionsContextMenu);
}

GlyphTreeListView::~GlyphTreeListView()
{

}

void GlyphTreeListView::SetItemFocusSelectionModel(SynGlyphX::ItemFocusSelectionModel* itemFocusSelectionModel) {

	m_itemFocusSelectionModel = itemFocusSelectionModel;
	setSelectionModel(itemFocusSelectionModel);
}

QItemSelectionModel::SelectionFlags GlyphTreeListView::selectionCommand(const QModelIndex& index, const QEvent* event) const {

	QItemSelectionModel::SelectionFlags selectionFlags = SynGlyphX::TreeView::selectionCommand(index, event);

	if (m_itemFocusSelectionModel != nullptr) {

		if (selectionFlags.testFlag(QItemSelectionModel::SelectionFlag::ClearAndSelect)) {

			m_itemFocusSelectionModel->SetFocus(index, SynGlyphX::ItemFocusSelectionModel::FocusFlag::ClearAndFocus);
		}
		else if (selectionFlags.testFlag(QItemSelectionModel::SelectionFlag::Toggle)) {

			m_itemFocusSelectionModel->SetFocus(index, SynGlyphX::ItemFocusSelectionModel::FocusFlag::Toggle);
		}
		else if (selectionFlags.testFlag(QItemSelectionModel::SelectionFlag::Select)) {

			m_itemFocusSelectionModel->SetFocus(index, SynGlyphX::ItemFocusSelectionModel::FocusFlag::Focus);
		}
		else if (selectionFlags.testFlag(QItemSelectionModel::SelectionFlag::Deselect)) {

			m_itemFocusSelectionModel->SetFocus(index, SynGlyphX::ItemFocusSelectionModel::FocusFlag::Unfocus);
		}
		else if (selectionFlags.testFlag(QItemSelectionModel::SelectionFlag::Clear)) {

			m_itemFocusSelectionModel->SetFocus(index, SynGlyphX::ItemFocusSelectionModel::FocusFlag::Clear);
		}
	}

	return selectionFlags;
}