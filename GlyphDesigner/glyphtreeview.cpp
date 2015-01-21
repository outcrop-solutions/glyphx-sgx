#include "glyphtreeview.h"
#include "singlewidgetdialog.h"
#include "singleglyphwidget.h"

GlyphTreeView::GlyphTreeView(SynGlyphXANTz::MinMaxGlyphTreeModel* model, SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent)
	: SynGlyphX::TreeView(parent),
	m_model(model),
	m_glyphTreeType(glyphTreeType)
{
	setModel(m_model);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(false);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
	SetScrollOnSelection(true);

	CreateContextMenuActions();
}

GlyphTreeView::~GlyphTreeView()
{

}

const SynGlyphX::SharedActionList& GlyphTreeView::GetGlyphActions() const {

	return m_glyphActions;
}

const SynGlyphX::SharedActionList& GlyphTreeView::GetEditActions() const {

	return m_editActions;
}

void GlyphTreeView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	SynGlyphX::TreeView::selectionChanged(selected, deselected);
	EnableActions();
}

void GlyphTreeView::CreateContextMenuActions() {

	//Edit Actions

	//m_cutAction = m_editActions.AddAction(tr("Cut"), QKeySequence::Cut);
	//QObject::connect(m_cutAction, &QAction::triggered, m_model, &GlyphTreeModel::CutToClipboard);

	//m_copyAction = m_editActions.AddAction(tr("Copy"), QKeySequence::Copy);
	//QObject::connect(m_copyAction, &QAction::triggered, m_model, &GlyphTreeModel::CopyToClipboard);

	//m_pasteAction = m_editActions.AddAction(tr("Paste"), QKeySequence::Paste);
	//QObject::connect(m_pasteAction, &QAction::triggered, m_model, &GlyphTreeModel::PasteFromClipboard);

	//m_pasteAsChildAction = m_editActions.AddAction(tr("Paste As Child"));
	//QObject::connect(m_pasteAction, &QAction::triggered, m_model, &GlyphTreeModel::PasteChildFromClipboard);

	//m_editActions.AddSeparator();

	m_deleteAction = m_editActions.AddAction(tr("Delete"), QKeySequence::Delete);
	QObject::connect(m_deleteAction, &QAction::triggered, this, &GlyphTreeView::DeleteSelected);

	m_deleteChildrenAction = m_editActions.AddAction(tr("Delete Children"));
	QObject::connect(m_deleteChildrenAction, &QAction::triggered, this, &GlyphTreeView::DeleteChildrenFromSelected);

	m_editActions.AddSeparator();

	m_propertiesAction = m_editActions.AddAction(tr("Properties"));
	QObject::connect(m_propertiesAction, &QAction::triggered, this, &GlyphTreeView::PropertiesActivated);

	//Glyph Actions

	m_addChildrenAction = m_glyphActions.AddAction(tr("Add Children"));
	QObject::connect(m_addChildrenAction, &QAction::triggered, this, &GlyphTreeView::AddChildren);

	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_glyphActions);
	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_editActions);
	
	setContextMenuPolicy(Qt::ActionsContextMenu);
}

void GlyphTreeView::DeleteSelected() {

	QItemSelectionModel* glyphTreeSelectionModel = selectionModel();
	while (glyphTreeSelectionModel->hasSelection()) {

		const QModelIndexList& selectedItems = glyphTreeSelectionModel->selectedIndexes();
		const QModelIndex& selectedItem = selectedItems.back();

		//Only delete selected node if it isn't the root node
		if (selectedItem.parent().isValid()) {
		
			m_model->removeRow(selectedItem.row(), selectedItem.parent());
		}
	}
}

void GlyphTreeView::DeleteChildrenFromSelected() {

	QModelIndexList selectedItems = selectionModel()->selectedIndexes();
	std::sort(selectedItems.begin(), selectedItems.end(), SynGlyphXANTz::MinMaxGlyphTreeModel::GreaterBranchLevel);
	for (int i = 0; i < selectedItems.length(); ++i) {

		m_model->removeRows(0, m_model->rowCount(selectedItems[i]), selectedItems[i]);
	}
}

void GlyphTreeView::PropertiesActivated() {

	const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

	const QModelIndex& index = selectedItems.back();
	SynGlyphX::Glyph oldGlyph;
	if (m_glyphTreeType == SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType::Max) {

		oldGlyph = m_model->GetMinMaxGlyph(index)->GetMaxGlyph();
	}
	else {

		oldGlyph = m_model->GetMinMaxGlyph(index)->GetMinGlyph();
	}

	SingleGlyphWidget* singleGlyphWidget = new SingleGlyphWidget(SingleGlyphWidget::ShowOnBottom, this);
	singleGlyphWidget->SetWidgetFromGlyph(oldGlyph, index.parent().isValid());
	singleGlyphWidget->SetNumberOfChildren(m_model->rowCount(index));

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, singleGlyphWidget, this);
	dialog.setWindowTitle(tr("Glyph Properties"));
	if (dialog.exec() == QDialog::Accepted) {

		SynGlyphX::Glyph newGlyph;
		singleGlyphWidget->SetGlyphFromWidget(newGlyph);
		m_model->UpdateGlyphs(selectedItems, m_glyphTreeType, newGlyph);
	}
}

void GlyphTreeView::AddChildren() {

	const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

	SingleGlyphWidget* singleGlyphWidget = new SingleGlyphWidget(SingleGlyphWidget::ShowOnTop | SingleGlyphWidget::EnabledSpinBox, this);
	singleGlyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::s_defaultGlyph, true);

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, singleGlyphWidget, this);
	dialog.setWindowTitle(tr("Add Children"));
	if (dialog.exec() == QDialog::Accepted) {

		SynGlyphX::Glyph glyph;
		singleGlyphWidget->SetGlyphFromWidget(glyph);
		SynGlyphX::DataMappingGlyph minMaxGlyph(glyph);
		for (int i = 0; i < selectedItems.length(); ++i) {

			m_model->AppendChild(selectedItems[i], minMaxGlyph, singleGlyphWidget->GetNumberOfChildren());
		}
	}
}

void GlyphTreeView::EnableActions() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	bool isObjectSelected = !selected.isEmpty();

	bool hasChildren = false;
	for (int j = 0; j < selected.count(); ++j) {
		if (m_model->hasChildren(selected[j])) {
			hasChildren = true;
			break;
		}
	}

	m_glyphActions.EnableActions(isObjectSelected);

	bool areMultipleObjectsSelected = (selected.count() > 1);

	if (isObjectSelected) {
		const QModelIndex& index = selected.last();
		bool isRootObjectOnlySelected = (!index.parent().isValid()) && !areMultipleObjectsSelected;

		//m_cutAction->setEnabled(!isRootObjectSelected);
		//m_copyAction->setEnabled(true);
		//m_pasteAction->setEnabled(!m_model->IsClipboardEmpty());
		//m_pasteAsChildAction->setEnabled(!m_model->IsClipboardEmpty());
		m_deleteAction->setEnabled(!isRootObjectOnlySelected);
		m_deleteChildrenAction->setEnabled(hasChildren);
		m_propertiesAction->setEnabled(true);
	}
	else {
		
		m_editActions.EnableActions(false);
	}
}