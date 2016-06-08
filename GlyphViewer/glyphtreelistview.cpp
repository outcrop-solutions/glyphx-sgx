#include "glyphtreelistview.h"
#include <QtWidgets/QMessageBox>
#include "application.h"
#include "glyphforestmodel.h"
#include "allglyphpropertieswidget.h"
#include "singlewidgetdialog.h"
#include "textglyphpropertieswidget.h"

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

	QAction* propertiesAction = m_actions.AddAction(tr("Properties"));
	QObject::connect(propertiesAction, &QAction::triggered, this, &GlyphTreeListView::OnPropertiesActivated);

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
	bool anyURLsOpened = forestModel->OpenURLs(selectionModel()->selectedIndexes());
	if (!anyURLsOpened) {

		QMessageBox::information(this, tr("No URLs Opened"), tr("No files or URLs were opened since none of the selected objects had a file or URL associated with them."));
	}
}

void GlyphTreeListView::OnPropertiesActivated() {

	const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();
	if (!selectedItems.empty()) {

		const QModelIndex& index = selectedItems.back();
		SynGlyphXANTz::GlyphForestModel* forestModel = dynamic_cast<SynGlyphXANTz::GlyphForestModel*>(model());
		SynGlyphX::Glyph glyph = forestModel->GetGlyphAtIndex(index);

		SynGlyphX::TextGlyphPropertiesWidget* glyphPropertiesWidget = new SynGlyphX::TextGlyphPropertiesWidget(this);
		glyphPropertiesWidget->SetWidget(QString::fromStdWString(glyph.GetTag()), QString::fromStdWString(glyph.GetURL()), QString::fromStdWString(glyph.GetDescription()));
		glyphPropertiesWidget->SetReadOnly(true);

		SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok, glyphPropertiesWidget, this);
		dialog.setWindowTitle(tr("Glyph Properties"));
		dialog.exec();
	}
}