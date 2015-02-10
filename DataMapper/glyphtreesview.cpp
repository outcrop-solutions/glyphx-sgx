#include "glyphtreesview.h"
#include "glyphpropertieswidget.h"
#include "singlewidgetdialog.h"
#include "roledatafilterproxymodel.h"

GlyphTreesView::GlyphTreesView(DataTransformModel* sourceModel, QWidget *parent)
	: SynGlyphX::TreeView(parent),
	m_sourceModel(sourceModel)
{
	SynGlyphX::RoleDataFilterProxyModel* filterModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	filterModel->setFilterRole(DataTransformModel::DataTypeRole);
	filterModel->setSourceModel(m_sourceModel);
	filterModel->SetFilterData(DataTransformModel::DataType::GlyphTrees);
	setModel(filterModel);

	SetScrollOnSelection(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_addChildrenAction = m_sharedActions.AddAction(tr("Add Children"));
	QObject::connect(m_addChildrenAction, &QAction::triggered, this, &GlyphTreesView::AddChildren);

	m_sharedActions.AddSeparator();

	m_removeAction = m_sharedActions.AddAction(tr("Remove"), QKeySequence::Delete);
	QObject::connect(m_removeAction, &QAction::triggered, this, &GlyphTreesView::RemoveGlyph);

	m_removeChildrenAction = m_sharedActions.AddAction(tr("Remove Children"));
	QObject::connect(m_removeChildrenAction, &QAction::triggered, this, &GlyphTreesView::RemoveChildren);

	m_sharedActions.AddSeparator();

	m_clearSelectedInputBindingsAction = m_sharedActions.AddAction(tr("Clear Input Bindings"));

	m_sharedActions.EnableActions(false);
	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_sharedActions);
}

GlyphTreesView::~GlyphTreesView()
{

}

const SynGlyphX::SharedActionList& GlyphTreesView::GetSharedActions() {

	return m_sharedActions;
}

const QAction* const GlyphTreesView::GetClearSelectedInputBindingsAction() const {

	return m_clearSelectedInputBindingsAction;
}

void GlyphTreesView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	SynGlyphX::TreeView::selectionChanged(selected, deselected);
	EnableActions();
	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	emit SelectionChangedSourceModel(filterModel->mapSelectionToSource(selected), filterModel->mapSelectionToSource(deselected));
}

void GlyphTreesView::EnableActions() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		m_addChildrenAction->setEnabled(true);
		m_removeAction->setEnabled(true);
		m_removeChildrenAction->setEnabled(model()->rowCount(selected.front()) > 0);
		m_clearSelectedInputBindingsAction->setEnabled(true);
	}
	else {

		m_sharedActions.EnableActions(false);
	}
}

void GlyphTreesView::RemoveGlyph() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		const QModelIndex& index = selected.front();
		model()->removeRow(index.row(), index.parent());
	}
}

void GlyphTreesView::RemoveChildren() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {
		
		const QModelIndex& parent = selected.front();
		model()->removeRows(0, model()->rowCount(parent), parent);
	}
}

void GlyphTreesView::AddChildren() {

	SynGlyphX::GlyphPropertiesWidget* singleGlyphWidget = new SynGlyphX::GlyphPropertiesWidget(SynGlyphX::GlyphPropertiesWidget::ShowOnTop | SynGlyphX::GlyphPropertiesWidget::EnabledSpinBox, this);
	singleGlyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::s_defaultGlyph, true);

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, singleGlyphWidget, this);
	dialog.setWindowTitle(tr("Add Children"));
	if (dialog.exec() == QDialog::Accepted) {

		SynGlyphX::Glyph glyph;
		singleGlyphWidget->SetGlyphFromWidget(glyph);
		SynGlyphX::DataMappingGlyph minMaxGlyph(glyph);

		const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

		if (!selectedItems.isEmpty()) {

			SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
			m_sourceModel->AddChildGlyph(filterModel->mapToSource(selectedItems.front()), minMaxGlyph, singleGlyphWidget->GetNumberOfChildren());
		}
	}
}

void GlyphTreesView::SelectLastGlyphTreeRoot() {

	selectionModel()->select(model()->index(m_sourceModel->GetDataMapping()->GetGlyphGraphs().size() - 1, 0), QItemSelectionModel::ClearAndSelect);
}