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
    QAction* propertiesAction = new QAction(tr("Properties"), this);
    
    QObject::connect(propertiesAction, SIGNAL(triggered()), this, SLOT(PropertiesActivated()));
    m_glyphActions.append(propertiesAction);

    CreateDialogs();
    EnableActions(!m_selectionModel->selectedIndexes().isEmpty());

    QObject::connect(m_selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionChanged(const QItemSelection&, const QItemSelection&)));
}

SharedActionManager::~SharedActionManager()
{

}

void SharedActionManager::CreateDialogs() {

    m_propertiesDialog = new QDialog(SynGlyphX::Application::activeWindow());
    QVBoxLayout* layout = new QVBoxLayout(m_propertiesDialog);
    m_glyphWidget = new SingleGlyphWidget(false, m_propertiesDialog);
    layout->addWidget(m_glyphWidget);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, m_propertiesDialog);
    layout->addWidget(buttonBox);
    m_propertiesDialog->setLayout(layout);
    m_propertiesDialog->setWindowTitle(tr("Properties"));
    QObject::connect(buttonBox, SIGNAL(accepted()), m_propertiesDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), m_propertiesDialog, SLOT(reject()));
}

void SharedActionManager::SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

    EnableActions(!m_selectionModel->selectedIndexes().isEmpty());
}

void SharedActionManager::EnableActions(bool enable) {

    for (int i = 0; i < m_glyphActions.length(); ++i) {
        m_glyphActions[i]->setEnabled(enable);
    }
}

void SharedActionManager::PropertiesActivated() {
    
    const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
    
    boost::shared_ptr<SynGlyphX::Glyph> glyph(new SynGlyphX::Glyph(static_cast<pNPnode>(selectedItems.back().internalPointer())));
    m_glyphWidget->SetWidgetFromGlyph(glyph);
    if (m_propertiesDialog->exec() == QDialog::Accepted) {
        m_glyphWidget->SetGlyphFromWidget(glyph);
        m_model->UpdateNode(selectedItems.back(), glyph);
    }
}

const QList<QAction*>& SharedActionManager::GetGlyphActions() const {

    return m_glyphActions;
}