#include "sharedactionmanager.h"
#include "singleglyphwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include "glyph.h"
#include "data/nptypes.h"
#include "application.h"

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
    //QObject::connect(m_pasteAction, &QAction::triggered, m_model, &GlyphTreeModel::PasteFromClipboard);

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

    for (int i = 0; i < m_glyphActions.length(); ++i) {
        m_glyphActions[i]->setEnabled(isObjectSelected);
    }

    if (isObjectSelected) {
        const QModelIndex& index = selected.last();
        bool isRootObjectSelected = (index.internalPointer() == m_model->GetRootGlyph());

        //m_cutAction->setEnabled(!isRootObjectSelected);
        //m_copyAction->setEnabled(true);
        //m_pasteAction->setEnabled(!m_model->IsClipboardEmpty());
        //m_pasteAsChildAction->setEnabled(!m_model->IsClipboardEmpty());
        m_deleteAction->setEnabled(!isRootObjectSelected);
        m_deleteChildrenAction->setEnabled(m_model->hasChildren(index));
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
    
    boost::shared_ptr<SynGlyphX::Glyph> oldGlyph(new SynGlyphX::Glyph(static_cast<pNPnode>(selectedItems.back().internalPointer())));
    m_glyphWidget->SetWidgetFromGlyph(oldGlyph);
    if (m_propertiesDialog->exec() == QDialog::Accepted) {
        boost::shared_ptr<SynGlyphX::Glyph> newGlyph(new SynGlyphX::Glyph());
        m_glyphWidget->SetGlyphFromWidget(newGlyph);
        m_model->UpdateNodes(selectedItems, newGlyph, GlyphTreeModel::FindUpdates(oldGlyph, newGlyph));
    }
}

void SharedActionManager::DeleteSelected() {

    const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
    if (!selectedItems.isEmpty()) {
        m_model->DeleteNode(selectedItems.last());
    }
}

void SharedActionManager::DeleteChildrenFromSelected() {

    const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
    if (!selectedItems.isEmpty()) {
        m_model->DeleteChildren(selectedItems.last());
    }
}

void SharedActionManager::AddChildren() {

    const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();

    m_addChildrenDialog->SetDialogFromGlyph(SynGlyphX::Glyph::GetTemplate());
    if (m_addChildrenDialog->exec() == QDialog::Accepted) {
        boost::shared_ptr<SynGlyphX::Glyph> glyph(new SynGlyphX::Glyph());
        m_addChildrenDialog->SetGlyphFromDialog(glyph);
        m_model->AppendChild(selectedItems.back(), glyph, glyph->GetNumberOfChildren());
    }
}

const QList<QAction*>& SharedActionManager::GetGlyphActions() const {

    return m_glyphActions;
}

const QList<QAction*>& SharedActionManager::GetEditActions() const {

    return m_editActions;
}