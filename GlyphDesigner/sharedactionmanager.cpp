#include "sharedactionmanager.h"
#include "singleglyphwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include "glyph.h"
#include "data/nptypes.h"
#include "application.h"
#include <algorithm>

SharedActionManager::SharedActionManager(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QObject *parent)
    : QObject(parent),
    m_selectionModel(selectionModel),
    m_model(model)
{
    CreateGlyphActions();
    CreateEditActions();

    CreatePropertiesDialog();
    CreateAddChildrenDialog();
    EnableActions();

    QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &SharedActionManager::SelectionChanged);
}

SharedActionManager::~SharedActionManager()
{

}

void SharedActionManager::CreateGlyphActions() {

    m_addChildrenAction = new QAction(tr("Add Children"), this);
    QObject::connect(m_addChildrenAction, &QAction::triggered, this, &SharedActionManager::AddChildren);

    m_glyphActions.append(m_addChildrenAction);
}

void SharedActionManager::CreateEditActions() {

    m_cutAction = new QAction(tr("Cut"), this);
    m_cutAction->setShortcut(QKeySequence::Cut);
    //QObject::connect(m_cutAction, &QAction::triggered, m_model, &GlyphTreeModel::CutToClipboard);
    m_copyAction = new QAction(tr("Copy"), this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    //QObject::connect(m_copyAction, &QAction::triggered, m_model, &GlyphTreeModel::CopyToClipboard);
    m_pasteAction = new QAction(tr("Paste"), this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    //QObject::connect(m_pasteAction, &QAction::triggered, m_model, &GlyphTreeModel::PasteFromClipboard);
    m_pasteAsChildAction = new QAction(tr("Paste As Child"), this);
    //QObject::connect(m_pasteAction, &QAction::triggered, m_model, &GlyphTreeModel::PasteChildFromClipboard);

    m_deleteAction = new QAction(tr("Delete"), this);
    m_deleteAction->setShortcut(QKeySequence::Delete);
    QObject::connect(m_deleteAction, &QAction::triggered, this, &SharedActionManager::DeleteSelected);
    m_deleteChildrenAction = new QAction(tr("Delete Children"), this);
    QObject::connect(m_deleteChildrenAction, &QAction::triggered, this, &SharedActionManager::DeleteChildrenFromSelected);
    m_propertiesAction = new QAction(tr("Properties"), this);
    QObject::connect(m_propertiesAction, &QAction::triggered, this, &SharedActionManager::PropertiesActivated);

    m_editActions.append(m_cutAction);
    m_editActions.append(m_copyAction);
    m_editActions.append(m_pasteAction);
    m_editActions.append(m_pasteAsChildAction);
    m_editActions.append(CreateSeparator());
    m_editActions.append(m_deleteAction);
    m_editActions.append(m_deleteChildrenAction);
    m_editActions.append(CreateSeparator());
    m_editActions.append(m_propertiesAction);
}

void SharedActionManager::CreatePropertiesDialog() {

    m_propertiesDialog = new QDialog(SynGlyphX::Application::activeWindow());
    QVBoxLayout* layout = new QVBoxLayout(m_propertiesDialog);
    m_glyphWidget = new SingleGlyphWidget(SingleGlyphWidget::ShowOnBottom, m_propertiesDialog);
    layout->addWidget(m_glyphWidget);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, m_propertiesDialog);
    layout->addWidget(buttonBox);
    m_propertiesDialog->setLayout(layout);
    m_propertiesDialog->setWindowTitle(tr("Properties"));

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, m_propertiesDialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, m_propertiesDialog, &QDialog::reject);
}

void SharedActionManager::CreateAddChildrenDialog() {

    m_addChildrenDialog = new AddChildrenDialog(SynGlyphX::Application::activeWindow());   
}

QAction* SharedActionManager::CreateSeparator() {

    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    return separator;
}

void SharedActionManager::SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

    EnableActions();
}

void SharedActionManager::EnableActions() {

    const QModelIndexList& selected = m_selectionModel->selectedIndexes();
    bool isObjectSelected = !selected.isEmpty();

    bool hasChildren = false;
    for (int j = 0; j < selected.count(); ++j) {
        if (m_model->hasChildren(selected[j])) {
            hasChildren = true;
            break;
        }
    }

    for (int i = 0; i < m_glyphActions.length(); ++i) {
        m_glyphActions[i]->setEnabled(isObjectSelected);
    }

    bool areMultipleObjectsSelected = (selected.count() > 1);

    if (isObjectSelected) {
        const QModelIndex& index = selected.last();
        bool isRootObjectOnlySelected = (index.internalPointer() == m_model->GetRootGlyph()) && !areMultipleObjectsSelected;

        //m_cutAction->setEnabled(!isRootObjectSelected);
        //m_copyAction->setEnabled(true);
        //m_pasteAction->setEnabled(!m_model->IsClipboardEmpty());
        //m_pasteAsChildAction->setEnabled(!m_model->IsClipboardEmpty());
        m_deleteAction->setEnabled(!isRootObjectOnlySelected);
        m_deleteChildrenAction->setEnabled(hasChildren);
        m_propertiesAction->setEnabled(true);
    }
    else {
        for (int i = 0; i < m_editActions.length(); ++i) {
            m_editActions[i]->setEnabled(false);
        }
    }
}

void SharedActionManager::PropertiesActivated() {
    
    const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
    
	const QModelIndex& index = selectedItems.back();
	pNPnode node = static_cast<pNPnode>(index.internalPointer());
    boost::shared_ptr<SynGlyphX::GlyphProperties> oldGlyph(new SynGlyphX::GlyphProperties(node));
    m_glyphWidget->SetWidgetFromGlyph(oldGlyph, index.parent().isValid());
	m_glyphWidget->SetNumberOfChildren(node->childCount);
    if (m_propertiesDialog->exec() == QDialog::Accepted) {
        boost::shared_ptr<SynGlyphX::GlyphProperties> newGlyph(new SynGlyphX::GlyphProperties());
        m_glyphWidget->SetGlyphFromWidget(newGlyph);
        m_model->UpdateNodes(selectedItems, newGlyph, GlyphTreeModel::FindUpdates(oldGlyph, newGlyph));
    }
}

void SharedActionManager::DeleteSelected() {

    while (m_selectionModel->hasSelection()) {
        const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
        m_model->removeRow(selectedItems.back().row(), selectedItems.back().parent());
    }
}

void SharedActionManager::DeleteChildrenFromSelected() {

    //const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
    QModelIndexList selectedItems = m_selectionModel->selectedIndexes();
    std::sort(selectedItems.begin(), selectedItems.end(), GlyphTreeModel::GreaterBranchLevel);
    for (int i = 0; i < selectedItems.length(); ++i) {
        m_model->removeRows(0, m_model->rowCount(selectedItems[i]), selectedItems[i]);
    }
}

void SharedActionManager::AddChildren() {

    const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();

    m_addChildrenDialog->SetDialogFromGlyph(SynGlyphX::GlyphProperties::GetTemplate());
    if (m_addChildrenDialog->exec() == QDialog::Accepted) {
        boost::shared_ptr<SynGlyphX::GlyphProperties> glyph(new SynGlyphX::GlyphProperties());
        m_addChildrenDialog->SetGlyphFromDialog(glyph);
        for (int i = 0; i < selectedItems.length(); ++i) {
            m_model->AppendChild(selectedItems[i], glyph, m_addChildrenDialog->GetNumberOfChildren());
        }
    }
}

const QList<QAction*>& SharedActionManager::GetGlyphActions() const {

    return m_glyphActions;
}

const QList<QAction*>& SharedActionManager::GetEditActions() const {

    return m_editActions;
}