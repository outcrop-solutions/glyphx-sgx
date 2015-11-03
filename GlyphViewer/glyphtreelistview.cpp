#include "glyphtreelistview.h"
#include <QtWidgets/QMessageBox>
#include "application.h"
#include "glyphforestmodel.h"

GlyphTreeListView::GlyphTreeListView(QWidget *parent)
	: SynGlyphX::TreeView(parent),
	m_itemFocusSelectionModel(nullptr)
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setDragEnabled(false);
	SetScrollOnSelection(true);
	setContextMenuPolicy(Qt::ActionsContextMenu);

	QAction* openURLAction = m_actions.AddAction(tr("Open URL"), Qt::Key_U);
	QObject::connect(openURLAction, &QAction::triggered, this, &GlyphTreeListView::OnOpenURLs);

	m_actions.EnableActions(false);

	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_actions);
}

GlyphTreeListView::~GlyphTreeListView()
{

}

const SynGlyphX::SharedActionList& GlyphTreeListView::GetSharedActions() const {

	return m_actions;
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

void GlyphTreeListView::EnableActions(const QItemSelection& selected) {

	m_actions.EnableActions(!selected.empty());
}

void GlyphTreeListView::OnOpenURLs() {

	SynGlyphXANTz::GlyphForestModel* forestModel = dynamic_cast<SynGlyphXANTz::GlyphForestModel*>(model());
	forestModel->OpenURLs(selectionModel()->selectedIndexes());
}